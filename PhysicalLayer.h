
/*
 * PhysicalLayer.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Aaron Sikes
 */

#ifndef PHYSICAL_H_
#define PHYSICAL_H_

class PhysicalLayer {
private:
	int sockfd;
	void run();
public:
	static char* FoldSerializedFrame(char* sFrame);  //Note, sFrame may no longer be a valid pointer
	static bool FrameValid(char* sFrame, int length);
	PhysicalLayer(const char *hostname);
	PhysicalLayer(int sockfd);
	virtual ~PhysicalLayer();
};

#endif /* PHYSICAL_H_ */
