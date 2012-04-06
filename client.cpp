/*
 * client.C
 */

#include "did.h"
#include <unistd.h>

using namespace std;

NetworkLayer *nl;
DataLink* dl;
PhysicalLayer* pl;

Packet** sendPackets, recvPackets;
Frame** sendFrames, recvFrames;
char sPNum, sPCur, rPNum, rPCur, sFNum, sFCur, rFNum, rFCur;

void startPrompt();
bool checkLength(char *field, int max);

void testPL();


int main(int argc, char **argv) {
	//testPL();
	
	// init lower leves, spawn threads, etc
	// once connected on well known port...	
	nl = new NetworkLayer();
	dl = new DataLink();
	pl = new PhysicalLayer();
	
	sendPackets = (Packet**) calloc(MAX_SEND_PACKET, sizeof(Packet*));
	recvPackets = (Packet**) calloc(MAX_RECV_PACKET, sizeof(Packet*));
	sendFrames = (Frame**) calloc(MAX_SEND_FRAME, sizeof(Frame*));
	recvFrames = (Frame**) calloc(MAX_RECV_FRAME, sizeof(Frame*));
	sPNum = 0; sPCur = 0;
	rPNum = 0; rPCur = 0;
	sFNum = 0; sFCur = 0;
	rFNum = 0; rFCur = 0;
	
	startPrompt();

}

void startPrompt(){
	char input[128];
	char *tmp;
	char *argvNew[8];
	int i = 1;
	
	cout << "did>";
	cin.getline(input, 128);
	
	if (cin.eof() != 0){
		//closeAndDisconnect()
		cout << endl;
		exit(-1);
	}
	
	if((tmp = strtok(input, " ")) != NULL){
		argvNew[0] = tmp;
		
		while ((tmp = strtok(NULL, " ")) != NULL){
			if(i < 7){
				argvNew[i] = tmp;
				i++;
			} else{
				cerr << "Way too many params, stop that. I'm gunna ignore those extra ones for now." << endl;
				break;
			}
		}
		
		if(i < 8){
			argvNew[i] = NULL;
		} else {
			cerr << "This will never fire" << endl;
		}
		
		if(strcmp(argvNew[0], "quit") == 0){
			cout << "Bye!" << endl;
			//disconnect
			exit(0);
		} else if(strcmp(argvNew[0], "locations") == 0){
			Message *m = new Message();
			m->setCmd("locations");
			nl -> FromApplicationLayer(m);
			//TODO: wait for response, display it
			
		} else if(strcmp(argvNew[0], "createmissing") == 0){
			if((argvNew[1] != NULL) && 
			 (argvNew[2] != NULL) &&
			 (argvNew[3] != NULL)){
				if(checkLength(argvNew[1], MAX_FIRST) && checkLength(argvNew[2], MAX_LAST) && checkLength(argvNew[3], MAX_LOCATION)){
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 
							strlen(argvNew[2]) + strlen(argvNew[3]);
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]);
					strcat(cmd, argvNew[1]);
					strcat(cmd, argvNew[2]);
					strcat(cmd, argvNew[3]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else{
				cout << "Error: createmissing expects firstName lastName lastKnownLocation" << endl;
			}
		} else if(strcmp(argvNew[0], "login") == 0){
			if((argvNew[1] != NULL) && 
			 (argvNew[2] != NULL)){
				if(checkLength(argvNew[1], MAX_USER) && checkLength(argvNew[2], MAX_USER)){
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 
							strlen(argvNew[2]);
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]);
					strcat(cmd, argvNew[1]);
					strcat(cmd, argvNew[2]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else{
				cout << "Error: login expects username password" << endl;
			}
		} else if(strcmp(argvNew[0], "query") == 0){
			if((argvNew[1] != NULL) && 
			 (argvNew[2] != NULL)){
				if(checkLength(argvNew[1], MAX_FIRST) && checkLength(argvNew[2], MAX_LAST)) {
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 
							strlen(argvNew[2]);
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]);
					strcat(cmd, argvNew[1]);
					strcat(cmd, argvNew[2]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else{
				cout << "Error: query expects firstName lastName" << endl;
			}
		} else if(strcmp(argvNew[0], "adduser") == 0){
			if((argvNew[1] != NULL) && 
			 (argvNew[2] != NULL)){
				if(checkLength(argvNew[1], MAX_USER) && checkLength(argvNew[2], MAX_USER)) {
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 
							strlen(argvNew[2]);
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]);
					strcat(cmd, argvNew[1]);
					strcat(cmd, argvNew[2]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			 } else{
				cout << "Error: adduser expects username password" << endl;
				}
		} else if(strcmp(argvNew[0], "id") == 0){
			if((argvNew[1] != NULL) && 
			 (argvNew[2] != NULL) &&
			 (argvNew[3] != NULL)){
				if(checkLength(argvNew[1], MAX_ID) && checkLength(argvNew[2], MAX_FIRST) && checkLength(argvNew[3], MAX_LAST)) {
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 
							strlen(argvNew[2]) + strlen(argvNew[3]);
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]);
					strcat(cmd, argvNew[1]);
					strcat(cmd, argvNew[2]);
					strcat(cmd, argvNew[3]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else{
				cout << "Error: id expects id firstName lastName" << endl;
			}
		} else if(strcmp(argvNew[0], "password") == 0){
			if((argvNew[1] != NULL) && 
			 (argvNew[2] != NULL)){
				if(checkLength(argvNew[1], MAX_USER) && checkLength(argvNew[2], MAX_USER)) {
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 
							strlen(argvNew[2]);
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]);
					strcat(cmd, argvNew[1]);
					strcat(cmd, argvNew[2]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else{
				cout << "Error: password expects oldPassword newPassword" << endl;
			}
		} else{
			cout << "Bad command!" << endl;
		}
		
		startPrompt();
	}
	
}

//returns true if the string is of valid length
bool checkLength(char *field, int max){
	if(strlen(field) > max){
		switch(max){
			case MAX_FIRST:
				cout << "First names must not exceed " << MAX_FIRST << " characters!" << endl;
				break;
			
			case MAX_LAST:
				cout << "Last names, usernames, and passwords must not exceed " << MAX_LAST << " characters!"  << endl;
				break;
				
			case MAX_ID:
				cout << "IDs must not exceed " << MAX_ID << " characters!" << endl;
				break;
				
			case MAX_LOCATION:
				cout << "Locations must not exceed " << MAX_LOCATION << " characters!" << endl;
				
			default:
				cout << "Parameter too long!" << endl;
		}
		return false;
	}
	return true;
}

void testPL(){
	PhysicalLayer pl = PhysicalLayer("localhost");
}
