// message.C
// Author: Chris Pinola

#include "Message.h"

using namespace std;

Message::Message(){
	this->contentSize = 0;
	setCmd("");
	setCmd("");
}

//Constructor that simply inits both fields from the given parameters
Message::Message(char *_cmd, char *_content){
	this->contentSize = 0;
	setCmd(_cmd);
	setContent(_content);
}

//Sets the Cmd field of this message after converting it to an unsigned char array
void Message::setCmd(char *_cmd){
	cmd = (unsigned char*) calloc(strlen(_cmd), sizeof(unsigned char));
	cmd = reinterpret_cast<unsigned char*>(_cmd);
}

//Same thing, but for the content field
void Message::setContent(char *_content){
	content = (unsigned char*) calloc(strlen(_content), sizeof(unsigned char));
	content = reinterpret_cast<unsigned char*>(_content);
}

void Message::setImg(char *img, long size){
	content = (unsigned char*) calloc(size, sizeof(unsigned char));
	memcpy(content, reinterpret_cast<unsigned char*>(img), size);
	this->contentSize = size;
}

long Message::getContentSize(){
	return this->contentSize;
}

//Returns cmd converted back to a regular char*
char *Message::getCmd(){
	return reinterpret_cast<char*>(cmd);
}

//Same thing, but for content
char *Message::getContent(){
	return reinterpret_cast<char*>(content);
}

//In the event that you would want the actual unsigned value of the cmd field
unsigned char *Message::getCmdRaw(){
	return cmd;
}

//Same thing, but for content
unsigned char *Message::getContentRaw(){
	return content;
}

unsigned char *Message::serialize(){
	unsigned char *data = (unsigned char*) calloc(strlen((char *) this), sizeof(unsigned char));
	strcat((char *) data, (char *) this);
	return data;
}

Message* Message::unserialize(unsigned char *pkt){
	return (Message *) pkt;
}

Message::~Message(){
}
