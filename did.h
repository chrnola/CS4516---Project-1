// did.h

#ifndef ___DID_H_INCLUDE___
#define ___DID_H_INCLUDE___

#include <string>
#include <iostream>
#include <stdlib.h>

#include "Frame.h"
#include "Packet.h"
#include "DataLink.h"
#include "PhysicalLayer.h"

#define WELL_KNOWN_PORT 2777 //Why not?

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
unsigned long createMissing(const char *first, const char *last, const char *location);

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

#endif
