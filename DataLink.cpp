/*
 * DataLink.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#include "DataLink.h"

using namespace std;

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
	p->payload = (unsigned char*) calloc(90, sizeof(unsigned char));
	string s = "Yay it works through layers n stuff. This is quite long to test some stuff\n";
	char* str = const_cast<char*>(s.c_str());
	p->payload = (unsigned char*) strcpy((char*)p->payload, str);
	
	s_packets[0] = *p;
	networkReady = true;
	
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
	Event event;
	
	buffer = FromNetworkLayer(buffer);
	MakeFrames(buffer);
	cout << "Curr ready is " << currReady+0 << " and num ready is " << numReady+0;
	ToPhysicalLayer(&(ready[currReady]));
	if(currReady < MAX_READY) currReady++; else currReady = 0;
	StartTimer(0);
	
	while(true) {
		WaitForEvent(&event);
		if(event == arrival) {
			FromPhysicalLayer(r);
			if(r->type == ack) {
				free(&ready[currReady]);
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
		ToPhysicalLayer(&ready[currReady]);
		StartTimer(0);
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
	if(sizeof(currPacket) <= MAX_FRAME) {
		f1->payload = (unsigned char*) calloc(sizeof(currPacket) + 1, sizeof(unsigned char));
		strcpy((char*) f1->payload, currPacket);
		f1->seq = nextSend;
		inc(nextSend);
		ready[numReady] = *f1;
		numReady++;
	} else {
		strncpy((char*) f1->payload, currPacket, MAX_FRAME);
		f1->seq = nextSend;
		inc(nextSend);
		ready[numReady] = *f1;
		numReady++;
		strcpy((char*) f2->payload, currPacket+MAX_FRAME);
		f2->seq = nextSend;
		inc(nextSend);
		ready[numReady] = *f2;
		numReady++;
	}
}

void DataLink::SendData(unsigned int frame_num, unsigned int frame_expect, Packet buffer[]) {

}

void DataLink::WaitForEvent(Event* e) {
	exit(0);
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
	cout << "\nFrame payload is: " << s->payload;
	Packet* p = DataLink::UnserializeP((char*) s->payload);
	bool p1type = (p->type+0 == data);
	cout << "\nPacket is data: " << p1type;
	cout << "\nPacket sequence number: " << p->seq;
	cout << "\nPacket is end: " << p->end;
	cout << "\nPacket message: " << p->payload << "\n";
}

void DataLink::StartTimer(unsigned short k) {

}

void DataLink::StopTimer(unsigned short k) {

}

void DataLink::EnableNetworkLayer(void) {

}

void DataLink::DisableNetworkLayer(void) {

}

//static unsigned char* DataLink::Serialize(Msg* m) {
//}

unsigned char* DataLink::Serialize(Packet* p) {
	unsigned char* data = (unsigned char*) calloc(sizeof(p->payload) + PACKET_HEAD + 1, sizeof(unsigned char));
	p->type == ack ? strcat((char*) data, "1") : strcat((char*) data, "0");
	strcat((char*) data, (char*) DataLink::itoa(p->seq));
	p->end ? strcat((char*) data, "1") : strcat((char*) data, "0");
	strcat((char*) data, (char*) p->payload);
	return data;
}

//static Msg* DataLink::UnserializeM(unsigned char d[]) {

//}

Packet* DataLink::UnserializeP(char* d) {
	cout << "-0In unserializep:  " << d << " length is " << strlen(d);
	cout << "\nElement 30 of d is: " << d[30] << "dhsd";
	char* d2 = (char*) malloc(strlen(d) * sizeof(char));
	cout << "\nElement 30 of d is: " << d[30] << "dhsd";
	cout << "\nLength of d2 is: " << strlen(d2) << " and it is: " << d2;
	strncpy(d2, d, 44);
	cout << "\nLength of d2 is now: " << strlen(d2);
	cout << "\nElement 30 of d is: " << d[30] << "dhsd";
	cout << "\n0In unserializep:  " << d2 << " length is " << strlen(d);
	int i;
	
	char* temp2 =  (char*) calloc(strlen(d) - 7, sizeof(char));
	cout << "\n4In unserializep:  " << d << " length is " << strlen(d);
	for(i = 7;i < strlen(d);i++) {
		temp2[i-7] = d[i];
	}
	char* temp =  (char*) malloc(6 * sizeof(char));
	cout << "\n-2In unserializep:  " << d << " length is " << strlen(d);

	cout << "\n2In unserializep:  " << d << " length is " << strlen(d);
	for(i = 1;i < 6;i++) {
		temp[i-1] = d[i];
	}
	
	Packet* p = new Packet();
	d[0] == 0 ? p->type = ack : p->type = data;
	p->seq = (unsigned short) atoi(temp);
	d[6] == 0 ? p->end = false : p-> end = true;
	p->payload = (unsigned char*) temp2;
	return p;
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
