/*
 * Packet.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#include "Packet.h"

Packet::Packet() {
}

Packet::~Packet() {
}

/*
 * Author: Ray Short
 */
void Packet::Print() {
	cout << "Packet is data: " << (this->type == DATA)<<endl;
	cout << "Packet sequence number: " << this->seq<<endl;
	cout << "Packet is end: " << this->end<<endl;
	cout << "Packet payload length is: " << this->payloadLength<<endl;
	cout << "Packet message: " << this->payload << endl;
}

/*
 * Author: Ray Short
 */
// serialize Packet into a character array
unsigned char* Packet::Serialize(){
	unsigned char* data = (unsigned char*) calloc(this->payloadLength + PACKET_HEAD, sizeof(unsigned char));
	this->type == ACK ? memcpy(data, "1", 1) : memcpy(data, "0", 1);
	memcpy(data + 1, Utils::itoa(this->seq), 5);
	this->end ? memcpy(data + 6, "1", 1) : memcpy(data + 6, "0", 1);
	memcpy(data + 7, Utils::itoa(this->payloadLength), 5);
	memcpy(data + 12, this->payload, this->payloadLength);
	return data;
}

/*
 * Author: Ray Short
 */
// make a Packet out of a previously serialized Packet
Packet* Packet::Unserialize(char* d) {
	Packet* p = new Packet();
	string str((char*)d);
	d[0] == 0x30 ? p->type = DATA : p->type = ACK;
	p->seq = (unsigned short) atoi(str.substr(1,5).c_str());
	d[6] == 0x30 ? p->end = false : p-> end = true;
	p->payloadLength = (unsigned short) atoi(str.substr(7,5).c_str());
	p->payload = (unsigned char*) calloc(p->payloadLength, sizeof(unsigned char));
	memcpy(p->payload, str.substr(12, p->payloadLength).c_str(), p->payloadLength);
	return p;
}
