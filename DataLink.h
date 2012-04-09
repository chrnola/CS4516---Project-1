/*
 * DataLink.h
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#ifndef DATALINK_H_
#define DATALINK_H_

class Frame;
class Packet;

#include "did.h"

using namespace std;

// shared memory structures
extern queue<Packet*> sendPackets, recvPackets;
extern queue<Frame*> sendFrames, recvFrames;
extern pthread_mutex_t mutSP, mutRP, mutSF, mutRF;

#define MAX_READY 20

// make some of my own signals
#define SIGPSND 40
#define SIGFSND 41
#define SIGFRCV 42
#define SIGPRCV 43
#define SIGFERR 44

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
	char numReady, numWindow;
	unsigned short nextSend, frameExpect;
};

#endif /* DATALINK_H_ */
