/*
 * DataLink.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#include "DataLink.h"

using namespace std;

volatile bool frmTimeout, frmArrive, pktArrive, frmSend, pktSend, frmError;
pthread_mutex_t mutTime, mutFArv, mutPArv, mutFSnd, mutPSnd, mutFErr;

void HandleFrameTimeout(int sig) {
	cout << "Got timeout.\n";
	pthread_mutex_lock(&mutTime);
	frmTimeout = true;
	pthread_mutex_unlock(&mutTime);
	signal(sig, HandleFrameTimeout);
}

void HandleFrameArrival(int sig) {
	cout << "Frame arrive.\n";
	pthread_mutex_lock(&mutFArv);
	frmArrive = true;
	pthread_mutex_unlock(&mutFArv);
	signal(sig, HandleFrameArrival);
}

void HandlePacketArrival(int sig) {
	cout << "Packet arrive.\n";
	pthread_mutex_lock(&mutPArv);
	pktArrive = true;
	pthread_mutex_unlock(&mutPArv);
	signal(sig, HandlePacketArrival);
}

void HandleFrameSend(int sig) {
	cout << "Frame send.\n";
	pthread_mutex_lock(&mutFSnd);
	frmSend = true;
	pthread_mutex_unlock(&mutFSnd);
	signal(sig, HandleFrameSend);
}

void HandlePacketSend(int sig) {
	cout << "Packet send.\n";
	pthread_mutex_lock(&mutPSnd);
	pktSend = true;
	pthread_mutex_unlock(&mutPSnd);
	signal(sig, HandlePacketSend);
}

void HandleFrameError(int sig) {
	cout << "Frame error.\n";
	pthread_mutex_lock(&mutFErr);
	frmError = true;
	pthread_mutex_unlock(&mutFErr);
	signal(sig, HandleFrameError);
}

DataLink::DataLink() {
	window = (Frame**) calloc(1, sizeof(Frame*)); // 1-sliding window
	//window = (Frame**) calloc(4, sizeof(Frame*)); // 4-sliding window
	ready = (Frame**) calloc(MAX_READY, sizeof(Frame*)); // for frames ready to send
	numWindow = 0;
	currWindow = 0;
	numReady = 0;
	currReady = 0;
	nextSend = 0;
	frameExpect = 0;
	
	// should only send 1 packet for testing
	s_packets = (Packet**) calloc(1, sizeof(Packet*));
	// should only ever receive 1 packet
	r_packets = (Packet**) calloc(1, sizeof(Packet*));
	// receive up to 2 frames
	r_frames = (Frame**) calloc(2, sizeof(Frame*));
	
	Packet* p = new Packet();
	p->type = data;
	p->seq = 30;
	p->end = true;
	p->payloadLength = 186;
	p->payload = (unsigned char*) calloc(200, sizeof(unsigned char));
	string s = "Yay it works through layers n stuff. This is quite long to test some stuffYay it works through layers n stuff. This is quite long to test some stuffYay it works through layers n stuff.\n";
	char* str = const_cast<char*>(s.c_str());
	p->payload = (unsigned char*) strcpy((char*)p->payload, str);
	
	s_packets[0] = p;
	frmTimeout = false;
	frmArrive = false;
	pktArrive = false;
	frmSend = false;
	pktSend = true;
	frmError = false;
	signal(SIGALRM, HandleFrameTimeout);
	signal(SIGFRCV, HandleFrameArrival);
	signal(SIGPRCV, HandlePacketArrival);
	signal(SIGFSND, HandleFrameSend);
	signal(SIGPSND, HandlePacketSend);
	signal(SIGFERR, HandleFrameError);
	
	pthread_mutex_init(&mutTime, NULL);
	pthread_mutex_init(&mutFArv, NULL);
	pthread_mutex_init(&mutPArv, NULL);
	pthread_mutex_init(&mutFSnd, NULL);
	pthread_mutex_init(&mutPSnd, NULL);
	pthread_mutex_init(&mutFErr, NULL);
	
	// make a dummy network layer thread that will make and send packets to the DLL
}

DataLink::~DataLink() {
	free(window);
	free(ready);
	free(s_packets);
	free(r_packets);
}

void DataLink::GoBack1() {
	Frame* r = new Frame(), * s = new Frame();
	Packet* buffer = (Packet*) calloc(1, sizeof(Packet));
	Event event = none;
	
	buffer = FromNetworkLayer(buffer);
	MakeFrames(buffer);
	//cout << "Curr ready is " << currReady+0 << " and num ready is " << numReady+0;
	//fflush(stdout);
	ToPhysicalLayer(ready[currReady]);
	StartTimer(0);
	r->type = ack;
	event = arrival;
	while(true) {
		// for testing
		numWindow--;
		// end for testing
		WaitForEvent(&event);
		if(event == arrival) {
			FromPhysicalLayer(r);
			cout << "Frame seq:expected " << r->seq << ":" << frameExpect << ", " << (r->seq == frameExpect);
			if(r->type+0 == ack) {
				cout << "This is an ack somehow";
				r->Print();
				if(currReady < MAX_READY) currReady++; else currReady = 0;
				numReady--;
				if(numReady < MAX_READY - 2) EnableNetworkLayer();
				Frame* f = r_frames[0];
				r_frames[0] = NULL;
				StopTimer(0);
				free(f);
			} else if(r->seq == frameExpect) {
				cout << "Past logical test\n";
				if(r->end == false) {
					cout << "First frame of 2\n";
					*(r_frames[1]) = *(r_frames[0]);
					r_frames[0] = NULL;
				} else {
					cout << "Have frames\n";
					ToNetworkLayer();
				}
				inc(frameExpect);
			}
		}
		event = none;
		if(numReady < MAX_READY - 2 && pktSend) {
			FromNetworkLayer(buffer);
			MakeFrames(buffer);
		}
		//cout << "Curr ready is " << currReady+0 << " and num ready is " << numReady+0;
		ToPhysicalLayer(ready[currReady]);
		StartTimer(0);
		exit(0);
	}
}

void DataLink::GoBackN() {
  /*unsigned short ackExpect = 0;
	Frame r;
	Packet buffer[MAX_SEQ + 1];
	int numBuff = 0, i = 0;
	Event event;

	while (true) {
		WaitForEvent(&event);
		switch (event) {
		case pktReady:
			FromNetworkLayer(&buffer[nextSend]);
			numBuff++;
			SendData(nextSend, frameExpect, buffer);
			inc(nextSend);
			break;
		case arrival:
			FromPhysicalLayer(&r);
			if(r.seq == frameExpect) {
				ToNetworkLayer(r.payload);
				inc(frameExpect);
			}
			if(r.type == ack && r.seq == ackExpect) {
				StopTimer(r.seq);
				numBuff--;
				inc(ackExpect);
			}
			break;
		case error: break;
		case timeout:
			nextSend = ackExpect;
			for(i = 0;i < numBuff;i++) {
				SendData(nextSend, frameExpect, buffer);
				inc(nextSend);
			}
			break;
		}

		if(numBuff < MAX_SEQ)
			EnableNetworkLayer();
		else
			DisableNetworkLayer();
	}*/
}

