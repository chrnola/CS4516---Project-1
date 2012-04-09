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

/*
 * Author: Ray Short
 */
void HandleFrameTimeout(int sig) {
	cout << "Got timeout.\n";
	pthread_mutex_lock(&mutTime);
	frmTimeout = true;
	pthread_mutex_unlock(&mutTime);
	signal(sig, HandleFrameTimeout);
}

/*
 * Author: Ray Short
 */
void HandleFrameArrival(int sig) {
	cout << "Frame arrive.\n";
	pthread_mutex_lock(&mutFArv);
	frmArrive = true;
	pthread_mutex_unlock(&mutFArv);
	signal(sig, HandleFrameArrival);
}

/*
 * Author: Ray Short
 */
void HandlePacketArrival(int sig) {
	cout << "Packet arrive.\n";
	pthread_mutex_lock(&mutPArv);
	pktArrive = true;
	pthread_mutex_unlock(&mutPArv);
	signal(sig, HandlePacketArrival);
}

/*
 * Author: Ray Short
 */
void HandleFrameSend(int sig) {
	cout << "Frame send.\n";
	pthread_mutex_lock(&mutFSnd);
	frmSend = true;
	pthread_mutex_unlock(&mutFSnd);
	signal(sig, HandleFrameSend);
}

/*
 * Author: Ray Short
 */
void HandlePacketSend(int sig) {
	cout << "Packet send.\n";
	pthread_mutex_lock(&mutPSnd);
	pktSend = true;
	pthread_mutex_unlock(&mutPSnd);
	signal(sig, HandlePacketSend);
}

/*
 * Author: Ray Short
 */
void HandleFrameError(int sig) {
	cout << "Frame error.\n";
	pthread_mutex_lock(&mutFErr);
	frmError = true;
	pthread_mutex_unlock(&mutFErr);
	signal(sig, HandleFrameError);
}

/*
 * Author: Ray Short
 */
DataLink::DataLink() {
	//window = (Frame**) calloc(1, sizeof(Frame*)); // 1-sliding window
	//window = (Frame**) calloc(4, sizeof(Frame*)); // 4-sliding window
	//ready = (Frame**) calloc(MAX_READY, sizeof(Frame*)); // for frames ready to send
	//numWindow = 0;
	//currWindow = 0;
	//numReady = 0;
	//currReady = 0;
	nextSend = 0;
	frameExpect = 0;
		
	/*Packet* p = new Packet();
	p->type = data;
	p->seq = 30;
	p->end = true;
	p->payloadLength = 186;
	p->payload = (unsigned char*) calloc(200, sizeof(unsigned char));
	string s = "Yay it works through layers n stuff. This is quite long to test some stuffYay it works through layers n stuff. This is quite long to test some stuffYay it works through layers n stuff.\n";
	char* str = const_cast<char*>(s.c_str());
	p->payload = (unsigned char*) strcpy((char*)p->payload, str);
	
	pthread_mutex_lock(&mutSP);
	sendPackets.push(p);
	//sendPackets.front()->Print();
	pthread_mutex_unlock(&mutSP);*/
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

/*
 * Author: Ray Short
 */
DataLink::~DataLink() {
	//free(window);
	//free(ready);
}

/*
 * Author: Ray Short
 */
void DataLink::GoBack1() {
	Frame* r = new Frame(), * s = new Frame();
	Packet* buffer = (Packet*) calloc(1, sizeof(Packet));
	Event* event = (Event*) calloc(1, sizeof(Event));
	*event = none;
	
	buffer = FromNetworkLayer(buffer);
	MakeFrames(buffer);
	//cout << "Curr ready is " << currReady+0 << " and num ready is " << numReady+0;
	//fflush(stdout);
	ToPhysicalLayer(ready.front());
	StartTimer(0);
	r->type = ack;
	*event = arrival;
	while(true) {
		// for testing
		numWindow = 0;
		// end for testing
		event = WaitForEvent(event);
		if(*event == arrival) {
			r = FromPhysicalLayer(r);
			//r->Print();
			//cout << (r->seq == frameExpect) << " seq:expect " << r->seq << ":" << frameExpect;
			//fflush(stdout);
			if(r->type == ack) {
				StopTimer(0);
				ready.pop();
				//if(numReady < MAX_READY - 2) EnableNetworkLayer();
				// remove ack from queue of received frames
				RemoveAck();
				pthread_mutex_lock(&mutRF);
				Frame* f = recvFrames.front();
				recvFrames.pop();
				pthread_mutex_unlock(&mutRF);			
				free(f);
			} else if(r->seq == frameExpect) {
				if(r->end == true) {
					ToNetworkLayer();
				} else {
					//recvFrames.push(r);
				}
				SendAck();
				inc(frameExpect);
			}
		}
		*event = none;
		if(numReady < MAX_READY - 2 && pktSend) {
			FromNetworkLayer(buffer);
			MakeFrames(buffer);
		}
		//cout << "Curr ready is " << currReady+0 << " and num ready is " << numReady+0;
		numReady--;
		if(numReady == 0) return;
		ToPhysicalLayer(ready.front());
		StartTimer(0);
	}
}

/*
 * Author: Ray Short
 */
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

/*
 * Author: Ray Short
 */
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
		ready.push(f1);
	} else {
		f1->payload = (unsigned char*) calloc(MAX_FRAME, sizeof(unsigned char));
		f2->payload = (unsigned char*) calloc(pktLen - MAX_FRAME, sizeof(unsigned char));
		memcpy(f1->payload, currPacket, MAX_FRAME);
		f1->type = data;
		f1->seq = nextSend;
		f1->payloadLength = MAX_FRAME;
		f1->end = false;
		inc(nextSend);
		ready.push(f1);
		memcpy(f2->payload, currPacket + MAX_FRAME, pktLen - MAX_FRAME + 8);
		f2->type = data;
		f2->seq = nextSend;
		f2->payloadLength = pktLen - MAX_FRAME;
		f2->end = true;
		inc(nextSend);
		ready.push(f2);
	}
	//if(numReady > MAX_READY - 2) DisableNetworkLayer();
}

