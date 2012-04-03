// did.h

#ifndef ___DID_H_INCLUDE___
#define ___DID_H_INCLUDE___

#include <string>
#include <iostream>
#include <stdlib.h>

#include "Frame.h"
#include "Packet.h"
#include "DataLink.h"

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
//true = success
bool createMissing(const char *first, const char *last, const char *location);

//checks the users table to see if we should grant admin rights to user
//true = valid credentials
//TODO: compare MD5 hash instead
bool authenticateUser(const char *username, const char *password);

#endif
