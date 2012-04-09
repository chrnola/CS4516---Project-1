// did.h

#ifndef ___DID_H_INCLUDE___
#define ___DID_H_INCLUDE___

#include <string>
#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <fstream>
#include <queue>

#include "Frame.h"
#include "Packet.h"
#include "DataLink.h"
#include "PhysicalLayer.h"
#include "Utils.h"
#include "Message.h"
#include "NetworkLayer.h"
#include "db.h"

#define WELL_KNOWN_PORT 2778 //Why not?
#define PACKET_HEAD 6
#define FRAME_HEAD 6
#define FRAME_TAIL 2
#define MAX_FRAME 150
#define MAX_PACKET 256
#define MAX_SEQ 65535

#define MAX_FIRST 15
#define MAX_LAST 20
#define MAX_LOCATION 36
#define MAX_ID 9
#define MAX_USER 20

#define MAX_SEND_PACKET 5
#define MAX_SEND_FRAME 1
#define MAX_RECV_PACKET 1
#define MAX_RECV_FRAME 1

#define inc(k) if (k < MAX_SEQ) k++; else k = 0;

enum Type{data, ack};

using namespace std;

void startClient();

#endif
