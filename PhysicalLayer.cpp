/*
 * PhysicalLayer.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Aaron Sikes
 */

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>		// Required for getaddrinfo
#include <netdb.h>
#include <string.h>		// Required for memset()
#include <pthread.h>
#include <sys/poll.h>		// For poll


#include "did.h"



using namespace std;

// Constructor called by client. It connects to the well-known port of the server,
// then passes control to the general physical layer code.
PhysicalLayer::PhysicalLayer(const char *hostname) {
	struct addrinfo hints;		/*  Hints for getaddrinfo */
	struct addrinfo *resolved;	/*  Result from resolving hostname */

	//  Set up hints before resolving hostname
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;         // use IPv4
	hints.ai_socktype = SOCK_STREAM;   //TCP/IP

	// Resolves hostname
	char portString[10];
	sprintf(portString, "%d", WELL_KNOWN_PORT);

	if(getaddrinfo(hostname, portString, &hints, &resolved)){
		cerr << "Couldn't resolve host." << endl;
		exit(EXIT_FAILURE);
	}
	if(resolved == NULL){
		cerr << "Couldn't resolve host." << endl;
		exit(EXIT_FAILURE);
	}

	// Creates the socket with which to connect to the server
	// Note ai_family, ai_socktype will be the same as those in hints
	if ( (sockfd = socket(resolved->ai_family, resolved->ai_socktype, resolved->ai_protocol)) < 0 ) {
		cerr << "Client: Error creating socket." << endl;
		exit(EXIT_FAILURE);
	}

	// Connects to the server
	// ai_addr is the resolved IP address of the host
	if ( connect(sockfd, resolved->ai_addr, resolved->ai_addrlen ) < 0 ) {
		cerr << "Couldn't connect() to host." << endl;
		exit(EXIT_FAILURE);
	}
	connected = true;
}


// Constructor called by server. sockfd is already connected with the client,
// so just passes control to the general physical layer code.
PhysicalLayer::PhysicalLayer(int sockfd) {
	PhysicalLayer::sockfd = sockfd;
	connected = true;
}



// Generic physical layer code
// Should send out any frames received from DL, and pass up any frames received.
// Placeholder tester code for now.
void PhysicalLayer::run(){
	int i = 0;
	while(connected){
		if(verboseDebug && i % 100 == 0) cout<<"[PhysicalLayer:run] Run loop #"<<i<<endl;
		if(!pthread_mutex_trylock(&mutSF)){
			if(!sendFrames.empty()){
				SendAFrame();
			}
			pthread_mutex_trylock(&mutSF);
			pthread_mutex_unlock(&mutSF);
		}
		ReceiveFrames();
		i++;
	}

}

// Assumes it has a mutex on mutSF.
// Sends 1 frame from sendFrames out on the wire.
void PhysicalLayer::SendAFrame(){
	if(debug) cout <<"[PhysicalLayer:SendAFrame] Function start"<<endl;
	Frame *theFrame = NULL;
	if(!sendFrames.empty()) {
		theFrame = sendFrames.front();
		sendFrames.pop();
	}
	else cerr<<"[PhysicalLayer:SendAFrame] Who's touching my queue?!"<<endl;
	pthread_mutex_unlock(&mutSF);

	if(theFrame != NULL) {
		unsigned char *cereal = theFrame->Serialize();
		int len = theFrame->payloadLength + FRAME_HEAD;
		cereal = FoldSerializedFrame(cereal, len);
		len += 2;

		///cout << "Sending a frame !!!";
		///theFrame->Print();
		if(send(sockfd, cereal, len, 0) != len){
			cout << "Crap! Couldn't send the whole frame" << endl;
		}
		if(debug){
			cout << "[PhysicalLayer:SendAFrame] Sent this frame"<<endl;
			theFrame->Print();
			cout<<"But serialized to this:"<<endl<<cereal<<endl;
		}
	}
	else cerr<<"[PhysicalLayer:SendAFrame] theFrame was NULL, weird"<<endl;
}


