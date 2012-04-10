// message.h
// Author: Chris Pinola

#ifndef ___MESSAGE_H___
#define ___MESSAGE_H___

#include "did.h"

class Message{
	private:
		unsigned char *content; //binary
		long contentSize;
	public:
		unsigned char *cmd; // only text
		Message();
		Message(char *_cmd, char *_content);
		~Message();
		void setCmd(char *_cmd);
		char *getCmd();
		unsigned char *getCmdRaw();
		void setImg(char *img, long size);
		void setContent(char *_content);
		char *getContent();
		unsigned char *getContentRaw();
		long getContentSize();
		
		unsigned char *serialize(short size);
		static Message* unserialize(unsigned char *pkt);
};

#endif
