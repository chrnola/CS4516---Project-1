/*
 * Frame.h
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#ifndef FRAME_H_
#define FRAME_H_

class Frame {
public:
	unsigned char type;
	unsigned short seq;
	bool end;
	unsigned char* payload;

	Frame();
	virtual ~Frame();
	unsigned char* Serialize();
	static Frame* Unserialize(char* d);
};

#endif /* FRAME_H_ */
