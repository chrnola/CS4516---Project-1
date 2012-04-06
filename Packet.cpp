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
	unsigned char* data = (unsigned char*) calloc(this->payloadLength + PACKET_HEAD, sizeof(unsigned char));
	this->type == ack ? memcpy(data, "1", 1) : memcpy(data, "0", 1);
	memcpy(data + 1, Utils::itoa(this->seq), 5);
	this->end ? memcpy(data + 6, "1", 1) : memcpy(data + 6, "0", 1);
	memcpy(data + 7, Utils::itoa(this->payloadLength), 5);
	memcpy(data + 12, this->payload, this->payloadLength);
	return data;
}

Packet* Packet::Unserialize(char* d) {
	cout << "Got a string to unserialize: " << d;
	Packet* p = new Packet();
	d[0] == 0 ? p->type = ack : p->type = data;
	string str(d);
	p->seq = (unsigned short) atoi(str.substr(1,5).c_str());
	d[6] == 0 ? p->end = false : p-> end = true;
	p->payloadLength = (unsigned short) atoi(str.substr(7,5).c_str());
	p->payload = (unsigned char*) str.substr(12,str.size()).c_str();
	return p;
}
