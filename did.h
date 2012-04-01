// did.h

#ifndef ___DID_H_INCLUDE___
#define ___DID_H_INCLUDE___

#include <string>

using namespace std;

void startClient();

//returns true if connected to MySQL, false otherwise
bool connectToDB();

//returns the number of records in the public table
int getRowCountPublic();

//returns the number of records in the admin table
int getRowCountAdmin();

//checks the users table to see if we should grant admin rights to user
//true = valid credentials
//TODO: compare MD5 hash instead
bool authenticateUser(string username, string password);

#endif
