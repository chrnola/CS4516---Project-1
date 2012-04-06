// db.C
// Used for server side interactions with the database
// Author: Chris Pinola

#include "did.h"
#include <mysql.h>

//forward declare these function, not included in header,
// don't want to expose them to everyone else
int getRowCountTbl(const char*);
bool removeFromTableWhereCol(const char *table, const char *col, const char *id);
char *getPhoto(const char *id, unsigned long *fsize, const char *q1);
bool addPhoto(const char *id, char *file, long fileSize, const char *q1, const char *q2);

// the global MySQL connection object that will be used for all
// transactions. decided to keep it here so the server code won't
// have to worry about maintaining the connection object!
MYSQL mysql;

//Initalizes the MySQL C library, creates the global connection
// object and attempts to connect to WPI's server
// returns true if successful
// NOTE: should call disconnectFromDB at the end of session
bool connectToDB(){
	// initalizes the MySQL C library
	if(mysql_library_init(0, NULL, NULL)){
		return false;
	}

	// creates the MySQL connection object
	mysql_init(&mysql);

	// attempt to connect to the WPI server with the specified
	// credentials
 	if(!mysql_real_connect(&mysql, "mysql.wpi.edu", "db_user", 
			"zvRPqu", "cs4516_d12_t6", 0, NULL, 0)){
		return false;
	}

	// if all goes well, it worked!
	return true;
}

// calls library routines to deallocate MySQL objects
void disconnectFromDB(){
	mysql_close(&mysql);
	mysql_library_end();
}

// Returns number of rows in the public table
// Error if return value is < 0
int getRowCountPublic(){
	return getRowCountTbl("public");
}

// Returns number of rows in the admin table
// Error if return value is < 0
int getRowCountAdmin(){
	return getRowCountTbl("admin");
}

// Performs an aggregate count on the specified table
// Returns number < 0 in the event of an error
// NOTE: don't let the user pass input to this, to speed
// things up, the input isn't sanitized
int getRowCountTbl(const char* tbl){
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return -7; //connection couldn't be re-established
	}
	
	// construct the string that represents the query
	char qry[40];
	strcpy(qry, "SELECT COUNT(*) FROM ");
	strcat(qry, tbl);
	
	// issue the query to the server
	if(mysql_query(&mysql, qry) != 0){
		// attempts to fetch and output the error message
		// from mysql out to STDERR
		cerr << mysql_error(&mysql) << endl;
		return -1; //error
	}
	
	// if that worked, fetch the resulting record set and
	// store it for the time being
	MYSQL_RES *res = mysql_store_result(&mysql);
	
	// fetch the first row from the record set (the aggreagte
	// function will just return 1 row)
	MYSQL_ROW rw = mysql_fetch_row(res);

	// make sure the row object isn't just a NULL pointer
	// would mean empty result set
	if (rw != NULL){
		// check to make sure that there is at least 1
		// column in this row!
		if(mysql_num_fields(res) > 0){
			// now that we've extracted the row, we
			// can free the result set
			mysql_free_result(res);
			// return the first cell in the row, casted
			// back to an integer first
			return atoi(rw[0]);
			// success!!!!
		} else{
			// the row contains no columns!
			// doubt this is even possible
			// anyway, free the result set
			mysql_free_result(res);
			// error :(
			return -2;
		}
	}
	
	// empty result set! free the object anyway.
	mysql_free_result(res);
	// error :(
	return -3;
}

bool removeFromPublic(const char *id){
	return removeFromTableWhereCol("public", "id", id);
}

bool removeFromAdmin(const char *id){
	return removeFromTableWhereCol("admin", "personID", id);
}

