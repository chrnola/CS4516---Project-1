// db.h

#ifndef ___DB_H_INCLUDE___
#define ___DB_H_INCLUDE___

#include "did.h"

using namespace std;

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

char *getPhotoAdmin(const char *id, long *fsize);

char *getPhotoPublic(const char *id, long *fsize);

#endif
