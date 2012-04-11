/*
 * DataLink.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#include "DataLink.h"

using namespace std;

volatile bool frmTimeout, frmArrive, pktSend;
pthread_mutex_t mutTime;

/*
 * Author: Ray Short
 */
void HandleFrameTimeout(int sig) {
	//cout << "Got timeout.\n";
	pthread_mutex_lock(&mutTime);
	frmTimeout = true;
	pthread_mutex_unlock(&mutTime);
	signal(sig, HandleFrameTimeout);
}

/*
 * Author: Ray Short
 */
// constructor for initializing some variables
DataLink::DataLink() {
	// initialize counters
	numWindow = 0;
	numReady = 0;
	nextSend = 0;
	frameExpect = 0;
	
	// initialize handling variables
	frmTimeout = false;
	frmArrive = false;
	pktSend = false;
	signal(SIGALRM, HandleFrameTimeout);
	
	// initialize mutex variable for timeout
	pthread_mutex_init(&mutTime, NULL);
}

/*
 * Author: Ray Short
 */
// deconstructor, nothing really
DataLink::~DataLink() {
}

/*
 * Author: Ray Short
 */
// implementation of Go-Back-1
// uses sliding window of 1, retransmissions and acks
void DataLink::GoBack1() {
	Frame* r = (Frame*) calloc(1, sizeof(Frame));
	Packet* buffer = (Packet*) calloc(1, sizeof(Packet));
	Event* event = (Event*) calloc(1, sizeof(Event));
	*event = none;
	
	buffer = FromNetworkLayer(buffer);
	MakeFrames(buffer);
	cout << "ready has " << ready.size() << endl;
	cout << "ready is empty? " << ready.empty() << endl;
	if(!ready.empty()) {
		ToPhysicalLayer(ready.front());
		StartTimer(0);
	}

	/* testing code
	r->type = ack;
	*event = arrival;
	// end testing code*/
	while(true) {
		// for testing
		numWindow = 0;
		// end for testing

		cout << "Waiting for an event; arrival:"<<arrival<<" error:"<<error<<" timeout:"<<timeout<<" pktready:"<<pktReady<<" none:"<<none<<endl;
		event = WaitForEvent(event);
		cout << "pktsend is now " << pktSend << endl;
		if(*event == arrival) {
			r = FromPhysicalLayer(r);
			cout << "received is";
			r->Print();
			//cout << (r->seq == frameExpect) << " seq:expect " << r->seq << ":" << frameExpect;
			fflush(stdout);
			if(r == NULL) continue;
			if(r->type == ACK) {
				cout << "Got an ack";
				StopTimer(0);
				if(!ready.empty()) 
					ready.pop();
				if(!window.empty())
					window.pop();
				if(!sendFrames.empty())
					sendFrames.pop();
				// remove ack from queue of received frames
				pthread_mutex_lock(&mutRF);
				if(!recvFrames.empty()) recvFrames.pop();
				cout << "3424recvFrames has size " << recvFrames.size();
				pthread_mutex_unlock(&mutRF);
			} else if(r->seq == frameExpect) {
				cout << "this is the expected frame";
				if(r->end == true) {
					ToNetworkLayer();
				}
				//cout << "Sending an ack";
				SendAck();
				//cout << "Ack sent";
				inc(frameExpect);
			} else cout << "this is not the expected frame nor an ack";
		}
		// reset for waiting
		*event = none;
		if(ready.size() < MAX_READY - 2 && pktSend) {
			buffer = FromNetworkLayer(buffer);
			MakeFrames(buffer);
			pktSend = false;
		}
		//cout << "Curr ready is " << currReady+0 << " and num ready is " << numReady+0;
		// some more testing code
		numReady--;
		if(numReady == 0) return;
		// end of testing code
		if(!ready.empty()) {
			ToPhysicalLayer(ready.front());
			StartTimer(0);
		}
	}
}

/*
 * Author: Ray Short
 */
// implement once go back 1 is working
// dummy code copied from tanenbaum
void DataLink::GoBackN() {
  unsigned short ackExpect = 0;
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
			ToPhysicalLayer(sendFrames.front());
			inc(nextSend);
			break;
		case arrival:
			FromPhysicalLayer(&r);
			if(r.seq == frameExpect) {
				ToNetworkLayer();
				inc(frameExpect);
			}
			if(r.type == ACK && r.seq == ackExpect) {
				StopTimer(r.seq);
				numBuff--;
				inc(ackExpect);
			}
			break;
		case error: break;
		case timeout:
			nextSend = ackExpect;
			for(i = 0;i < numBuff;i++) {
				ToPhysicalLayer(sendFrames.front());
				inc(nextSend);
			}
			break;
		case none: break;
		}
	}
}