bool removeFromTableWhereCol(const char *table, const char *col, const char *id){
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return false; //connection couldn't be re-established
	}
	
	char *d1 = "DELETE FROM "; char *d2 = " WHERE ";
	char *d3 = "=\'"; char *d4 = "\'";
	
	char *qry, *cleanId;
	
	//sanitize the only user-provided string
	cleanId = (char *) calloc((2*strlen(id)) + 1, sizeof(char));
	mysql_real_escape_string(&mysql, cleanId, id, strlen(id));
	
	int size = 1;
	size += strlen(d1) + strlen(d2) + strlen(d3) + strlen(d4);
	size += strlen(table) + strlen(col) + strlen(cleanId);
	
	qry = (char *) calloc(size, sizeof(char));
	
	strcpy(qry, d1); strcat(qry, table); strcat(qry, d2);
	strcat(qry, col); strcat(qry, d3); strcat(qry, cleanId);
	strcat(qry, d4);
	
	free(cleanId);
	
	// issue query to the server
	if(mysql_query(&mysql, qry) != 0){
		// attempts to fetch and output the error message
		// from mysql out to STDERR
		cerr << mysql_error(&mysql) << endl;
		free(qry); //deallocate query string
		return false; //error
	}
	
	free(qry); //deallocate query string
	
	// checks that the query we just executed only affected one row
	// like it was supposed to
	if(mysql_store_result(&mysql) == 0 && mysql_field_count(&mysql) == 0 &&
	 (unsigned long) mysql_affected_rows(&mysql) == 1){
		return true;
	}
	
	return false;
}

bool positiveID(const char *id, const char *firstName, const char *lastName){
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return false; //connection couldn't be re-established
	}
	
	char *qry, *cleanId, *cleanFirst, *cleanLast;
	
	//allocate temp strings for cleaning process
	cleanId = (char *) calloc((2*strlen(id)) + 1, sizeof(char));
	cleanFirst = (char *) calloc((2*strlen(firstName)) + 1, sizeof(char));
	cleanLast = (char *) calloc((2*strlen(lastName)) + 1, sizeof(char));
	
	//sanitize strings for db
	mysql_real_escape_string(&mysql, cleanId, id, strlen(id));
	mysql_real_escape_string(&mysql, cleanFirst, firstName, strlen(firstName));
	mysql_real_escape_string(&mysql, cleanLast, lastName, strlen(lastName));
	
	//declare constants
	char *qStart = "UPDATE admin SET firstName=\'";
	char *q1 = "\', lastName=\'";
	char *q2 = "\' WHERE personID=\'";
	char *qEnd = "\'";
	
	//calculate total length of qry
	int size = 1;
	size += strlen(qStart) + strlen(q1) + strlen(q2) + strlen(qEnd);
	size += strlen(cleanId) + strlen(cleanFirst) + strlen(cleanLast);
	
	//allocate final query string
	qry = (char *) calloc(size, sizeof(char));
	
	//piece it all together
	strcpy(qry, qStart); strcat(qry, cleanFirst); strcat(qry, q1);
	strcat(qry, cleanLast); strcat(qry, q2); strcat(qry, cleanId);
	strcat(qry, qEnd);
	
	//clean up temp strings used for sanitation
	free(cleanId); free(cleanFirst); free(cleanLast);
	
	// issue query to the server
	if(mysql_query(&mysql, qry) != 0){
		// attempts to fetch and output the error message
		// from mysql out to STDERR
		cerr << mysql_error(&mysql) << endl;
		free(qry); //deallocate query string
		return false; //error
	}
	
	free(qry); //deallocate query string
	
	// checks that the query we just executed only affected one row
	// like it was supposed to
	if(mysql_store_result(&mysql) == 0 && mysql_field_count(&mysql) == 0 &&
	 (unsigned long) mysql_affected_rows(&mysql) == 1){
		return true;
	}
	
	return false;
}

bool newUser(const char *username, const char *password){
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return false; //connection couldn't be re-established
	}
	
	char *qry, *cleanUser, *cleanPassword;
	
	cleanUser = (char *) calloc((2 * strlen(username)) + 1, sizeof(char));
	cleanPassword = (char *) calloc((2 * strlen(password)) + 1, sizeof(char));
	
	mysql_real_escape_string(&mysql, cleanUser, username, strlen(username));
	mysql_real_escape_string(&mysql, cleanPassword, password, strlen(password));
	
	char *q1 = "INSERT INTO users (username, password) VALUES (\'";
	char *q2 = "\', \'";
	char *q3 = "\')";
	
	int size = 1;
	size += strlen(q1) + strlen(q2) + strlen(q3);
	size += strlen(cleanUser) + strlen(cleanPassword);
	
	qry = (char *) calloc(size, sizeof(char));
	
	strcpy(qry, q1); strcat(qry, cleanUser); strcat(qry, q2);
	strcat(qry, cleanPassword); strcat(qry, q3);
	
	free(cleanUser); free(cleanPassword);
	
	// issue the query to the database
	if(mysql_query(&mysql, qry) != 0){
		// attempts to fetch and output the error message
		// from mysql out to STDERR
		cerr << mysql_error(&mysql) << endl;
		free(qry); //deallocate query string
		return false; //error
	}
	
	free(qry);
	
	if(mysql_store_result(&mysql) == 0 &&
	 mysql_field_count(&mysql) == 0 &&
	 (unsigned long) mysql_affected_rows(&mysql) == 1 &&
	 mysql_insert_id(&mysql) != 0){
		return true;
	}
	
	return false;
}

