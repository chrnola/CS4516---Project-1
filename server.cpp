//server.C

#include <iostream>
#include "did.h"

using namespace std;

int main(){
	cout << "Connecting to database" << endl;
	bool connected = connectToDB();
	if (connected){
		cout << "Connected!" << endl;
		//int rws = getRowCountPublic();
		//createMissing("Bob", "Smith", "Worcester, MA");
	} else{
		cout << "Connection error!" << endl;
	}
	disconnectFromDB();
}
