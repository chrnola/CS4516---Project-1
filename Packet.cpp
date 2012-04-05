/*
 * Packet.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#include "Packet.h"

Packet::Packet() {
	// TODO Auto-generated constructor stub

}

Packet::~Packet() {
	// TODO Auto-generated destructor stub
}

unsigned char* Packet::Serialize(){
	unsigned char* data = (unsigned char*) calloc(strlen((char*) this->payload) + PACKET_HEAD + 1, sizeof(unsigned char));
	this->type == ack ? strcat((char*) data, "1") : strcat((char*) data, "0");
	strcat((char*) data, (char*) Utils::itoa(this->seq));
	this->end ? strcat((char*) data, "1") : strcat((char*) data, "0");
	strcat((char*) data, (char*) this->payload);
	return data;
}

Packet* Packet::Unserialize(char* d) {
	Packet* p = new Packet();
	d[0] == 0 ? p->type = ack : p->type = data;
	string str(d);
	p->seq = (unsigned short) atoi(str.substr(1,5).c_str());
	d[6] == 0 ? p->end = false : p-> end = true;
	p->payload = (unsigned char*) str.substr(7,str.size()).c_str();
	return p;
}
