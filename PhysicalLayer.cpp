/*
 * PhysicalLayer.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: Aaron Sikes
 */

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>		// Required for getaddrinfo
#include <netdb.h>
#include <string.h>		// Required for memset()


#include "did.h"

using namespace std;


// Constructor called by client. It connects to the well-known port of the server,
// then passes control to the general physical layer code.
PhysicalLayer::PhysicalLayer(const char *hostname) {
	struct addrinfo hints;		/*  Hints for getaddrinfo */
	struct addrinfo *resolved;	/*  Result from resolving hostname */

	//  Set up hints before resolving hostname
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;         // use IPv4
	hints.ai_socktype = SOCK_STREAM;   //TCP/IP

	// Resolves hostname
	char portString[10];
	sprintf(portString, "%d", WELL_KNOWN_PORT);

	if(getaddrinfo(hostname, portString, &hints, &resolved)){
		cerr << "Couldn't resolve host." << endl;
		exit(EXIT_FAILURE);
	}
	if(resolved == NULL){
		cerr << "Couldn't resolve host." << endl;
		exit(EXIT_FAILURE);
	}

	// Creates the socket with which to connect to the server
	// Note ai_family, ai_socktype will be the same as those in hints
	if ( (sockfd = socket(resolved->ai_family, resolved->ai_socktype, resolved->ai_protocol)) < 0 ) {
		cerr << "Client: Error creating socket." << endl;
		exit(EXIT_FAILURE);
	}

	// Connects to the server
	// ai_addr is the resolved IP address of the host
	if ( connect(sockfd, resolved->ai_addr, resolved->ai_addrlen ) < 0 ) {
		cerr << "Couldn't connect() to host." << endl;
		exit(EXIT_FAILURE);
	}

	// sockfd is set and valid, so pass control to the main code
	run();

}

// Constructor called by server. sockfd is already connected with the client,
// so just passes control to the general physical layer code.
PhysicalLayer::PhysicalLayer(int sockfd) {
	PhysicalLayer::sockfd = sockfd;
	run();
}

// Generic physical layer code
// Should send out any frames received from DL, and pass up any frames received.
// Placeholder tester code for now.
void PhysicalLayer::run(){
	int i = 3;
	sleep(1);
	send(sockfd, &i, 4, 0);
	i = 2;
	recv(sockfd, &i, 4, 0);
	cout << "Frame received! i=" << i << endl;
}

PhysicalLayer::~PhysicalLayer() {
	close(sockfd);
}