/*
 * Author: Ray Short
 */
void DataLink::MakeFrames(Packet* p) {
	if(p == NULL) return;
	unsigned char* currPacket;
	Frame* f1 = new Frame(), * f2 = new Frame();
	
	cout << "Got packet" << endl;
	p->Print();
	cout << "Making into frames" << endl;
	
	// get length of packet and serialize into char array
	unsigned short pktLen = p->payloadLength + PACKET_HEAD;
	currPacket = p->Serialize();
	if(pktLen <= MAX_FRAME) { // 1 frame's worth
		cout << "one frame" << endl;
		f1->payload = (unsigned char*) calloc(pktLen, sizeof(unsigned char));
		memcpy(f1->payload, currPacket, pktLen);
		f1->type = DATA;
		f1->seq = nextSend;
		f1->payloadLength = pktLen;
		f1->end = true;
		inc(nextSend);
		ready.push(f1);
		f1->Print();
	} else { // 2 frame's worth (never more than 2)
		cout << "two frames" << endl;
		// assign first frame
		f1->payload = (unsigned char*) calloc(MAX_FRAME, sizeof(unsigned char));
		f2->payload = (unsigned char*) calloc(pktLen - MAX_FRAME, sizeof(unsigned char));
		memcpy(f1->payload, currPacket, MAX_FRAME);
		f1->type = DATA;
		f1->seq = nextSend;
		f1->payloadLength = MAX_FRAME;
		f1->end = false;
		inc(nextSend);
		ready.push(f1);
		f1->Print();
		//assign second frame
		memcpy(f2->payload, currPacket + MAX_FRAME, pktLen - MAX_FRAME);
		f2->type = DATA;
		f2->seq = nextSend;
		f2->payloadLength = pktLen - MAX_FRAME;
		f2->end = true;
		inc(nextSend);
		ready.push(f2);
		f2->Print();
	}
}

/*
 * Author: Ray Short
 */
// sends an ack for the frame just received
void DataLink::SendAck() {
	Frame* f = new Frame();
	f->type = ACK;
	f->seq = frameExpect;
	f->end = true;
	f->payloadLength = 0;
	cout << "Sending ACK:\n";
	f->Print();
	cout << "Serialized is " << f->Serialize();
	ToPhysicalLayer(f);
}

/*
 * Author: Ray Short
 */
// poll for signals and change event as needed
Event* DataLink::WaitForEvent(Event* e) {
	cout << "inside waitForEvent, ";
	while(*e == none) {
		if(frmTimeout) {
			frmTimeout = false;
			*e = timeout;
		}
		if(pthread_mutex_trylock(&mutRF) == 0) {
			//cout << "checking recvFrames\n";
			if(!recvFrames.empty()) {
				//cout << "recvframes has something";
				frmArrive = true;
				*e = arrival;
			}
			pthread_mutex_unlock(&mutRF);
		}
		if(!pktSend && pthread_mutex_trylock(&mutSP) == 0) {
			//cout << "checking sendpackets\n";
			if(!sendPackets.empty()) {
				cout << "sendpackets has something";
				pktSend = true;
				*e = pktReady;
			}
			pthread_mutex_unlock(&mutSP);
		}
	}
	cout << "got event " << *e << endl;
	return e;
}

/*
 * Author: Ray Short
 */
// move a packet from the shared buffer into a local Packet
Packet* DataLink::FromNetworkLayer(Packet* p) {
	if(!pktSend) return NULL;
	pthread_mutex_lock(&mutSP);
	if(!sendPackets.empty()) {
		p = sendPackets.front();
		sendPackets.pop();
	} else {
		p = NULL;
	}
	pthread_mutex_unlock(&mutSP);
	pktSend = false;
	return p;
}

/*
 * Author: Ray Short
 */
