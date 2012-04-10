//server.cpp

#include <iostream>
#include <netinet/in.h>		// For sockadd_in
#include <arpa/inet.h>		// For htons, htonl, etc
#include <sys/socket.h>
#include <cstdio>		// For perror()
#include <string.h>		// For memset
#include <unistd.h>		// For close()
#include <pthread.h>

class NetworkLayer;
class Message;

#include "did.h"

using namespace std;

void handleMessage(Message *m);
void testPL();


queue<Packet*> sendPackets, recvPackets;
queue<Frame*> sendFrames, recvFrames;
pthread_mutex_t mutSP, mutRP, mutSF, mutRF;

bool connected = false;
bool auth = false;
char *user;

NetworkLayer *nl;
DataLink* dl;
PhysicalLayer* pl;

char *err = "The server encountered an error while processing your request.";
char *nauth = "You are not authorized to execute this command, please login first.";

int AcceptConn();
void *RunPLThread(void* ptr);
void* RunDLThread(void* ptr);

int main(){

	pthread_mutex_init(&mutSF, NULL);
	pthread_mutex_init(&mutRF, NULL);


	int sockfd = AcceptConn();  /* Parent never leaves this call
	here. Past here, we're a child with a valid, connected socket file descriptor. */

	
	connected = connectToDB(); //true if connected
	nl = new NetworkLayer();
	
	pl = new PhysicalLayer(sockfd);
	pthread_t PL_thread, DL_thread;
	pthread_create(&PL_thread, NULL, RunPLThread, pl);

	dl = new DataLink();
	pthread_create(&DL_thread, NULL, RunDLThread, dl);
	
	while(connected){
		Message *incoming = nl -> FromDataLinkLayer();
		cout << "Got message from the shadow realm" << endl;
		short q = incoming->getContentSize() + strlen((char*)incoming->cmd);
		cout << "$$$$$$$$$$$$$$$ " << q << endl;
		cout << incoming->serialize(q) << endl;
		cout << "End region" << endl;
		fflush(stdout);
		handleMessage(incoming);
	}
	
	disconnectFromDB();
	
	return 0;
}

void *RunPLThread(void* ptr){
	PhysicalLayer *pl = (PhysicalLayer*) ptr;
	pl->run();
	return NULL;
}

void *RunDLThread(void* ptr){
	DataLink* dl = (DataLink*) ptr;
	dl->GoBack1();
	return NULL;
}

/*
 * Author: Chris Pinola
 */
