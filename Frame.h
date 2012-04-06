/*
 * Frame.h
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#ifndef FRAME_H_
#define FRAME_H_

#include "did.h"

class Frame {
public:
	unsigned char type;
	unsigned short seq;
	bool end;
	unsigned short payloadLength;
	unsigned char* payload;

	Frame();
	virtual ~Frame();
	void Print(void);
	unsigned char* Serialize(void);
	static Frame* Unserialize(char* d);
};

#endif /* FRAME_H_ */