// take the recently received frames and construct
// a packet from them
// checks for 2-frame and 1-frame length packets
void DataLink::ToNetworkLayer() {
	if(pthread_mutex_trylock(&mutRP) == 0){
		if(!recvPackets.empty()) {
			pthread_mutex_unlock(&mutRP);
			return;
		}
	}
	Packet* pkt = (Packet*) calloc(1, sizeof(Packet));
	Frame* fr1 = (Frame*) calloc(1, sizeof(Frame));
	Frame* fr2 = (Frame*) calloc(1, sizeof(Frame));
	pthread_mutex_lock(&mutRF);
	if(!reconstructFrames.empty()) {
		fr1 = reconstructFrames.front();
		reconstructFrames.pop();
	} else fr1 = NULL;
	if(!reconstructFrames.empty()) {
		fr2 = reconstructFrames.front();
		reconstructFrames.pop();
	} else fr2 = NULL;
	pthread_mutex_unlock(&mutRF);
	if(fr2 != NULL) { // long packet
		unsigned char* f1 = fr1->payload;
		unsigned char* f2 = fr2->payload;
		//cout << "sending to network, frame1 followed by frame2";
		//fr1->Print();
		//fr2->Print();
		char* pload = (char*) calloc(fr1->payloadLength + fr2->payloadLength + 8, sizeof(char));
		memcpy(pload, f1, fr1->payloadLength);
		memcpy(pload + fr1->payloadLength, f2, fr2->payloadLength + 8);
		pkt = Packet:: Unserialize((char*) pload);
	} else { // short packet
		pkt = Packet::Unserialize((char*) fr1->payload);
	}
	// move packet up to network layer
	pthread_mutex_lock(&mutRP);
	recvPackets.push(pkt);
	pthread_mutex_unlock(&mutRP);
	//pkt->Print();
}

/*
 * Author: Ray Short
 */
// return the most recently received frame
// takes back of queue because of 2-frame packets
// and using this queue as temporary storage
// for the first before the second arrives
Frame* DataLink::FromPhysicalLayer(Frame* r) {
	if(!frmArrive) return NULL;
	pthread_mutex_lock(&mutRF);
	cout << "receivedFrames has size " << recvFrames.size() << endl;
	if(!recvFrames.empty()) {
		r = recvFrames.front();
		recvFrames.pop();
	}
	if(recvFrames.empty()) frmArrive = false;
	pthread_mutex_unlock(&mutRF);
	if(r != NULL && r->type == DATA) {
		reconstructFrames.push(r);
		reconstructFrames.front()->Print();
	}
	cout << "receivedFrames has size after reconstruct " << recvFrames.size();
	fflush(stdout);
	
	return r;
}

/*
 * Author: Ray Short
 */
// places the passed frame into the window, as well
// as the shared buffer with the physical layer,
// signalling the physical layer that a frame is ready
void DataLink::ToPhysicalLayer(Frame* s) {
	cout << "attempting to send to physical layer";
	if(window.size() == 1) return; // 1-sliding window
	//if(numWindow == 4) return; // 4-sliding window
	cout << "Sending frame";
	s->Print();
	cout << "size of window is " << window.size();
	fflush(stdout);
	window.push(s);
	cout << "size of window is " << window.size();
	fflush(stdout);
	numWindow++;
	cout << "locking mutex to send frame";
	fflush(stdout);
	int lock;
	if((lock = (int) pthread_mutex_trylock(&mutSF)) == 0) {
		sendFrames.push(s);
		cout << "pushed frame";
		pthread_mutex_unlock(&mutSF);
	}
	//cout << "lock was " << lock;
	//fflush(stdout);
	// testing below
	//cout << "adding to recvFrames";
	/*pthread_mutex_lock(&mutRF);
	recvFrames.push(s);
	pthread_mutex_unlock(&mutRF);*/
	//Packet* p = Packet::Unserialize((char*) s->payload);
	//p->Print();
}

/*
 * Author: Ray Short
 */
// start timeout timer for ack receive after frame send
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
// stops timeout timer
void DataLink::StopTimer(unsigned short k) {
	struct timeval* zero = (struct timeval*) calloc(1, sizeof(struct timeval));
	zero->tv_usec = 0;
	zero->tv_sec = 0;
	struct itimerval* timerval = (struct itimerval*) calloc(1, sizeof(struct itimerval));
	timerval->it_value = *zero;
	timerval->it_interval = *zero;
	setitimer(ITIMER_REAL, timerval, NULL);
}
