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
	cout << PhysicalLayer::FrameValid(s, 9) << endl;
	strcpy(s, "hulxkj");
	s = PhysicalLayer::FoldSerializedFrame(s);
	cout << PhysicalLayer::FrameValid(s, 8) << endl;
	*s = 'i';
	cout << PhysicalLayer::FrameValid(s, 8) << endl;
}
