// NetworkLayer.h
// Author: Chris Pinola

#ifndef __NETWORKLAYER_H__
#define __NETWORKLAYER_H__

class Message;
class Packet;
class DataLink;

#include "did.h"

using namespace std;

extern queue<Packet*> sendPackets, recvPackets;
extern pthread_mutex_t mutSP, mutRP;
queue<Packet*> buildPackets;
volatile bool readyToBuild = false;

class NetworkLayer {
	public:
		NetworkLayer();
		virtual ~NetworkLayer();
		
		//static unsigned char *Serialize(Message *m);
		//static Message *Unserialize(const unsigned char *d);
		void ToDataLinkLayer(Packet *p);
		Message *FromDataLinkLayer();
		void FromApplicationLayer(Message *m);
		void Run();
		void SendAPacket(Packet *p);
		void ReceivePackets();
		void ToApplicationLayer(unsigned char *message);
};

#endif
