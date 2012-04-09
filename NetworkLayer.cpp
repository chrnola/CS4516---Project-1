// NetworkLayer.cpp
// Author: Chris Pinola

#include "NetworkLayer.h"

// Author: Chris Pinola
NetworkLayer::NetworkLayer(){
	//pthread_mutex_init(&mutSP, NULL);
	//pthread_mutex_init(&mutRP, NULL);
}

NetworkLayer::~NetworkLayer(){
}

void NetworkLayer::Run(){
// 	while(true){
// 		if(!pthread_mutex_trylock(&mutSP)){
// 			if(!sendPackets.empty()){
// 				SendAPacket();
// 			}
// 		}
// 		ReceivePackets();
// 	}
}

void NetworkLayer::SendAPacket(){
	
}

void NetworkLayer::ReceivePackets(){
	
}

void NetworkLayer::FromApplicationLayer(Message *m){
	unsigned char *mess = m -> serialize();
	short mLength = m -> getContentSize();
	mLength += strlen(m -> getCmd());
	
	//make packets (and be greedy about it)!
	int pToMake = mLength / MAX_PACKET;
	int leftover = mLength % MAX_PACKET;
	
	int s = MAX_SEQ;
	//make whole packets
	for (int i = 1; i <= pToMake; i++){
		Packet *p = new Packet();
		p->type = data;
		inc(s);
		p->seq = s;
		if((i == pToMake) && (leftover == 0)){
			p->end = true;
		} else{
			p->end = false;
		}
		memcpy(p->payload, mess, MAX_PACKET);
		mess += MAX_PACKET;
		p->payloadLength = (unsigned short) MAX_PACKET;
		NetworkLayer::ToDataLinkLayer(p);
	}
	
	//make partial packet
	if(leftover > 0){
		Packet *p = new Packet();
		p->type = data;
		p->end = true;
		inc(s);
		p->seq = s;
		memcpy(p->payload, mess, leftover);
		p->payloadLength = (unsigned short) leftover;
		NetworkLayer::ToDataLinkLayer(p);
	}
	
}

void NetworkLayer::ToApplicationLayer(unsigned char *message){
	Message *m = Message::unserialize(message);
	//to app layer?
}

void NetworkLayer::ToDataLinkLayer(Packet *p){
	//to DL shared buffer
	//pthread_mutex_lock(&mutSP);
	//sendPacket
}

void NetworkLayer::FromDataLinkLayer(Packet *p){
	//get from shared buffer
	//reconstruct
}

