/*
 * client.C
 */

#include "did.h"
#include <iostream>

using namespace std;

void testPL();

int main() {
	// cout << "Calling client init code..." << endl;
	// startClient();
	testPL();

}

void testPL(){
	PhysicalLayer pl = PhysicalLayer("localhost");
}
