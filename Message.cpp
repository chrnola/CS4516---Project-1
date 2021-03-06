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
	return reinterpret_cast<char*>(this->cmd);
}

//Same thing, but for content
char *Message::getContent(){
	return reinterpret_cast<char*>(this->content);
}

//In the event that you would want the actual unsigned value of the cmd field
unsigned char *Message::getCmdRaw(){
	return this->cmd;
}

//Same thing, but for content
unsigned char *Message::getContentRaw(){
	return this->content;
}

// make a Message into a character string
unsigned char *Message::serialize(long size){
	unsigned char *data = (unsigned char*) calloc(size, sizeof(unsigned char));
	short cmdLen = strlen(this->getCmd());
	long contentLength = this->getContentSize();
	
	memcpy(data, Utils::ltoa(contentLength), 17); //17=long
	memcpy(data + 17, Utils::itoa(cmdLen), 5); //5=short
	memcpy(data + MESSAGE_HEAD, this->getCmd(), cmdLen);
	memcpy(data + MESSAGE_HEAD + cmdLen, this->getContent(), contentLength);
	
	return data;
}

// make a Message out of a previously serialized Message
Message* Message::unserialize(unsigned char *pkt){
	Message *m = new Message();
	string str(reinterpret_cast<char*>(pkt));
	long size = atol(str.substr(1,17).c_str());
	short strSize = atoi(str.substr(18,5).c_str());
	char *cmd = (char *)str.substr(22,strSize).c_str();
	m->setCmd(cmd);
	char *img = (char *)str.substr(22+strSize, size).c_str();
	m->setImg(img, size);
	
	return m;
}

void Message::Print(){
	cout<<"Message:"<<endl;
	cout<<"Command: "<<getCmd()<<endl;
	cout<<"Content Size: "<<getContentSize()<<endl;
	if(getContentSize() > 0) cout<<"Content: "<<getContent()<<endl;
}

Message::~Message(){
}
