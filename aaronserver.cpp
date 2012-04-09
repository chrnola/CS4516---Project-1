/*
 *  aaronserv.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: Aaron Sikes
 */


#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <cstdio>
#include <string.h>

#include "PhysicalLayer.h"
#include "did.h"

using namespace std;

queue<Packet*> sendPackets, recvPackets;
queue<Frame*> sendFrames, recvFrames;
pthread_mutex_t mutSP, mutRP, mutSF, mutRF;

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
	pthread_mutex_init(&mutSF);
	pthread_mutex_init(&mutRF);

	



}
