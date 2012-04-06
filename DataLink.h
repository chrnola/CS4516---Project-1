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

#define MAX_SEQ 65535
#define inc(k) if (k < MAX_SEQ) k++; else k = 0;
#define PACKET_HEAD 6
#define FRAME_HEAD 6
#define MAX_FRAME 150
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
	void SendData(unsigned int frame_num, unsigned int frame_expect, Packet buffer[]);
	void WaitForEvent(Event* e);
	Packet* FromNetworkLayer(Packet* p);
	void ToNetworkLayer(void);
	void FromPhysicalLayer(Frame* r);
	void ToPhysicalLayer(Frame* s);
	void StartTimer(unsigned short k);
	void StopTimer(unsigned short k);
	void EnableNetworkLayer(void);
	void DisableNetworkLayer(void);
	

	//void HandleTimeout(int sig);
	
	// testing variables
	Packet** s_packets;
	Packet** r_packets;
	Frame** r_frames;
	
private:
	Frame** window;
	Frame** ready;
	char numReady, currReady, numWindow, currWindow;
	unsigned short nextSend, frameExpect;
};

#endif /* DATALINK_H_ */
