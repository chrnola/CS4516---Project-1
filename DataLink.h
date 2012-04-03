/*
 * DataLink.h
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#ifndef DATALINK_H_
#define DATALINK_H_

#include "did.h"

#define MAX_SEQ 65535
#define inc(k) if (k < MAX_SEQ) k++; else k = 0;
#define PACKET_HEAD 4
#define MAX_FRAME 150

typedef enum Event{arrival, error, timeout, network_ready};

class DataLink {
public:
	DataLink();
	virtual ~DataLink();
	void GoBack1(void);
	void GoBackN(void);
	void SendData(unsigned int frame_num, unsigned int frame_expect, Packet buffer[]);
	void WaitForEvent(Event* e);
	void FromNetworkLayer(Packet* p);
	void ToNetworkLayer(unsigned char p[]);
	void FromPhysicalLayer(Frame* r);
	void ToPhysicalLayer(Frame* s);
	void StartTimer(unsigned short k);
	void StopTimer(unsigned short k);
	void EnableNetworkLayer(void);
	void DisableNetworkLayer(void);
	
	//static unsigned char* Serialize(Msg* m);
	static unsigned char* Serialize(Packet* p);
	//static Msg* UnserializeM(unsigned char d[]);
	static Packet* UnserializeP(const unsigned char* d);
	static unsigned char* itoa(unsigned short n);
	
	// testing variables
	Packet* s_packets;
	Packet* r_packets;
	Frame* r_frames;
	
private:
	Frame* window;
	Frame* ready;
};

#endif /* DATALINK_H_ */
