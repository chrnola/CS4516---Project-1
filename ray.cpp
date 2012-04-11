#include "did.h"

queue<Packet*> sendPackets, recvPackets;
queue<Frame*> sendFrames, recvFrames;
pthread_mutex_t mutSP, mutRP, mutSF, mutRF;

/*
 * Author: Ray Short
 */
int main(void) {
	Packet* p = new Packet();
	p->type = data;
	p->seq = 24856;
	p->end = true;
	p->payload = (unsigned char*) calloc(45, sizeof(unsigned char));
	string s = "This is a test packet. Yay it works.\n";
	char* str = const_cast<char*>(s.c_str());
	p->payload = (unsigned char*) strcpy((char*)p->payload, str);
	
	unsigned char* string = p->Serialize();
	
	DataLink* d = new DataLink();
	d->GoBack1();
}


