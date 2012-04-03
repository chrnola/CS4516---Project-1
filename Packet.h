/*
 * Packet.h
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#ifndef PACKET_H_
#define PACKET_H_

enum type{data, ack};

class Packet {
public:
	unsigned char type;
	unsigned short seq;
	bool end;
	unsigned char payload[];

	Packet();
	virtual ~Packet();
};

#endif /* PACKET_H_ */
