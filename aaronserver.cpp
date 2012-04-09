/*
 *  aaronserv.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: Aaron Sikes
 */


#include <iostream>
#include <pthread.h>
#include <cstdio>
#include <string.h>

#include "PhysicalLayer.h"
#include "did.h"

#include <netinet/in.h>		// For sockadd_in
#include <arpa/inet.h>		// For htons, htonl, etc
#include <sys/socket.h>
#include <unistd.h>		// For close()

#include <time.h> 		// For nanosleep()

using namespace std;

queue<Packet*> sendPackets, recvPackets;
queue<Frame*> sendFrames, recvFrames;
pthread_mutex_t mutSP, mutRP, mutSF, mutRF;


void *RunPLThread(void* ptr);

int AcceptConn(bool singleProc);

int main(){

	Frame f = Frame();
	f.type = data;
	f.seq = 0;
	f.end = true;
	f.payloadLength = 10;
	char *s = (char *) malloc(10);
	strcpy(s, "Hey you!!");
	f.payload = (unsigned char*) s;

	sendFrames.push(&f);
	pthread_mutex_init(&mutSF, NULL);
	pthread_mutex_init(&mutRF, NULL);


	PhysicalLayer pl = PhysicalLayer(AcceptConn(true));

	pthread_t pt;
	pthread_create(&pt, NULL, RunPLThread, &pl);

	struct timespec qs;
	qs.tv_sec = 0;
	qs.tv_nsec = 250000000;

	while(true){
		pthread_mutex_lock(&mutRF);
		if(!recvFrames.empty()){
			cout << "Recieved a frame!! (server)" << endl << (char *) recvFrames.front()->payload << endl;
			return 0;
		}
		pthread_mutex_unlock(&mutRF);
		cout << "Server waiting..." << endl;
		nanosleep(&qs, NULL);
	}


}

void *RunPLThread(void* ptr){
	PhysicalLayer *pl = (PhysicalLayer*) ptr;
	pl->run();
}

int AcceptConn(bool singleProc){
	int listen_socket;
	if ( (listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("Server-error creating listening socket");
		exit(EXIT_FAILURE);
	}

	struct    sockaddr_in servaddr;  /*  socket address structure  */
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(WELL_KNOWN_PORT);

	/*  Bind our socket addresss to the 
	listening socket, and call listen()  */
	int a = 1;
	setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &a, 4);

	if ( bind(listen_socket, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
		perror("Server-error calling bind()");
		exit(EXIT_FAILURE);
	}

	if ( listen(listen_socket, 30) < 0 ) {// All the connections!!
		perror("Server-error calling listen()");
		exit(EXIT_FAILURE);
	}

	cout << "Setup complete!" << endl;

	int sockfd;

	while(true){
		if ((sockfd = accept(listen_socket, NULL, NULL)) < 0 ) {
			perror("Server-error calling accept()");
			exit(EXIT_FAILURE);
		}
		if(singleProc){
			return sockfd;
			close(listen_socket);
		}
		int pid = fork();
		if(pid == 0) return sockfd;
	}

}
