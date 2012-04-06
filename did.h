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

#define WELL_KNOWN_PORT 2777 //Why not?
#define PACKET_HEAD 6
#define FRAME_HEAD 6
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

//returns true if connected to MySQL, false otherwise
bool connectToDB();

//closes socket to MySQL DB and frees memory structures
void disconnectFromDB();

//returns the number of records in the public table
int getRowCountPublic();

//returns the number of records in the admin table
int getRowCountAdmin();

//adds a new row to the 'public' table, if there's room
//on success: returns person's new id
//on success: returns negative number
long createMissing(const char *first, const char *last, const char *location);

//checks the users table to see if we should grant admin rights to user
//true = valid credentials
bool authenticateUser(const char *username, const char *password);

bool queryAdmin(const char *first, const char *last);

char *locationsWithMissing();

bool createMissingAdmin(const char *id, const char *location, const char *firstName, const char *lastName);

bool positiveID(const char *id, const char *firstName, const char *lastName);

bool removeFromPublic(const char *id);

bool removeFromAdmin(const char *id);

bool newUser(const char *username, const char *password);

bool changePassword(const char *username, const char *oldPass, const char *newPass);

bool addPhotoPublic(const char *id, char *file, long fileSize);

bool addPhotoAdmin(const char *id, char *file, long fileSize);

char *getPhotoAdmin(const char *id, unsigned long *fsize);

char *getPhotoPublic(const char *id, unsigned long *fsize);

#endif
