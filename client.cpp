/*
 * client.C
 */

#include "did.h"
#include <unistd.h>

using namespace std;

NetworkLayer *nl;
DataLink* dl;
PhysicalLayer* pl;

queue<Packet*> sendPackets, recvPackets;
queue<Frame*> sendFrames, recvFrames;
pthread_mutex_t mutSP, mutRP, mutSF, mutRF;

void startPrompt();
bool checkLength(char *field, int max);
void quit();

void testPL();

struct timeval* start, * end;

int main(int argc, char **argv) {
	//testPL();
	gettimeofday(start, NULL);

	
	// init lower leves, spawn threads, etc
	// once connected on well known port...	
	nl = new NetworkLayer();
	//dl = new DataLink();
	//pl = new PhysicalLayer();
	
	pthread_mutex_init(&mutSP, NULL);
	pthread_mutex_init(&mutRP, NULL);
	pthread_mutex_init(&mutSF, NULL);
	pthread_mutex_init(&mutRF, NULL);

	
	startPrompt();

}

void quit(){
	//close connection, disconnet
	cout << endl;
	gettimeofday(end, NULL);
	unsigned long secs = end->tv_sec - start->tv_sec;
	unsigned long usecs = end->tv_usec - start->tv_sec;
	unsigned int mins = secs / 60;
	cout << "Session ended. Time taken: ";
	if(mins > 0) {
		cout << mins << " minute(s), ";
	}
	cout << secs << " seconds and " << usecs << " microseconds.\n";
	exit(0);
}

void startPrompt(){
	char input[128];
	char *tmp;
	char *argvNew[8];
	int i = 1;
	
	cout << "did>";
	cin.getline(input, 128);
	
	if (cin.eof() != 0){
		quit();
	}
	
	if(strlen(input) == 0){
		startPrompt();
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
		
		argvNew[i] = NULL;
		
		if(strcmp(argvNew[0], "quit") == 0){
			cout << "Bye!" << endl;
			quit();
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
							strlen(argvNew[2]) + strlen(argvNew[3]) + 3;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]); strcat(cmd, " ");
					strcat(cmd, argvNew[2]); strcat(cmd, " ");
					strcat(cmd, argvNew[3]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else{
				cout << "Error: createmissing expects firstName lastName lastKnownLocation" << endl;
			}
		} else if(strcmp(argvNew[0], "login") == 0){
			if((argvNew[1] != NULL) && (argvNew[2] != NULL)){
				if(checkLength(argvNew[1], MAX_USER) && checkLength(argvNew[2], MAX_USER)){
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 
							strlen(argvNew[2]) + 2;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]); strcat(cmd, " ");
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
							strlen(argvNew[2]) + 2;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]); strcat(cmd, " ");
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
							strlen(argvNew[2]) + 2;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]); strcat(cmd, " ");
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
							strlen(argvNew[2]) + strlen(argvNew[3]) +3;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]); strcat(cmd, " ");
					strcat(cmd, argvNew[2]); strcat(cmd, " ");
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
							strlen(argvNew[2]) + 2;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]); strcat(cmd, " ");
					strcat(cmd, argvNew[2]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else{
				cout << "Error: password expects oldPassword newPassword" << endl;
			}
		} else if(strcmp(argvNew[0], "removemissing") == 0){
			if(argvNew[1] != NULL){
				if(checkLength(argvNew[1], MAX_ID)) {
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 1;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else{
				cout << "Error: removemissing expects id" << endl;
			}
		} else if(strcmp(argvNew[0], "removebody") == 0){
			if(argvNew[1] != NULL){
				if(checkLength(argvNew[1], MAX_ID)) {
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 1;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else{
				cout << "Error: removebody expects id" << endl;
			}
		} else if(strcmp(argvNew[0], "createbody") == 0){
			if((argvNew[1] != NULL) && (argvNew[2] != NULL) &&
			(argvNew[3] != NULL) && (argvNew[4] != NULL)){
				if(checkLength(argvNew[1], MAX_ID) && checkLength(argvNew[2], MAX_LOCATION) && 
				 checkLength(argvNew[3], MAX_FIRST) && checkLength(argvNew[4], MAX_LAST)) {
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 
							strlen(argvNew[2]) + strlen(argvNew[3]) + strlen(argvNew[4]) + 4;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]); strcat(cmd, " ");
					strcat(cmd, argvNew[2]); strcat(cmd, " ");
					strcat(cmd, argvNew[3]); strcat(cmd, " ");
					strcat(cmd, argvNew[4]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else if((argvNew[1] != NULL) && (argvNew[2] != NULL)){
				if(checkLength(argvNew[1], MAX_ID) && checkLength(argvNew[2], MAX_LOCATION)) {
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 
							strlen(argvNew[2]) + 2;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]); strcat(cmd, " ");
					strcat(cmd, argvNew[2]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
				}
			} else{
				cout << "Error: createbody expects id location [firstName lastName]" << endl;
			}
		} else if((strcmp(argvNew[0], "addphoto") == 0) || (strcmp(argvNew[0], "addbodyphoto") == 0)){
			if((argvNew[1] != NULL) && (argvNew[2] != NULL)){
				if(checkLength(argvNew[1], MAX_ID)){
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 1;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]);
					m->setCmd(cmd);
					
					ifstream pic;
					pic.open(argvNew[2], ios::in | ios::binary | ios::ate);
	
					ifstream::pos_type newsize;
					char *img;
	
					if(pic.is_open()){
						newsize = pic.tellg();
						img = new char[newsize];
						pic.seekg(0, ios::beg);
						pic.read(img, size);
						pic.close();
						m->setImg(img, (long)size);
						delete[] img;
						nl -> FromApplicationLayer(m);
						//TODO: wait for response, display it
					} else{
						cout << "Unable to load file!" << endl;
					}
				}
			} else{
				if(strcmp(argvNew[0], "addphoto") == 0){
					cout << "Error: addphoto expects id filename" << endl;
				} else{
					cout << "Error: addbodyphoto expects id filename" << endl;
				}
			}
		} else if(strcmp(argvNew[0], "dlmissingphoto") == 0){
			if((argvNew[1] != NULL) && (argvNew[2] != NULL)){
				if(checkLength(argvNew[1], MAX_ID)) {
					Message *m = new Message();
					int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 1;
					char *cmd = (char *) calloc(size, sizeof(char));
					strcpy(cmd, argvNew[0]); strcat(cmd, " ");
					strcat(cmd, argvNew[1]);
					m->setCmd(cmd);
					nl -> FromApplicationLayer(m);
					//TODO: wait for response, display it
					Message *resp = new Message(); //fromLower
					long newSize = resp->getContentSize();
					char *newImg = (char *) calloc(newSize, sizeof(char));
					memcpy(newImg, resp->getContent(), newSize);
					
					ofstream newPic;
					newPic.open(argvNew[2], ios::out|ios::binary);
					if(newPic.is_open()){
						newPic.write(newImg, newSize);
						newPic.close();
						cout << "File saved to " << argvNew[2] << endl;
					} else{
						cout << "Error writing file!" << endl;
					}
				}
			} else{
				cout << "Error: dlmissingphoto expects id" << endl;
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
