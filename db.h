// db.h
// Author: Chris Pinola


#ifndef ___DB_H_INCLUDE___
#define ___DB_H_INCLUDE___

#include "did.h"

using namespace std;

// Author: Chris Pinola
//returns true if connected to MySQL, false otherwise
bool connectToDB();

// Author: Chris Pinola
//closes socket to MySQL DB and frees memory structures
void disconnectFromDB();

// Author: Chris Pinola
//returns the number of records in the public table
int getRowCountPublic();

// Author: Chris Pinola
//returns the number of records in the admin table
int getRowCountAdmin();

// Author: Chris Pinola
//adds a new row to the 'public' table, if there's room
//on success: returns person's new id
//on success: returns negative number
long createMissing(const char *first, const char *last, const char *location);

// Author: Chris Pinola
//checks the users table to see if we should grant admin rights to user
//true = valid credentials
bool authenticateUser(const char *username, const char *password);

// Author: Chris Pinola
// returns true is the given first AND last name are found together
// in the same row in the database, otherwise false
bool queryAdmin(const char *first, const char *last);

// Author: Chris Pinola
//returns a newline-delimited c-string with all the locations that
// have unidentified bodies
char *locationsWithMissing();

// Author: Chris Pinola
//creates a new missing person record in the ADMIN table
//success = true
bool createMissingAdmin(const char *id, const char *location, const char *firstName, const char *lastName);

// Author: Chris Pinola
// allows an admin to go back an add a name to an old record that lacks one
//success = true
bool positiveID(const char *id, const char *firstName, const char *lastName);

// Author: Chris Pinola
// allows an admin to delete a row by id from the PUBLIC table
// success = true
bool removeFromPublic(const char *id);

// Author: Chris Pinola
// allows an admin to delete a row by id from the ADMIN table
// success = true
bool removeFromAdmin(const char *id);

// Author: Chris Pinola
// allows an admin to create a new user and initalize their password
bool newUser(const char *username, const char *password);

// Author: Chris Pinola

bool changePassword(const char *username, const char *oldPass, const char *newPass);

bool addPhotoPublic(const char *id, char *file, long fileSize);

bool addPhotoAdmin(const char *id, char *file, long fileSize);

char *getPhotoAdmin(const char *id, long *fsize);

char *getPhotoPublic(const char *id, long *fsize);

#endif
