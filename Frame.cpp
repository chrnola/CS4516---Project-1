/*
 * Frame.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#include "Frame.h"

#include <string.h>

Frame::Frame() {
}

Frame::~Frame() {
}

/*
 * Author: Ray Short
 */
void Frame::Print() {
	cout << "Frame is data: " << (this->type == DATA)<<endl;
	cout << "Frame sequence number: " << this->seq<<endl;
	cout << "Frame is end: " << this->end<<endl;
	cout << "Frame payload length is: " << this->payloadLength<<endl;
	if(this->payloadLength > 0)
		cout << "Frame message: " << this->payload <<endl;
}

/*
 * Author: Ray Short
 */
// serialize Frame into a character array
unsigned char* Frame::Serialize() {
	unsigned char* data = (unsigned char*) calloc(this->payloadLength + FRAME_HEAD, sizeof(unsigned char));
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
// make a Frame out of a previously serialized Frame
Frame* Frame::Unserialize(char* d) {
	Frame* f = new Frame();
	d[0] == 0x30 ? f->type = DATA : f->type = ACK;
	string str(d);
	f->seq = (unsigned short) atoi(str.substr(1, 5).c_str());
	d[6] == 0x30 ? f->end = false : f-> end = true;
	f->payloadLength = (unsigned short) atoi(str.substr(7, 5).c_str());
	f->payload = (unsigned char*) calloc(f->payloadLength, sizeof(unsigned char));
	memcpy(f->payload, str.substr(12, f->payloadLength).c_str(), f->payloadLength);
	return f;
}
