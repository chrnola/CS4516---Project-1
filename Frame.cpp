/*
 * Frame.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#include "Frame.h"

Frame::Frame() {
}

Frame::~Frame() {
}

/*
 * Author: Ray Short
 */
void Frame::Print() {
	cout << "\nFrame is data: " << (this->type+0 == data);
	cout << "\nFrame sequence number: " << this->seq;
	cout << "\nFrame is end: " << this->end;
	cout << "\nFrame payload length is: " << this->payloadLength;
	if(this->payloadLength > 0)
		cout << "\nFrame message: " << this->payload << "\n";
}

/*
 * Author: Ray Short
 */
unsigned char* Frame::Serialize() {
	unsigned char* data = (unsigned char*) calloc(this->payloadLength + FRAME_HEAD, sizeof(unsigned char));
	this->type == ack ? memcpy(data, "1", 1) : memcpy(data, "0", 1);
	memcpy(data + 1, Utils::itoa(this->seq), 5);
	this->end ? memcpy(data + 6, "1", 1) : memcpy(data + 6, "0", 1);
	memcpy(data + 7, Utils::itoa(this->payloadLength), 5);
	memcpy(data + 12, this->payload, this->payloadLength);
	return data;
}

/*
 * Author: Ray Short
 */
Frame* Frame::Unserialize(char* d) {
	Frame* f = new Frame();
	d[0] == 0 ? f->type = ack : f->type = data;
	string str(d);
	f->seq = (unsigned short) atoi(str.substr(1, 5).c_str());
	d[6] == 0 ? f->end = false : f-> end = true;
	f->payloadLength = (unsigned short) atoi(str.substr(7, 5).c_str());
	f->payload = (unsigned char*) str.substr(12, f->payloadLength).c_str();
	return f;
}
