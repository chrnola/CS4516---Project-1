// message.h
// Author: Chris Pinola

#ifndef ___MESSAGE_H___
#define ___MESSAGE_H___

#include <string.h>
#include <stdlib.h>

class Message{
	private:
		unsigned char *cmd;
		unsigned char *content;
	public:
		Message();
		Message(char *_cmd, char *_content);
		~Message();
		void setCmd(char *_cmd);
		char *getCmd();
		unsigned char *getCmdRaw();
		void setContent(char *_content);
		char *getContent();
		unsigned char *getContentRaw();
		unsigned char *serialize();
		void Message Unserialize(unsigned char* pkt);
};

#endif
