// message.C

#include "message.h"
#include <iostream>

using namespace std;

Message::Message(){
}

void Message::setCmd(char *_cmd){
	cmd = (unsigned char*) calloc(strlen(_cmd), sizeof(unsigned char));
	cmd = reinterpret_cast<unsigned char*>(_cmd);
}

void Message::setContent(char *_content){
	content = (unsigned char*) calloc(strlen(_content), sizeof(unsigned char));
	content = reinterpret_cast<unsigned char*>(_content);
}

char *Message::getCmd(){
	return reinterpret_cast<char*>(cmd);
}

char *Message::getContent(){
	return reinterpret_cast<char*>(content);
}

unsigned char *Message::getCmdRaw(){
	return cmd;
}

unsigned char *Message::getContentRaw(){
	return content;
}

Message::~Message(){
}
