/*
 * DataLink.h
 *
 *  Created on: Apr 2, 2012
 *      Author: Ray Short
 */

#ifndef DATALINK_H_
#define DATALINK_H_

#define MAX_SEQ 65535
#define inc(k) if (k < MAX_SEQ) k++; else k = 0;

typedef enum Event{arrival, error, timeout, network_ready};

class DataLink {
public:
	DataLink();
	virtual ~DataLink();
	void GoBack1(void);
	void GoBackN(void);
	static void SendData(unsigned int frame_num, unsigned int frame_expect, Packet buffer[]);
	void WaitForEvent(event* e);
	void FromNetworkLayer(Packet* p);
	void ToNetworkLayer(unsigned char p[]);
	void FromPhysicalLayer(Frame* r);
	void ToPhysicalLayer(Frame* s);
	void StartTimer(unsigned short k);
	void StopTimer(unsigned short k);
	void EnableNetworkLayer(void);
	void DisableNetworkLayer(void);
};

#endif /* DATALINK_H_ */