// Checks if a frame is waiting on the wire. Validates it and sends it on up.
void PhysicalLayer::ReceiveFrames(){
	if(verboseDebug) cout <<"[PhysicalLayer:ReceiveFrames] Function start"<<endl;
	struct pollfd pfd;
	pfd.fd = sockfd;
	pfd.events = POLLIN;		// Use poll, so we don't block on the recv call
	if(poll(&pfd, 1, 10) > 0){
		if(debug) cout <<"[PhysicalLayer:ReceiveFrames] Poll says there's stuff"<<endl;
		int len = MAX_FRAME + FRAME_HEAD + FRAME_TAIL;
		char *incoming = (char *) malloc(len);
		int recvd = recv(sockfd, (void*)incoming, len, 0);
		if(debug) cout <<"[PhysicalLayer:ReceiveFrames] Received "<<recvd<<" bytes from the wire"<<endl;
		if(recvd == 0){
			if(debug) cout<<"[PhysicalLayer:ReceiveFrames] Connection was closed"<<endl;
			connected = false;
			return;
		}
		if(debug) cout<<"[PhysicalLayer:ReceiveFrames] Got these bytes off the wire:"<<endl<<incoming<<endl;
		if(FrameValid(incoming, recvd)){
			if(debug) cout<<"[PhysicalLayer:ReceiveFrames] Frame was valid"<<endl;
			Frame *result = Frame::Unserialize(incoming);
			pthread_mutex_lock(&mutRF);
			recvFrames.push(result);
			pthread_mutex_unlock(&mutRF);
			if(debug){
				cout<<"[PhysicalLayer:ReceiveFrames] Pushed the received frame:"<<endl;
				result->Print();
			}
		}
		else if(debug) cout<<"[PhysicalLayer:ReceiveFrames] Frame wasn't valid";
	}
	else if(verboseDebug) cout <<"[PhysicalLayer:ReceiveFrames] Nothing available on the wire"<<endl;
	if(verboseDebug) cout <<"[PhysicalLayer:ReceiveFrames] Function end"<<endl;
}

// NB: sFrame may no longer be valid!
// Use XOR folding to generate a 2-byte error-checking field at the end of
// serialized frame sFrame.
unsigned char* PhysicalLayer::FoldSerializedFrame(unsigned char* sFrame, int len){
	unsigned char foldByteA = 0;
	unsigned char foldByteB = 0;
	unsigned char* fCpy = (unsigned char*) calloc(len, sizeof(unsigned char));
	memcpy(fCpy, sFrame, len);
	
	for(int i = 0; i < len; i++){
		if(i % 2 == 0){
			foldByteA ^= *(fCpy + i);
		}
		else{
			foldByteB ^= *(fCpy + i);
		}
	}

	fCpy = (unsigned char *) realloc(fCpy, len + 2); // + 2 for folded bytes
	*(fCpy + len) = foldByteA;
	*(fCpy + len + 1) = foldByteB;
	
	///Frame* fasdfss = Frame::Unserialize((char*)fCpy);
	//Frame* f2 = Frame::Unserialize((char*)sFrame);
	///cout << "##########################";
	///fasdfss->Print();
	///fflush(stdout);
	//cout << "##########################";
	//f2->Print();	

	return reinterpret_cast<unsigned char*>(fCpy);
}

bool PhysicalLayer::FrameValid(char* sFrame, int length){
	int len = length - 2;		// Ignore the two error-checking bytes at the end
	unsigned char foldByteA = 0;
	unsigned char foldByteB = 0;
	for(int i = 0; i < len; i++){
		if(i % 2 == 0){
			foldByteA = foldByteA ^ *(sFrame + i);
		}
		else{
			foldByteB = foldByteB ^ *(sFrame + i);
		}
	}

	return *(sFrame + len) == foldByteA && *(sFrame + len + 1) == foldByteB;
}

PhysicalLayer::~PhysicalLayer() {
	close(sockfd);
}

