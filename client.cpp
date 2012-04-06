/*
 * client.C
 */

#include "did.h"

using namespace std;

NetworkLayer *nl;

void startPrompt();
void testPL();

int main(int argc, char **argv) {
	// testPL();
	
	// init lower leves, spawn threads, etc
	// once connected on well known port...	
	nl = new NetworkLayer();
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
			} else{
				cout << "Error: createmissing expects firstName lastName lastKnownLocation" << endl;
			}
		} else if(strcmp(argvNew[0], "login") == 0){
			if((argvNew[1] != NULL) && 
			 (argvNew[2] != NULL)){
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
			} else{
				cout << "Error: login expects username password" << endl;
			}
		} else if(strcmp(argvNew[0], "query") == 0){
			if((argvNew[1] != NULL) && 
			 (argvNew[2] != NULL)){
				Message *m = new Message();
				int size = strlen(argvNew[0]) + strlen(argvNew[1]) + 
						strlen(argvNew[2];
				char *cmd = (char *) calloc(size, sizeof(char));
				strcpy(cmd, argvNew[0]);
				strcat(cmd, argvNew[1]);
				strcat(cmd, argvNew[2]);
				m->setCmd(cmd);
				nl -> FromApplicationLayer(m);
				//TODO: wait for response, display it
			} else{
				cout << "Error: query expects firstName lastName" << endl;
			}
		} else{
			cout << "Bad command! Try 'help'." << endl;
		}
		
		startPrompt();
	}
	
}

void testPL(){
	PhysicalLayer pl = PhysicalLayer("localhost");
}
