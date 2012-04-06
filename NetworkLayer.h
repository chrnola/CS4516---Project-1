// NetworkLayer.h
// Author: Chris Pinola

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

class Message;
class Packet;

#include "did.h"

extern Packet** sendPackets, ** recvPackets;
extern char sPNum, sPCur, rPNum, rPCur;
extern pthread_mutex_t mutSP, mutRP;

class NetworkLayer {
	public:
		NetworkLayer();
		virtual ~NetworkLayer();
		
		//static unsigned char *Serialize(Message *m);
		//static Message *Unserialize(const unsigned char *d);
		void ToDataLinkLayer(Packet *p);
		void FromDataLinkLayer(Packet *p);
		void FromApplicationLayer(Message *m);
		void ToApplicationLayer(unsigned char *message);
};

#endif