void handleMessage(Message *inm){
	char *cmd = inm->getCmd(); //extract CMD from packet
	char *tmp; // just to hold string tokens
	char *argvNew[8]; // holds onto the various arguments given by the user
	int i = 1; // helps us stay within the array bounds
	Message *m = new Message(); // the message that we'll eventually send back to the client
	
	// run through and split the command string on all spaces
	if((tmp = strtok(cmd, " ")) != NULL){
		argvNew[0] = tmp;
		
		while ((tmp = strtok(NULL, " ")) != NULL){
			if(i < 7){
				argvNew[i] = tmp;
				i++;
			} else{
				break;
			}
		}
		
		argvNew[i] = NULL; //make sure the array is NULL "terminated"
		
		//now the fun part begins
		//each branch corresponds to a different command
		//basically just calls the appropriate database functions
		//and makes fills in our new message object with the
		//response
		if(strcmp(argvNew[0], "locations") == 0){
			cout << "awesome" << endl;
			m->setCmd(locationsWithMissing());
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "quit") == 0){
			connected = false;
		} else if(strcmp(argvNew[0], "createmissing") == 0){
			long newID;
			if((newID = createMissing(argvNew[1], argvNew[2], argvNew[3])) <= 0){
				m->setCmd(err);
			} else{
				m->setCmd((char *) newID);
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "query") == 0){
			if(queryAdmin(argvNew[1], argvNew[2])){
				m->setCmd("That person has been found!");
			} else{
				m->setCmd("That person has not been found");
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "login") == 0){
			if(auth){
				//already signed in
				m->setCmd("You're already signed in");
			} else{
				if(authenticateUser(argvNew[1], argvNew[2])){
					auth = true;
					user = argvNew[1];
					m->setCmd("You have successfully signed in!");
				} else{
					m->setCmd("Invalid credentials");
				}
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "adduser") == 0){
			if(auth){
				if(newUser(argvNew[1], argvNew[2])){
					m->setCmd("Successfully added new user");
				} else{
					m->setCmd(err);
				}
			} else{
				m->setCmd(nauth);
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "id") == 0){
			if(auth){
				if(positiveID(argvNew[1], argvNew[2], argvNew[3])){
					m->setCmd("Successfully ID'd body");
				} else{
					m->setCmd(err);
				}
			} else{
				m->setCmd(nauth);
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "removemissing") == 0){
			if(auth){
				if(removeFromPublic(argvNew[1])){
					m->setCmd("Successfully removed entry");
				} else{
					m->setCmd(err);
				}
			} else{
				m->setCmd(nauth);
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "removebody") == 0){
			if(auth){
				if(removeFromAdmin(argvNew[1])){
					m->setCmd("Successfully removed entry");
				} else{
					m->setCmd(err);
				}
			} else{
				m->setCmd(nauth);
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "password") == 0){
			if(auth){
				if(changePassword(argvNew[1], argvNew[2], argvNew[3])){
					m->setCmd("Successfully changed password");
				} else{
					m->setCmd("Either the given username does not exist, or the old password doesn't match for that user.");
				}
			} else{
				m->setCmd(nauth);
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "createbody") == 0){
			if(auth){
				if((argvNew[3] != NULL) && (argvNew[4] != NULL)){
					// full command
					if(createMissingAdmin(argvNew[1], argvNew[2], argvNew[3], argvNew[4])){
						m->setCmd("Successfully added entry");
					} else{
						m->setCmd(err);
					}
				} else{
					// no name given
					if(createMissingAdmin(argvNew[1], argvNew[2], NULL, NULL)){
						m->setCmd("Successfully added entry");
					} else{
						m->setCmd(err);
					}
				}
			} else{
				m->setCmd(nauth);
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "dlmissingphoto") == 0){
			if(auth){
				long s;
				m->setImg(getPhotoPublic(argvNew[1], &s), s);
				m->setCmd("Successfully retrieved photo!");
			} else{
				m->setCmd(nauth);
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "addphoto") == 0){
			if(addPhotoPublic(argvNew[1], inm->getContent(), inm->getContentSize())){
				m->setCmd("Successfully added photo");
			} else{
				m->setCmd(err);
			}
			nl -> FromApplicationLayer(m);
		} else if(strcmp(argvNew[0], "addbodyphoto") == 0){
			if(auth){
				if(addPhotoAdmin(argvNew[1], inm->getContent(), inm->getContentSize())){
					m->setCmd("Successfully added photo");
				} else{
					m->setCmd(err);
				}
			} else{
				m->setCmd(nauth);
			}
			nl -> FromApplicationLayer(m);
		} else{
			cerr << "Server received unrecognized command" << endl;
		}
		
	}
}


int AcceptConn(){
	int listen_socket;
	if ( (listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("Server-error creating listening socket");
		exit(EXIT_FAILURE);
	}

	struct    sockaddr_in servaddr;  /*  socket address structure  */
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(WELL_KNOWN_PORT);

	/*  Bind our socket addresss to the 
	listening socket, and call listen()  */
	int a = 1;
	setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &a, 4);

	if ( bind(listen_socket, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
		perror("Server-error calling bind()");
		exit(EXIT_FAILURE);
	}

	if ( listen(listen_socket, 30) < 0 ) {// All the connections!!
		perror("Server-error calling listen()");
		exit(EXIT_FAILURE);
	}

	cout << "Setup complete!" << endl;

	int sockfd;

	while(true){
		if ((sockfd = accept(listen_socket, NULL, NULL)) < 0 ) {
			perror("Server-error calling accept()");
			exit(EXIT_FAILURE);
		}
		int pid = fork();
		if(pid == 0) return sockfd;
	}

}
