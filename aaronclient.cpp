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


int main(){

	Frame f = Frame();
	f.type = data;
	f.seq = 0;
	f.end = true;
	f.payloadLength = 10;
	char *s = (char *) malloc(10);
	strcpy(s, "Hey me!!!");
	f.payload = (unsigned char*) s;

	sendFrames.push(&f);
	pthread_mutex_init(&mutSF, NULL);
	pthread_mutex_init(&mutRF, NULL);


	PhysicalLayer pl = PhysicalLayer("localhost");

	pthread_t pt;
	pthread_create(&pt, NULL, RunPLThread, &pl);

	struct timespec qs;
	qs.tv_sec = 0;
	qs.tv_nsec = 250000000;

	while(true){
		pthread_mutex_lock(&mutRF);
		if(!recvFrames.empty()){
			cout << "Recieved a frame!! (client)" << endl << (char *) recvFrames.front()->payload << endl;
			return 0;
		}
		pthread_mutex_unlock(&mutRF);
		cout << "Client waiting..." << endl;
		nanosleep(&qs, NULL);
	}


}

void *RunPLThread(void* ptr){
	PhysicalLayer *pl = (PhysicalLayer*) ptr;
	pl->run();
}

