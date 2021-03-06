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

class NetworkLayer {
	public:
		NetworkLayer();
		virtual ~NetworkLayer();
		
		void ToDataLinkLayer(Packet *p);
		Message *FromDataLinkLayer();
		void FromApplicationLayer(Message *m);
};

#endif
