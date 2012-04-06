/*
 * client.C
 */

#include "did.h"

using namespace std;

void startPrompt();
void testPL();

int main(int argc, char **argv) {
	// testPL();
	
	// init lower leves, spawn threads, etc
	// once connected on well known port...	
	startPrompt();

}

void startPrompt(){
	char input[128];
	cout << "did>";
	cin.getline(input, 128);
	
	if (cin.eof() != 0){
		//closeAndDisconnect()
		cout << endl;
		exit(-1);
	}
	
	if(() != NULL){
		
	}
	
}

void testPL(){
	PhysicalLayer pl = PhysicalLayer("localhost");
}