// inserts a new missing person record into the public table, if there's room (<= 100 entries)
// returns id of new entry, negative number on failure
long createMissing(const char *first, const char *last, const char *location){
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return -7; //connection couldn't be re-established
	}
	
	// fetch the number of rows in the public table
	int count = getRowCountPublic();

	// first off, check to make sure we haven't hit our row limit yet
	// also ensures that the RowCount function doesn't fail
	if ((count >= 0) && (count < 100)){
		// construct the string that represents the INSERT query
		char *qry;
		char *qIns = "INSERT INTO public (firstName, lastName, location) VALUES (\'";
		char *qComma = "\', \'";
		char *qEnd = "\')";
		
		// allocate temp strings, big enough to be sanitized (2 * length + 1)
		char *cleanFirst = (char *) calloc(2 * strlen(first) + 1, sizeof(char));
		char *cleanLast = (char *) calloc(2 * strlen(last) + 1, sizeof(char));
		char *cleanLocation = (char *) calloc(2 * strlen(location) + 1, sizeof(char));;
		
		//sanitize strings
		mysql_real_escape_string(&mysql, cleanFirst, first, strlen(first));
		mysql_real_escape_string(&mysql, cleanLast, last, strlen(last));
		mysql_real_escape_string(&mysql, cleanLocation, location, strlen(location));
		
		//dynamically allocate final string
		int size = 1;
		size += strlen(qIns) + strlen(qEnd);
		size += (2 * strlen(qComma));
		size += strlen(cleanFirst) + strlen(cleanLast) + strlen(cleanLocation);
		qry = (char *) calloc(size, sizeof(char));
		
		// piece it together
		strcpy(qry, qIns); strcat(qry, cleanFirst); strcat(qry, qComma);
		strcat(qry, cleanLast); strcat(qry, qComma); strcat(qry, cleanLocation);
		strcat(qry, qEnd);
		
		//free the temp strings
		free(cleanFirst); free(cleanLast); free(cleanLocation);

		// issue the query to the database
		if(mysql_query(&mysql, qry) != 0){
			// attempts to fetch and output the error message
			// from mysql out to STDERR
			cerr << mysql_error(&mysql) << endl;
			free(qry); //deallocate query string
			return -1; //error
		}
		
		free(qry); //deallocate query string
		
		// get the last auto-increment value generated by the server
		if(mysql_store_result(&mysql) == 0 &&
		mysql_field_count(&mysql) == 0 &&
		(long) mysql_affected_rows(&mysql) == 1 &&
		mysql_insert_id(&mysql) != 0){
			return (long) mysql_insert_id(&mysql);
		}
		
	}
	return -2; //error
}