void DataLink::MakeFrames(Packet* p) {
	if(p == NULL) return;
	unsigned char* currPacket;
	Frame* f1 = new Frame(), * f2 = new Frame();
	
	unsigned short pktLen = p->payloadLength + PACKET_HEAD;
	currPacket = p->Serialize();
	if(pktLen <= MAX_FRAME) {
		f1->payload = (unsigned char*) calloc(pktLen, sizeof(unsigned char));
		memcpy(f1->payload, currPacket, pktLen);
		f1->type = data;
		f1->seq = nextSend;
		f1->payloadLength = pktLen;
		f1->end = true;
		inc(nextSend);
		ready[numReady] = f1;
		numReady++;
	} else {
		f1->payload = (unsigned char*) calloc(MAX_FRAME, sizeof(unsigned char));
		f2->payload = (unsigned char*) calloc(pktLen - MAX_FRAME, sizeof(unsigned char));
		memcpy(f1->payload, currPacket, MAX_FRAME);
		f1->type = data;
		f1->seq = nextSend;
		f1->payloadLength = MAX_FRAME;
		f1->end = false;
		inc(nextSend);
		ready[numReady] = f1;
		numReady++;
		memcpy(f2->payload, currPacket + MAX_FRAME, pktLen - MAX_FRAME + 8);
		f2->type = data;
		f2->seq = nextSend;
		f2->payloadLength = pktLen - MAX_FRAME;
		f2->end = true;
		inc(nextSend);
		ready[numReady] = f2;
		numReady++;
	}
	if(numReady > MAX_READY - 2) DisableNetworkLayer();
}

