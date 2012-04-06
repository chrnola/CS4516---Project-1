/*
 * Packet.h
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#ifndef PACKET_H_
#define PACKET_H_

#include "did.h"

class Packet {
public:
	unsigned char type;
	unsigned short seq;
	bool end;
	unsigned short payloadLength;
	unsigned char* payload;
	
	Packet();
	virtual ~Packet();
	unsigned char *Serialize();
	static Packet* Unserialize(char* d);
};

#endif /* PACKET_H_ */
