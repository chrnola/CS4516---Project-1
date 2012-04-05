/*
 *  aaron.cpp
 *
 *  Created on: Apr 5, 2012
 *      Author: Aaron Sikes
 */


#include <iostream>
#include "PhysicalLayer.h"

using namespace std;

int main(){
	char* s = (char*) malloc(8);
	strcpy(s, "abcdefg");
	s = PhysicalLayer::FoldSerializedFrame(s);
	cout << "FoldByteA: ";
	cout << (int) (s[7]) << endl;
	cout << "FoldByteB: " << (int) (s[8]) << endl;
}