void DataLink::SendData(unsigned int frame_num, unsigned int frame_expect, Packet buffer[]) {

}

void DataLink::WaitForEvent(Event* e) {
	while(*e == none) {
		if(frmTimeout) {
			*e = timeout;
			frmTimeout = false;
		} else if(frmError) {
			*e = error;
			frmError = false;
		} else if(frmArrive) {
			*e = arrival;
		}
	}
}

Packet* DataLink::FromNetworkLayer(Packet* p) {
	if(!pktSend) return NULL;
	p = s_packets[0];
	pktSend = false;
	return p;
}

void DataLink::ToNetworkLayer() {
	if(pktArrive) return;
	Packet* pkt;
	if(r_frames[1] != NULL) {
		unsigned char* f1 = r_frames[1]->payload;
		unsigned char* f2 = r_frames[0]->payload;
		char* pload = (char*) calloc(r_frames[0]->payloadLength + r_frames[1]->payloadLength + 1, sizeof(char));
		memcpy(pload, f1, r_frames[1]->payloadLength);
		memcpy(pload + r_frames[1]->payloadLength, f2, r_frames[0]->payloadLength);
	} else {
		pkt = Packet::Unserialize((char*) r_frames[0]->payload);
	}
	r_frames[0] = NULL;
	r_frames[1] = NULL;
	r_packets[0] = pkt;
	pktArrive = true;
	raise(SIGPRCV);
}

void DataLink::FromPhysicalLayer(Frame* r) {
	if(!frmArrive) return;
	r = r_frames[0];
	frmArrive = false;
	return;
}

void DataLink::ToPhysicalLayer(Frame* s) {
	if(numWindow == 1) return; // 1-sliding window
	//if(numWindow == 4) return; // 4-sliding window
	window[0] = s;
	numWindow++;
	raise(SIGFSND);
	r_frames[0] = s;
	raise(SIGFRCV);
	/*Packet* p = Packet::Unserialize((char*) s->payload);
	bool p1type = (p->type+0 == data);
	cout << "\nPacket is data: " << p1type;
	cout << "\nPacket sequence number: " << p->seq;
	cout << "\nPacket is end: " << p->end;
	cout << "\nPacket payload length is: " << p->payloadLength;
	cout << "\nPacket message: " << p->payload << "\n";*/
}

void DataLink::StartTimer(unsigned short k) {
	struct timeval* tval = (struct timeval*) calloc(1, sizeof(struct timeval));
	struct timeval* zero = (struct timeval*) calloc(1, sizeof(struct timeval));
	// 1 second timeout
	tval->tv_usec = 500000;
	tval->tv_sec = 0;
	zero->tv_usec = 0;
	zero->tv_sec = 0;
	struct itimerval* timerval = (struct itimerval*) calloc(1, sizeof(struct itimerval));
	timerval->it_value = *tval;
	timerval->it_interval = *zero;
	setitimer(ITIMER_REAL, timerval, NULL);
}

void DataLink::StopTimer(unsigned short k) {
	struct timeval* zero = (struct timeval*) calloc(1, sizeof(struct timeval));
	zero->tv_usec = 0;
	zero->tv_sec = 0;
	struct itimerval* timerval = (struct itimerval*) calloc(1, sizeof(struct itimerval));
	timerval->it_value = *zero;
	timerval->it_interval = *zero;
	setitimer(ITIMER_REAL, timerval, NULL);
}

void DataLink::EnableNetworkLayer(void) {

}

void DataLink::DisableNetworkLayer(void) {

}
