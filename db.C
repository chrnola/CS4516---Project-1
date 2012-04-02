// db.C
// Used for server side interactions with the database

#include "did.h"
#include <mysql.h>
#include <string>
#include <stdlib.h> //for atoi()

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
		mysql_free_result(res);
		if(mysql_num_fields(res) > 0){
			return atoi(rw[0]);
		} else{
			return -2;
		}
	}
	mysql_free_result(res);
	return -3;
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
		mysql_free_result(res);
		if(mysql_num_fields(rw) > 0){
			if (atoi(rw[0]) > 0){
				return true;
			}
		} else{
			return false;
		}
	}
	mysql_free_result(res);
	return false;
}

