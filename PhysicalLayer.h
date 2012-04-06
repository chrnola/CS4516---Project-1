
/*
 * PhysicalLayer.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Aaron Sikes
 */

#ifndef PHYSICAL_H_
#define PHYSICAL_H_
#include <pthread.h>

class PhysicalLayer {
private:
	int sockfd;
public:
	void start(pthread_t *PL_thread);
	void run();
	static char* FoldSerializedFrame(char* sFrame, int len);  //Note, sFrame may no longer be a valid pointer
	static bool FrameValid(char* sFrame, int length);
	PhysicalLayer(const char *hostname);
	PhysicalLayer(int sockfd);
	virtual ~PhysicalLayer();
};

#endif /* PHYSICAL_H_ */
