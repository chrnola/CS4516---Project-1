/*
 * DataLink.h
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#ifndef DATALINK_H_
#define DATALINK_H_

extern bool debug;
extern bool verboseDebug;

class Frame;
class Packet;

#include "did.h"

using namespace std;

// shared memory structures
extern queue<Packet*> sendPackets, recvPackets;
extern queue<Frame*> sendFrames, recvFrames;
extern pthread_mutex_t mutSP, mutRP, mutSF, mutRF;

#define MAX_READY 20

enum Event{arrival, error, timeout, pktReady, none};

class DataLink {
public:
	DataLink();
	virtual ~DataLink();
	void GoBack1(void);
	void GoBackN(void);
	void MakeFrames(Packet* p);
	void SendAck(void);
	Event* WaitForEvent(Event* e);
	Packet* FromNetworkLayer(Packet* p);
	void ToNetworkLayer(void);
	Frame* FromPhysicalLayer(Frame* r);
	void ToPhysicalLayer(Frame* s);
	void StartTimer(unsigned short k);
	void StopTimer(unsigned short k);
	
private:
	queue<Frame*> window, ready, reconstructFrames;
	unsigned short nextSend, frameExpect;
};

#endif /* DATALINK_H_ */
