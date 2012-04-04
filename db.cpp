// db.C
// Used for server side interactions with the database
// Author: Chris Pinola

#include "did.h"
#include <mysql.h>
#include <string>
#include <stdlib.h> //for atoi()
#include <iostream>

using namespace std;

//forward declare this function, not included in header,
// no need to expose it to everyone else
int getRowCountTbl(const char*);

//the global MySQL connection object that will be used for all
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
int getRowCountTbl(const char* tbl){
	// construct the string that represents the query
	char qry[40];
	strcpy(qry, "SELECT COUNT(*) FROM ");
	strcat(qry, tbl);
	
	// issue the query to the server
	if(mysql_query(&mysql, qry) != 0){
		return -1;
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

// inserts a new missing person record into the public table, if there's room (<= 100 entries)
// returns id of new entry, negative number on failure
int createMissing(const char *first, const char *last, const char *location){
	// fetch the number of rows in the public table
	int count = getRowCountPublic();

	// first off, check to make sure we haven't hit our row limit yet
	// also ensures that the RowCount function doesn't fail
	if ((count >= 0) && (count < 100)){
		// construct the string that represents the INSERT query
		char qry[350];
		strcpy(qry, "INSERT INTO public (firstName, lastName, location) ");
		strcat(qry, "VALUES (\'");
		strcat(qry, first);
		strcat(qry, "\', \'");
		strcat(qry, last);
		strcat(qry, "\', \'");
		strcat(qry, location);
		strcat(qry, "\')");

		// issue the query to the database
		if(mysql_query(&mysql, qry) != 0){
			// attempts to fetch and output the error message
			// from mysql out to STDERR
			cerr << mysql_error(&mysql) << endl;
			return -1; //error
		}
		if( (long) mysql_affected_rows(&mysql) == 1){
			//TODO: get new row id instead
			return 0;
		}
	}
	return -2; //error
}

// authenticates admin credentials provided by user
// returns true on success
bool authenticateUser(const char *username, const char *password){
	// build the string that represents the SELECT query
	char qry[100];
	strcpy(qry, "SELECT COUNT(*) FROM users u WHERE u.username=\'");
	strcat(qry, username);
	strcat(qry, "\' AND u.password=\'");
	strcat(qry, password);
	strcat(qry, "\'");

	// issue the query statement to the server
	if(mysql_query(&mysql, qry) != 0){
		return false;
	}

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
		} else{
			return false; //error!
		}
	}
	mysql_free_result(res);
	return false;
}

