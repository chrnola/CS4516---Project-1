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
	long mLength = m -> getContentSize();
	mLength += strlen(m -> getCmd());
	mLength += 22;
	unsigned char *mess = m -> serialize(mLength);
	
	//make packets (and be greedy about it)!
	int pToMake = mLength / MAX_PACKET;
	int leftover = mLength % MAX_PACKET;
	
	fflush(stdout);
	int s = MAX_SEQ;
	//make whole packets
	for (int i = 1; i <= pToMake; i++){
		Packet *p = new Packet();
		p->type = DATA;
		inc(s);
		p->seq = s;
		if((i == pToMake) && (leftover == 0)){
			p->end = true;
		} else{
			p->end = false;
		}
		p->payload = (unsigned char *) calloc(MAX_PACKET, sizeof(unsigned char));
		memcpy(p->payload, mess, MAX_PACKET);
		mess += MAX_PACKET;
		p->payloadLength = (unsigned short) MAX_PACKET;
		NetworkLayer::ToDataLinkLayer(p);
	}
	
	//make partial packet
	if(leftover > 0){
		Packet *p = new Packet();
		p->type = DATA;
		p->end = true;
		inc(s);
		p->seq = s;
		p->payload = (unsigned char *) calloc(leftover, sizeof(unsigned char));
		memcpy(p->payload, mess, leftover);
		p->payloadLength = (unsigned short) leftover;
		NetworkLayer::ToDataLinkLayer(p);
	}
	
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
			cout << "NL - Pulling packet from recvPackets" << endl;
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
	cout << "Assembling packets into a message" << endl;
	unsigned char *result = (unsigned char *) malloc(sizeof(unsigned char) * MAX_PACKET * buildBuffer->size());
	for(int i = 0; i < (int)buildBuffer->size(); i++){
		Packet *tPack = buildBuffer->front();
		buildBuffer->pop();
		unsigned char *current = result + (i * MAX_PACKET * sizeof(unsigned char));
		memcpy(current, tPack->payload, tPack->payloadLength);
		//*current = tPack->payload;
	}
	
	return Message::unserialize(result);
}

Message *NetworkLayer::FromDataLinkLayer(){
	//get from shared buffer
	//reconstruct
	queue<Packet*> buildBuffer;
	Packet *thisPacket;
	
	while(true){
		thisPacket = ReceiveAPacket(&buildBuffer);
		if(thisPacket->end){
			return Assemble(&buildBuffer);
		}
	}
	
	return NULL;
}



