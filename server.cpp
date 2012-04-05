//server.C

#include <iostream>
#include <netinet/in.h>		// For sockadd_in
#include <arpa/inet.h>		// For htons, htonl, etc
#include <sys/socket.h>
#include <cstdio>		// For perror()
#include <string.h>		// For memset
#include <unistd.h>		// For close()

#include "did.h"

using namespace std;

void testDB();
void testPL();

int main(){
	//testPL();
	testDB();
}


void testDB(){
	cout << "Connecting to database" << endl;
	bool connected = connectToDB();
	char *bl;
	if (connected){
		cout << "Connected!" << endl;
		//int rws = getRowCountPublic();
		//cout << createMissing("Bob", "Smith", "Worcester, MA") << endl;
		cout << locationsWithMissing() << endl;
		if(createMissingAdmin("000000008", "Haverhill, MA", "John", "Dough")){
			cout << "it worked!" << endl;
		}
		if(createMissingAdmin("000000009", "Acton, MA", NULL, NULL)){
			cout << "it also worked!" << endl;
		}
	} else{
		cout << "Connection error!" << endl;
	}
	disconnectFromDB();
}


void testPL(){
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
	if ((sockfd = accept(listen_socket, NULL, NULL)) < 0 ) {
		perror("Server-error calling accept()");
		exit(EXIT_FAILURE);
	}
	else{
		cout << "Accepted something!" << endl;
		PhysicalLayer pl = PhysicalLayer(sockfd);
	}


	close(listen_socket);

}
