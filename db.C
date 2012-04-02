// db.C
// Used for server side interactions with the database

#include "did.h"
#include <mysql.h>
#include <string>
#include <stdlib.h> //for atoi()
#include <iostream>

using namespace std;

int getRowCountTbl(const char*);

MYSQL mysql;

bool connectToDB(){
	if(mysql_library_init(0, NULL, NULL)){
		return false;
	}
	mysql_init(&mysql);
 	if(!mysql_real_connect(&mysql, "mysql.wpi.edu", "db_user", 
			"zvRPqu", "cs4516_d12_t6", 0, NULL, 0)){
		return false;
	}
	return true;
}

void disconnectFromDB(){
	mysql_close(&mysql);
	mysql_library_end();
}

int getRowCountPublic(){
	return getRowCountTbl("public");
}

int getRowCountAdmin(){
	return getRowCountTbl("admin");
}

int getRowCountTbl(const char* tbl){
	char qry[40];
	strcpy(qry, "SELECT COUNT(*) FROM ");
	strcat(qry, tbl);
	if(mysql_query(&mysql, qry) != 0){
		return -1;
	}
	MYSQL_RES *res = mysql_store_result(&mysql);
	MYSQL_ROW rw = mysql_fetch_row(res);
	if (rw != NULL){
		if(mysql_num_fields(res) > 0){
			mysql_free_result(res);
			return atoi(rw[0]);
		} else{
			mysql_free_result(res);
			return -2;
		}
	}
	mysql_free_result(res);
	return -3;
}

bool createMissing(const char *first, const char *last, const char *location){
	int count = getRowCountPublic();
	if ((count >= 0) && (count < 100)){
		char qry[350];
		strcpy(qry, "INSERT INTO public (firstName, lastName, location) ");
		strcat(qry, "VALUES (\'");
		strcat(qry, first);
		strcat(qry, "\', \'");
		strcat(qry, last);
		strcat(qry, "\', \'");
		strcat(qry, location);
		strcat(qry, "\')");
		if(mysql_query(&mysql, qry) != 0){
			cerr << mysql_error(&mysql) << endl;
			return false;
		}
		cout << "Affected rows: ";
		cout << (long)mysql_affected_rows(&mysql) << endl;
		return true;
	}
	return false;
}

bool authenticateUser(const char *username, const char *password){
	char qry[100];
	strcpy(qry, "SELECT COUNT(*) FROM users u WHERE u.username=\'");
	strcat(qry, username);
	strcat(qry, "\' AND u.password=\'");
	strcat(qry, password);
	strcat(qry, "\'");
	if(mysql_query(&mysql, qry) != 0){
		return false;
	}
	MYSQL_RES *res = mysql_store_result(&mysql);
	MYSQL_ROW rw = mysql_fetch_row(res);
	if(rw != NULL){
		if(mysql_num_fields(res) > 0){
			if (atoi(rw[0]) > 0){
				mysql_free_result(res);
				return true;
			}
		} else{
			return false;
		}
	}
	mysql_free_result(res);
	return false;
}

