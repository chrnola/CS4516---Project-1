/*
 * Frame.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#include "Frame.h"

Frame::Frame() {
	// TODO Auto-generated constructor stub

}

Frame::~Frame() {
	// TODO Auto-generated destructor stub
}

unsigned char* Frame::Serialize() {
	unsigned char* data = (unsigned char*) calloc(strlen((char*) this->payload) + FRAME_HEAD + 1, sizeof(unsigned char));
	this->type == ack ? strcat((char*) data, "1") : strcat((char*) data, "0");
	strcat((char*) data, (char*) Utils::itoa(this->seq));
	this->end ? strcat((char*) data, "1") : strcat((char*) data, "0");
	strcat((char*) data, (char*) this->payload);
	return data;
}

Frame* Frame::Unserialize(char* d) {
	Frame* f = new Frame();
	d[0] == 0 ? f->type = ack : f->type = data;
	string str(d);
	f->seq = (unsigned short) atoi(str.substr(1,5).c_str());
	d[6] == 0 ? f->end = false : f-> end = true;
	f->payload = (unsigned char*) str.substr(7,str.size()).c_str();
	return f;
}