bool createMissingAdmin(const char *id, const char *location, const char *firstName, const char *lastName){
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return false; //connection couldn't be re-established
	}
	
	// fetch the number of rows in the admin table
	int count = getRowCountAdmin();

	// first off, check to make sure we haven't hit our row limit yet
	// also ensures that the RowCount function doesn't fail
	if ((count >= 0) && (count < 100)){
		
		char *qry, *cleanId, *cleanLocation, *cleanFirst, *cleanLast;
		char *qIns = "INSERT INTO admin (personID, firstName, lastName, location) VALUES (\'";
		char *qInsShort = "INSERT INTO admin (personID, location) VALUES (\'";
		char *qComma = "\', \'";
		char *qEnd = "\')";
		
		cleanId = (char *) calloc((2 * strlen(id)) + 1, sizeof(char));
		cleanLocation = (char *) calloc((2 * strlen(location)) + 1, sizeof(char));
		
		mysql_real_escape_string(&mysql, cleanId, id, strlen(id));
		mysql_real_escape_string(&mysql, cleanLocation, location, strlen(location));
		
		int size = 1;
		size += strlen(qComma) + strlen(qEnd);
		size += strlen(cleanId) + strlen(cleanLocation);
		
		//determine if the user has supplied the optional parameters or not
		if((firstName == NULL) && (lastName == NULL)){ //just id & location
			size += strlen(qInsShort);
			qry = (char *) calloc(size, sizeof(char));
			
			strcpy(qry, qInsShort); strcat(qry, cleanId);
			strcat(qry, qComma); strcat(qry, cleanLocation);
			strcat(qry, qEnd);
		} else{ //all four params
			//sanitize the other two
			cleanFirst = (char *) calloc((2 * strlen(firstName)) + 1, sizeof(char));
			cleanLast = (char *) calloc((2 * strlen(lastName)) + 1, sizeof(char));
			
			mysql_real_escape_string(&mysql, cleanFirst, firstName, strlen(firstName));
			mysql_real_escape_string(&mysql, cleanLast, lastName, strlen(lastName));
			
			size += strlen(qIns);
			size += (2 * strlen(qComma));
			size += strlen(cleanFirst) + strlen(cleanLast);
			
			qry = (char *) calloc(size, sizeof(char));
			
			strcpy(qry, qIns); strcat(qry, cleanId);
			strcat(qry, qComma); strcat(qry, cleanFirst);
			strcat(qry, qComma); strcat(qry, cleanLast);
			strcat(qry, qComma); strcat(qry, cleanLocation);
			strcat(qry, qEnd);
			
			free(cleanFirst); free(cleanLast);
		}
		
		//deallocate the "guaranteed" temp strings
		free(cleanId); free(cleanLocation);
		
		// issue the query to the database
		if(mysql_query(&mysql, qry) != 0){
			// attempts to fetch and output the error message
			// from mysql out to STDERR
			cerr << mysql_error(&mysql) << endl;
			free(qry); //deallocate query string
			return false; //error
		}
		
		free(qry);
		
		if(mysql_store_result(&mysql) == 0 &&
			mysql_field_count(&mysql) == 0 &&
			(unsigned long) mysql_affected_rows(&mysql) == 1 &&
			mysql_insert_id(&mysql) != 0){
				return true;
			}
	}
	return false;
}

// returns the alphabetically sorted list of locations with unidentified bodies
// just needs to be sent back to the client
char *locationsWithMissing(){
	
	char *retval;
	bool firstPass = true;
	
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return "DB connection error!"; //connection couldn't be re-established
	}
	
	// final query string, no user input required
	char *qry ="SELECT DISTINCT location FROM admin a WHERE a.firstName IS NULL AND a.lastName IS NULL ORDER BY a.location ASC";
	
	// issue the query statement to the server
	if(mysql_query(&mysql, qry) != 0){
		return "Error with query!";
	}
	
	// if that worked, fetch and store the result set
	MYSQL_RES *res = mysql_store_result(&mysql);

	MYSQL_ROW rw;
	if(mysql_num_fields(res) > 0){
		while((rw = mysql_fetch_row(res)) != NULL){
			if(firstPass){
				retval = (char *) calloc(strlen(rw[0]), sizeof(char));
				strcpy(retval, rw[0]);
				firstPass = false;
			} else{
				retval = (char *) realloc(retval, (strlen(retval) + strlen(rw[0]) + 2) * sizeof(char));
				strcat(retval, "\n");
				strcat(retval, rw[0]);
			}
		}
	}
	mysql_free_result(res);
	if(strlen(retval) <= 8){
		return "There are no locations with unidentified bodies!";
	}
	return retval;
}

bool changePassword(const char *username, const char *oldPass, const char *newPass){
	//first make sure that the user has provided the correct old password
	if(!authenticateUser(username, oldPass)){
		return false;
	}
	
	char *qry, *cleanUser, *cleanPassword;
	
	cleanUser = (char *) calloc((2 * strlen(username)) + 1, sizeof(char));
	cleanPassword = (char *) calloc((2 * strlen(newPass)) + 1, sizeof(char));
	
	mysql_real_escape_string(&mysql, cleanUser, username, strlen(username));
	mysql_real_escape_string(&mysql, cleanPassword, newPass, strlen(newPass));
	
	char *q1 = "UPDATE users SET password=\'";
	char *q2 = "\' WHERE username=\'";
	char *q3 = "\'";
	
	int size = 1;
	size += strlen(q1) + strlen(q2) + strlen(q3);
	size += strlen(cleanUser) + strlen(cleanPassword);
	
	qry = (char *) calloc(size, sizeof(char));
	
	strcpy(qry, q1); strcat(qry, cleanPassword);
	strcat(qry, q2); strcat(qry, cleanUser); strcat(qry, q3);
	
	// deallocate temp strings
	free(cleanUser);
	free(cleanPassword);

	// issue the query statement to the server
	if(mysql_query(&mysql, qry) != 0){
		free(qry);
		return false;
	}
	
	free(qry);
	
	if(mysql_store_result(&mysql) == 0 &&
	 mysql_field_count(&mysql) == 0 &&
	 (unsigned long) mysql_affected_rows(&mysql) == 1){
		return true;
	}
	
	return false;
}

