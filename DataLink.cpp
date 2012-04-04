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
	ready = (Frame*) calloc(20, sizeof(Frame)); // for frames ready to send
	
	// should only send 1 packet for testing
	s_packets = (Packet*) calloc(1, sizeof(Packet)); 
	// should only ever receive 1 packet
	r_packets = (Packet*) calloc(1, sizeof(Packet)); 
	r_frames = (Frame*) calloc(1, sizeof(Frame)); // receive only one frame
	
	// make a dummy network layer thread that will make and send packets to the DLL
}

DataLink::~DataLink() {
	free(window);
	free(ready);
	free(s_packets);
	free(r_packets);
}

void DataLink::GoBack1() {
	unsigned short nextSend = 0, frameExpect = 0;
	Frame r, s;
	Packet buffer;
	Event event;
	
	FromNetworkLayer(&buffer);
	s.payload = DataLink::Serialize(&buffer);
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
				ToNetworkLayer(r.payload);
				inc(frameExpect);
			}
		}
		s.payload = DataLink::Serialize(&buffer);
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

void DataLink::SendData(unsigned int frame_num, unsigned int frame_expect, Packet buffer[]) {

}

void DataLink::WaitForEvent(Event* e) {

}

void DataLink::FromNetworkLayer(Packet* p) {
	Packet pac;
	pac = s_packets[0];
	int size = sizeof(pac.payload) + PACKET_HEAD;
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

Packet* DataLink::UnserializeP(const unsigned char* d) {
	Packet* p = (Packet*) calloc(1, sizeof(Packet));//new Packet();
	string str ((const char*) d);
	d[0] == 0 ? p->type = ack : p->type = data;
	const char* temp = (const char*) ((str.substr(1,5)).c_str());
	p->seq = (unsigned short) atoi(temp);
	d[6] == 0 ? p->end = false : p-> end = true;
	p->payload = (unsigned char*) (const_cast<char*> ((str.substr(7, str.size()-1)).c_str()));
	return p;
}

unsigned char* DataLink::itoa(unsigned short n) {
	char val = 0;
	unsigned char* num = (unsigned char*) calloc(5, sizeof(unsigned char));
	if(n > 9999) {
		val = n / 10000;
		num[0] = val + 0x30;
		n -= val * 10000;
	} else num[0] = 0;
	
	if(n > 999) {
		val = n / 1000;
		num[1] = val + 0x30;
		n -= val * 1000;
	} else num[1] = 0;
	
	if(n > 99) {
		val = n / 100;
		num[2] = val + 0x30;
		n -= val * 100;
	} else num[2] = 0;
	
	if(n > 9) {
		val = n / 10;
		num[3] = val + 0x30;
		n -= val * 10;
	} else num[3] = 0;
	
	if(n > 0) {
		num[4] = n + 0x30;
	} else num[4] = 0;
	
	return num;
}
