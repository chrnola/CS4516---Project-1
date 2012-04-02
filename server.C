//server.C

#include <iostream>
#include "did.h"

using namespace std;

int main(){
	cout << "Connecting to database" << endl;
	bool connected = connectToDB();
	if (connected){
		cout << "Connected!" << endl;
		cout << "Looking up number of public rows..." << endl;
		int rws = getRowCountPublic();
		cout << "Result: " << rws << endl;
	} else{
		cout << "Connection error!" << endl;
	}
	disconnectFromDB();
}