// authenticates admin credentials provided by user
// returns true on success
bool authenticateUser(const char *username, const char *password){
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return -7; //connection couldn't be re-established
	}
	
	// construct query string
	char *qry, *cleanUsername, *cleanPassword;
	char *qStart = "SELECT COUNT(*) FROM users u WHERE u.username=\'";
	char *qMid = "\' AND u.password=\'";
	char *qEnd = "\'";
	
	// allocate enough room for strings to be sanitized
	cleanUsername = (char *) calloc((strlen(username) * 2) + 1, sizeof(char));
	cleanPassword = (char *) calloc((strlen(password) * 2) + 1, sizeof(char));
	
	//sanitize strings
	mysql_real_escape_string(&mysql, cleanUsername, username, strlen(username));
	mysql_real_escape_string(&mysql, cleanPassword, password, strlen(password));
	
	//dynamically allocate final query string
	int size = 1;
	size += strlen(qStart) + strlen(qMid) + strlen(qEnd);
	size += strlen(cleanUsername) + strlen(cleanPassword);
	qry = (char *) calloc(size, sizeof(char));
	
	// piece it all together
	strcpy(qry, qStart);
	strcat(qry, cleanUsername); strcat(qry, qMid);
	strcat(qry, cleanPassword); strcat(qry, qEnd);
	
	// deallocate temp strings
	free(cleanUsername);
	free(cleanPassword);

	// issue the query statement to the server
	if(mysql_query(&mysql, qry) != 0){
		free(qry);
		return false;
	}
	
	free(qry);

	// if that worked, fetch and store the result set
	MYSQL_RES *res = mysql_store_result(&mysql);

	// get the first (and what should be the only) row
	// from this result set
	MYSQL_ROW rw = mysql_fetch_row(res);

	// make sure rw actually points to a row object
	if(rw != NULL){
		// checks that there is at least one column
		// in this row
		if(mysql_num_fields(res) > 0){
			// makes sure that there is one and only
			// record in the database that matches the
			// given set of credentials
			if (atoi(rw[0]) == 1){
				// we're done here, free the results
				mysql_free_result(res);
				return true; //success!
			}
		}
	}
	mysql_free_result(res);
	return false; //error
}

// searches the admin table to see if the person with
// the given name has been positively ID'd
bool queryAdmin(const char *first, const char *last){
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return -7; //connection couldn't be re-established
	}
	
	// construct query string
	char *qry, *cleanFirst, *cleanLast;
	char *qStart = "SELECT COUNT(*) FROM admin a WHERE a.firstName=\'";
	char *qMid = "\' AND a.lastName=\'";
	char *qEnd = "\'";
	
	// allocate enough room for strings to be sanitized
	cleanFirst = (char *) calloc((strlen(first) * 2) + 1, sizeof(char));
	cleanLast = (char *) calloc((strlen(last) * 2) + 1, sizeof(char));
	
	//sanitize strings
	mysql_real_escape_string(&mysql, cleanFirst, first, strlen(first));
	mysql_real_escape_string(&mysql, cleanLast, last, strlen(last));
	
	//dynamically allocate final query string
	int size = 1;
	size += strlen(qStart) + strlen(qMid) + strlen(qEnd);
	size += strlen(cleanFirst) + strlen(cleanLast);
	qry = (char *) calloc(size, sizeof(char));
	
	// piece it all together
	strcpy(qry, qStart);
	strcat(qry, cleanFirst); strcat(qry, qMid);
	strcat(qry, cleanLast); strcat(qry, qEnd);
	
	// deallocate temp strings
	free(cleanFirst);
	free(cleanLast);

	// issue the query statement to the server
	if(mysql_query(&mysql, qry) != 0){
		free(qry);
		return false;
	}
	
	free(qry);

	// if that worked, fetch and store the result set
	MYSQL_RES *res = mysql_store_result(&mysql);

	// get the first (and what should be the only) row
	// from this result set
	MYSQL_ROW rw = mysql_fetch_row(res);

	// make sure rw actually points to a row object
	if(rw != NULL){
		// checks that there is at least one column
		// in this row
		if(mysql_num_fields(res) > 0){
			// makes sure that there is one and only
			// record in the database that matches the
			// given name
			if (atoi(rw[0]) > 0){
				// we're done here, free the results
				mysql_free_result(res);
				return true; //success!
			}
		}
	}
	mysql_free_result(res);
	return false; //error
}

