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
	//useless
// 	while(true){
// 		if(!pthread_mutex_trylock(&mutSP)){
// 			if(!sendPackets.empty()){
// 				SendAPacket();
// 			}
// 		}
// 		ReceivePackets();
// 	}
}

void NetworkLayer::SendAPacket(Packet *p){
	//useless
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
	pthread_mutex_lock(&mutSP);
	
	pthread_mutex_unlock(&mutSP);
}

void NetworkLayer::ToDataLinkLayer(Packet *p){
	//to DL shared buffer
	pthread_mutex_lock(&mutSP);
	sendPackets.push(p);
	pthread_mutex_unlock(&mutSP);
}

Packet *ReceiveAPacket(queue<Packet*> *buildBuffer){
	bool hasPacket = false;
	Packet *result;
	
	while(!hasPacket){
		pthread_mutex_lock(&mutRP);
		if(!recvPackets.empty()){
			result = recvPackets.front();
			buildBuffer->push(result);
			recvPackets.pop();
			hasPacket = true;
		}
		pthread_mutex_unlock(&mutRP);
	}
	return result;
}

Message *Assemble(queue<Packet*> *buildBuffer){
	Message *result = (Message *) malloc(sizeof(char) * MAX_PACKET * buildBuffer->size());
	for(int i = 0; i < buildBuffer->size(); i++){
		Packet *tPack = buildBuffer->front();
		buildBuffer->pop();
		Message *current = result + (i * MAX_PACKET * sizeof(char));
		*current = tPack->payload;
	}
	
	return result;
}

Message *NetworkLayer::FromDataLinkLayer(){
	//get from shared buffer
	//reconstruct
	queue<Packet*> buildBuffer;
	Packet *thisPacket;
	
	while(true){
		thisPacket = ReceiveAPacket(&buildBuffer);
		if(thisPacket->end) return Assemble(&buildBuffer);
	}
	
	return NULL;
}



