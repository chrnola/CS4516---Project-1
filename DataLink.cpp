/*
 * DataLink.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#include "DataLink.h"

using namespace std;

volatile bool pktTimeout, pktArrive, pktError, networkReady;

void HandleNetworkReady(int sig) {
	networkReady = true;
	signal(sig, HandleNetworkReady);
}

void HandleError(int sig) {
	pktError = true;
	signal(sig, HandleError);
}

void HandleArrival(int sig) {
	pktArrive = true;
	signal(sig, HandleArrival);
}

void HandleTimeout(int sig) {
	cout << "Got timeout.\n";
	pktTimeout = true;
	signal(sig, HandleTimeout);
}

DataLink::DataLink() {
	window = (Frame*) calloc(1, sizeof(Frame)); // 1-sliding window
	//window = (Frame*) calloc(4, sizeof(Frame)); // 4-sliding window
	ready = (Frame*) calloc(MAX_READY, sizeof(Frame)); // for frames ready to send
	numReady = 0;
	currReady = 0;
	nextSend = 0;
	frameExpect = 0;
	
	// should only send 1 packet for testing
	s_packets = (Packet*) calloc(1, sizeof(Packet)); 
	// should only ever receive 1 packet
	r_packets = (Packet*) calloc(1, sizeof(Packet)); 
	r_frames = (Frame*) calloc(1, sizeof(Frame)); // receive only one frame
	
	Packet* p = new Packet();
	p->type = data;
	p->seq = 30;
	p->end = true;
	p->payload = (unsigned char*) calloc(200, sizeof(unsigned char));
	string s = "Yay it works through layers n stuff. This is quite long to test some stuffYay it works through layers n stuff. This is quite long to test some stuffYay it works through layers n stuff.\n";
	char* str = const_cast<char*>(s.c_str());
	p->payload = (unsigned char*) strcpy((char*)p->payload, str);
	
	s_packets[0] = *p;
	networkReady = true;
	pktArrive = false;
	pktError = false;
	pktTimeout = false;
	signal(SIGALRM, HandleTimeout);
	
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
	cout << "Curr ready is " << currReady+0 << " and num ready is " << numReady+0;
	ToPhysicalLayer(&(ready[currReady]));
	StartTimer(0);
	r->type = ack;
	while(true) {
		WaitForEvent(&event);
		if(event == arrival) {
			FromPhysicalLayer(r);
			if(r->type == ack) {
				if(currReady < MAX_READY) currReady++; else currReady = 0;
				numReady--;
			} else if(r->seq == frameExpect) {
				ToNetworkLayer(r->payload);
				inc(frameExpect);
			}
		}
		if(numReady < MAX_READY - 2 && networkReady) {
			FromNetworkLayer(buffer);
			MakeFrames(buffer);
		}
		//cout << "Curr ready is " << currReady+0 << " and num ready is " << numReady+0;
		ToPhysicalLayer(&ready[currReady]);
		StartTimer(0);
		exit(0);
	}
}

void DataLink::GoBackN() {
  unsigned short ackExpect = 0;
	Frame r;
	Packet buffer[MAX_SEQ + 1];
	int numBuff = 0, i = 0;
	Event event;

	while (true) {
		WaitForEvent(&event);
		switch (event) {
		case network_ready:
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
	}
}

void DataLink::MakeFrames(Packet* p) {
	if(p == NULL) return;
	char* currPacket;
	Frame* f1 = new Frame(), * f2 = new Frame();
	
	currPacket = (char*) DataLink::Serialize(p);
	if(strlen(currPacket) <= MAX_FRAME) {
		f1->payload = (unsigned char*) calloc(strlen(currPacket) + 1, sizeof(unsigned char));
		strcpy((char*) f1->payload, currPacket);
		f1->seq = nextSend;
		f1->end = true;
		inc(nextSend);
		ready[numReady] = *f1;
		numReady++;
	} else {
		f1->payload = (unsigned char*) calloc(MAX_FRAME, sizeof(unsigned char));
		f2->payload = (unsigned char*) calloc(strlen(currPacket) - MAX_FRAME, sizeof(unsigned char*));
		strncpy((char*) f1->payload, currPacket, MAX_FRAME);
		f1->seq = nextSend;
		f1->end = false;
		inc(nextSend);
		ready[numReady] = *f1;
		numReady++;
		strcpy((char*) f2->payload, currPacket + MAX_FRAME - 8);
		f2->seq = nextSend;
		f2->end = true;
		inc(nextSend);
		ready[numReady] = *f2;
		numReady++;
	}
}

void DataLink::SendData(unsigned int frame_num, unsigned int frame_expect, Packet buffer[]) {

}

void DataLink::WaitForEvent(Event* e) {
	while(*e == none) {
		if(pktTimeout) {
			*e = timeout;
		} else if(pktError) {
			*e = error;
		} else if(pktArrive) {
			*e = arrival;
		}
	}
}

Packet* DataLink::FromNetworkLayer(Packet* p) {
	if(!networkReady) return NULL;
	p = &s_packets[0];
	networkReady = false;
	return p;
}

void DataLink::ToNetworkLayer(unsigned char p[]) {

}

void DataLink::FromPhysicalLayer(Frame* r) {

}

void DataLink::ToPhysicalLayer(Frame* s) {
	Packet* p = DataLink::UnserializeP((char*) s->payload);
	bool p1type = (p->type+0 == data);
	/*cout << "\nPacket is data: " << p1type;
	cout << "\nPacket sequence number: " << p->seq;
	cout << "\nPacket is end: " << p->end;
	cout << "\nPacket message: " << p->payload << "\n";*/
}

void DataLink::StartTimer(unsigned short k) {
	struct timeval* tval = (struct timeval*) calloc(1, sizeof(struct timeval));
	struct timeval* zero = (struct timeval*) calloc(1, sizeof(struct timeval));
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


unsigned char* DataLink::itoa(unsigned short n) {
	char val = 0;
	unsigned char* num = (unsigned char*) calloc(5, sizeof(unsigned char));
	if(n > 9999) {
		val = n / 10000;
		num[0] = val + 0x30;
		n -= val * 10000;
	} else num[0] = 0x30;
	
	if(n > 999) {
		val = n / 1000;
		num[1] = val + 0x30;
		n -= val * 1000;
	} else num[1] = 0x30;
	
	if(n > 99) {
		val = n / 100;
		num[2] = val + 0x30;
		n -= val * 100;
	} else num[2] = 0x30;
	
	if(n > 9) {
		val = n / 10;
		num[3] = val + 0x30;
		n -= val * 10;
	} else num[3] = 0x30;
	
	if(n > 0) {
		num[4] = n + 0x30;
	} else num[4] = 0x30;
	
	return num;
}