char *getPhotoPublic(const char *id, unsigned long *fsize){
	return getPhoto(id, fsize, "SELECT photo FROM public WHERE id =\'");
}

char *getPhotoAdmin(const char *id, unsigned long *fsize){
	return getPhoto(id, fsize, "SELECT photo FROM admin WHERE personID =\'");
}

char *getPhoto(const char *id, unsigned long *fsize, const char *q1){
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return "Error"; //connection couldn't be re-established
	}
	
	char *qry, *cleanID;
	char *q2 = "\'";
	
	cleanID = (char *) calloc((2 * strlen(id)) + 1, sizeof(char));
	mysql_real_escape_string(&mysql, cleanID, id, strlen(id));
	
	int size = 1;
	size += strlen(q1) + strlen(cleanID) + strlen(q2);
	
	qry = (char *) calloc(size, sizeof(char));
	
	strcpy(qry, q1); strcat(qry, cleanID); strcat(qry, q2);
	
	free(cleanID);
	
	// issue the query statement to the server
	if(mysql_query(&mysql, qry) != 0){
		free(qry);
		cerr << mysql_error(&mysql) << endl;
		return false;
	}
	
	free(qry);
	
	MYSQL_RES *result = mysql_store_result(&mysql);
	MYSQL_ROW row = mysql_fetch_row(result);
	unsigned long *lengths = mysql_fetch_lengths(result);
	
	mysql_free_result(result);
	
	*fsize = lengths[0];
	return row[0];
	
	return "error";
}

bool addPhotoPublic(const char *id, char *file, long fileSize){
	return addPhoto(id, file, fileSize, "UPDATE public SET photo=\'", "\' WHERE id=\'");
}

bool addPhotoAdmin(const char *id, char *file, long fileSize){
	return addPhoto(id, file, fileSize, "UPDATE admin SET photo=\'", "\' WHERE personID=\'");
}

bool addPhoto(const char *id, char *file, long fileSize, const char *q1, const char *q2){
	// ensures the connection to the server is still active, attempts to
	// reconnect if its not
	if(mysql_ping(&mysql)){
		return false; //connection couldn't be re-established
	}
	
	char *qry, *cleanId, *cleanImg;
	
	char *q3 = "\'";
	
	cleanId = (char *) calloc((2 * strlen(id)) + 1, sizeof(char));
	cleanImg = (char *) calloc((2 * fileSize) + 1, sizeof(char));
	
	mysql_real_escape_string(&mysql, cleanImg, file, fileSize);
	mysql_real_escape_string(&mysql, cleanId, id, strlen(id));
	
	int size = 1;
	size += strlen(q1) + strlen(q2) + strlen(q3);
	size += strlen(cleanId) + strlen(cleanImg);
	
	qry = (char *) calloc(size, sizeof(char));
	
	strcpy(qry, q1); strcat(qry, cleanImg); strcat(qry, q2);
	strcat(qry, cleanId); strcat(qry, q3);
	
	free(cleanId); free(cleanImg);
	
	// issue the query statement to the server
	if(mysql_real_query(&mysql, qry, strlen(qry)) != 0){
		free(qry);
		cerr << mysql_error(&mysql) << endl;
		return false;
	}
	
	free(qry);
	
	if(mysql_store_result(&mysql) == 0 &&
	 mysql_field_count(&mysql) == 0 &&
	 (unsigned long) mysql_affected_rows(&mysql) == 1){
		return true;
	 } else{
		 //just a duplicate photo
		 return true;
	 }
	return false;
}
