// NetworkLayer.h
// Author: Chris Pinola

#include "Message.h"

class NetworkLayer {
	public:
		NetworkLayer();
		virtual ~NetworkLayer();
		
		static unsigned char *Serialize(Message *m);
		static Message *Unserialize(const unsigned char *d);
		void ToDataLinkLayer(Packet *p);
		void FromDataLinkLayer(unsigned char *p);
		//void ToApplicationLayer(
};