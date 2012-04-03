/*
 * DataLink.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#include "DataLink.h"

DataLink::DataLink() {
	// make a dummy network layer thread that will make and send packets to the DLL
	
}

DataLink::~DataLink() {
	// TODO Auto-generated destructor stub
}

void DataLink::GoBack1() {
	unsigned short fextSend = 0, frameExpect = 0;
	Frame r, s;
	Packet buffer;
	Event event;
	
	FromNetworkLayer(&buffer);
	s.info = buffer;
	s.seq = nextSend;
	ToPhysicalLayer(&s);
	StartTimer(0);
	
	while(true) {
		WaitForEvent(&event);
		if(event == arrival) {
			FromPhysicalLayer(&r);
			if(r.type == ack) {
				StopTimer(0);
				FromNetworkLayer(&buffer);
				inc(nextSend)
			} else if(r.seq == frameExpect) {
				ToNetworkLayer(&r.payload);
				inc(frameExpect);
			}
		}
		s.payload = buffer;
		s.seq = nextSend;
		ToPhysicalLayer(&s);
		StartTimer(0);
	}
}

void DataLink::GoBackN() {
	int next_send = 0, ack_expect = 0, frame_expect = 0;
	Frame r;
	Packet buffer[MAX_SEQ + 1];
	int num_buff = 0, i = 0;
	Event event;

	while (true) {
		WaitForEvent(&event);

		switch (event) {
		case network_ready:
			FromNetworkLayer(&buffer[next_send]);
			num_buff++;
			SendData(next_send, frame_expect, buffer);
			inc(next_send);
			break;
		case arrival:
			FromPhysicalLayer(&r);
			if(r.seq == frame_expect) {
				ToNetworkLayer(r.payload);
				inc(frame_expect);
			}
			if(r.type == ack && r.seq == ack_expect) {
				StopTimer(r.seq);
				num_buff--;
				inc(ack_expect);
			}
			break;
		case error: break;
		case timeout:
			next_send = ack_expect;
			for(i = 0;i < num_buff;i++) {
				SendData(next_send, frame_expect, buffer);
				inc(next_send);
			}
			break;
		}

		if(num_buff < MAX_SEQ)
			EnableNetworkLayer();
		else
			DisableNetworkLayer();
	}
}

static void DataLink::SendData(unsigned int frame_num, unsigned int frame_expect, Packet buffer[]) {

}

void DataLink::WaitForEvent(event* e) {

}

void DataLink::FromNetworkLayer(Packet* p) {
	Packet* p;
	p = s_packets[0];
	int size = sizeof(p->payload);
	if(size <= MAX_FRAME) {
	
	} else {
	
	}
}
void DataLink::ToNetworkLayer(unsigned char p[]) {

}
void DataLink::FromPhysicalLayer(Frame* r) {

}
void DataLink::ToPhysicalLayer(Frame* s) {

}
void DataLink::StartTimer(unsigned short k) {

}
void DataLink::StopTimer(unsigned short k) {

}
void DataLink::EnableNetworkLayer(void) {

}
void DataLink::DisableNetworkLayer(void) {

}