/*
 * Author: Ray Short
 */
void DataLink::SendAck() {
	Frame* f = new Frame();
	f->type = ack;
	f->seq = frameExpect;
	f->end = true;
	f->payloadLength = 0;
	ToPhysicalLayer(f);
}

/*
 * Author: Ray Short
 */
void DataLink::SendData(unsigned int frame_num, unsigned int frame_expect, Packet buffer[]) {

}

/*
 * Author: Ray Short
 */
Event* DataLink::WaitForEvent(Event* e) {
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
	return e;
}

/*
 * Author: Ray Short
 */
Packet* DataLink::FromNetworkLayer(Packet* p) {
	if(!pktSend) return NULL;
	pthread_mutex_lock(&mutSP);
	p = sendPackets.front();
	sendPackets.pop();
	pthread_mutex_unlock(&mutSP);
	pktSend = false;
	return p;
}

/*
 * Author: Ray Short
 */
void DataLink::ToNetworkLayer() {
	if(pktArrive) return;
	Packet* pkt = (Packet*) calloc(1, sizeof(Packet));
	pthread_mutex_lock(&mutRF);
	Frame* fr1 = (Frame*) calloc(1, sizeof(Frame));
	fr1 = recvFrames.front();
	recvFrames.pop();
	Frame* fr2 = (Frame*) calloc(1, sizeof(Frame));
	fr2 = recvFrames.front();
	recvFrames.pop();
	pthread_mutex_unlock(&mutRF);
	if(fr2 != NULL) {
		unsigned char* f1 = fr1->payload;
		unsigned char* f2 = fr2->payload;
		char* pload = (char*) calloc(fr1->payloadLength + fr2->payloadLength + 8, sizeof(char));
		memcpy(pload, f1, fr1->payloadLength);
		memcpy(pload + fr1->payloadLength, f2, fr2->payloadLength + 8);
		pkt = Packet:: Unserialize((char*) pload);
	} else {
		pkt = Packet::Unserialize((char*) fr1->payload);
	}
	pthread_mutex_lock(&mutRP);
	recvPackets.push(pkt);
	pthread_mutex_unlock(&mutRP);
	//pkt->Print();
	pktArrive = true;
	raise(SIGPRCV);
}

/*
 * Author: Ray Short
 */
Frame* DataLink::FromPhysicalLayer(Frame* r) {
	if(!frmArrive) return NULL;
	pthread_mutex_lock(&mutRF);
	r = recvFrames.back();
	pthread_mutex_unlock(&mutRF);
	frmArrive = false;
	return r;
}

/*
 * Author: Ray Short
 */
void DataLink::ToPhysicalLayer(Frame* s) {
	if(numWindow == 1) return; // 1-sliding window
	//if(numWindow == 4) return; // 4-sliding window
	window.push(s);
	numWindow++;
	pthread_mutex_lock(&mutSF);
	sendFrames.push(s);
	pthread_mutex_unlock(&mutSF);
	raise(SIGFSND);
	pthread_mutex_lock(&mutRF);
	recvFrames.push(s);
	pthread_mutex_unlock(&mutRF);
	raise(SIGFRCV);
	/*Packet* p = Packet::Unserialize((char*) s->payload);
	bool p1type = (p->type+0 == data);
	cout << "\nPacket is data: " << p1type;
	cout << "\nPacket sequence number: " << p->seq;
	cout << "\nPacket is end: " << p->end;
	cout << "\nPacket payload length is: " << p->payloadLength;
	cout << "\nPacket message: " << p->payload << "\n";*/
}

/*
 * Author: Ray Short
 */
void DataLink::StartTimer(unsigned short k) {
	struct timeval* tval = (struct timeval*) calloc(1, sizeof(struct timeval));
	struct timeval* zero = (struct timeval*) calloc(1, sizeof(struct timeval));
	// 0.5 second timeout
	tval->tv_usec = 500000;
	tval->tv_sec = 0;
	zero->tv_usec = 0;
	zero->tv_sec = 0;
	struct itimerval* timerval = (struct itimerval*) calloc(1, sizeof(struct itimerval));
	timerval->it_value = *tval;
	timerval->it_interval = *zero;
	setitimer(ITIMER_REAL, timerval, NULL);
}

/*
 * Author: Ray Short
 */
void DataLink::StopTimer(unsigned short k) {
	struct timeval* zero = (struct timeval*) calloc(1, sizeof(struct timeval));
	zero->tv_usec = 0;
	zero->tv_sec = 0;
	struct itimerval* timerval = (struct itimerval*) calloc(1, sizeof(struct itimerval));
	timerval->it_value = *zero;
	timerval->it_interval = *zero;
	setitimer(ITIMER_REAL, timerval, NULL);
}

// remove ack from queue of received frames
void DataLink::RemoveAck() {
	int i, size = recvFrames.size();
	for(i = 0;i < size;i++) {
		pthread_mutex_lock(&mutRF);
		Frame* f = recvFrames.front();
		pthread_mutex_unlock(&mutRF);
		if(f->type == ack) {
			pthread_mutex_lock(&mutRF);
			recvFrames.pop();
			pthread_mutex_unlock(&mutRF);
			free(f);
			return;
		} else {
			pthread_mutex_lock(&mutRF);
			recvFrames.pop();
			recvFrames.push(f);
			pthread_mutex_unlock(&mutRF);
		}
	}
}
