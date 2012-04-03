/*
 * Frame.h
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#ifndef FRAME_H_
#define FRAME_H_

enum type{data, ack};

class Frame {
public:
	char type;
	unsigned short seq;
	bool end;
	char payload[];
	unsigned short err;

	Frame();
	virtual ~Frame();
};

#endif /* FRAME_H_ */
