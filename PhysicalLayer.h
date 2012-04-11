
/*
 * PhysicalLayer.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Aaron Sikes
 */

#ifndef PHYSICAL_H_
#define PHYSICAL_H_

#include "did.h"

using namespace std;

extern queue<Frame*> sendFrames, recvFrames;
extern pthread_mutex_t mutSF, mutRF;
extern bool debug;
extern bool verboseDebug;

class PhysicalLayer {
private:
	int sockfd;
	bool connected;
public:
	void start(pthread_t *PL_thread);
	void run();
	static unsigned char* FoldSerializedFrame(unsigned char* sFrame, int len);  //Note, sFrame may no longer be a valid pointer
	static bool FrameValid(char* sFrame, int length);
	void SendAFrame();
	void ReceiveFrames();
	PhysicalLayer(const char *hostname);
	PhysicalLayer(int sockfd);
	virtual ~PhysicalLayer();
};

#endif /* PHYSICAL_H_ */
