//server.cpp

#include <iostream>
#include <netinet/in.h>		// For sockadd_in
#include <arpa/inet.h>		// For htons, htonl, etc
#include <sys/socket.h>
#include <cstdio>		// For perror()
#include <string.h>		// For memset
#include <unistd.h>		// For close()
#include <pthread.h>

#include "did.h"

using namespace std;

void handlePacket(Message *m);
void testPL();

Packet **sendPackets, **recvPackets;
Frame **sendFrames, **recvFrames;


int AcceptConn();
void *RunPLThread(void* ptr);

int main(){

	sendPackets = (Packet**) calloc(MAX_SEND_PACKET, sizeof(Packet*));
	recvPackets = (Packet**) calloc(MAX_RECV_PACKET, sizeof(Packet*));
	sendFrames = (Frame**) calloc(MAX_SEND_FRAME, sizeof(Frame*));
	recvFrames = (Frame**) calloc(MAX_RECV_FRAME, sizeof(Frame*));

	//int sockfd = AcceptConn();

	PhysicalLayer *pl = new PhysicalLayer(0);
	pthread_t PL_thread, NL_thread, DL_thread;
	cout << "Creating thread.." << endl;
	pthread_create(&PL_thread, NULL, RunPLThread, pl);
	for(int i = 0; i <5; i++){
		cout << "Thread A!" << endl;
		sleep(1);
	}
	return 0;
}

void *RunPLThread(void* ptr){
	PhysicalLayer *pl = (PhysicalLayer*) ptr;
	pl->run();
}

void handlePacket(Message *m){
	char *cmd = m->getCmd();
	char *tmp;
	char *argvNew[8];
}


int AcceptConn(){
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
		int pid = fork();
		if(pid == 0) return sockfd;
	}

}
