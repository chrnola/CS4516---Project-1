/*
 * client.C
 */

#include "did.h"

using namespace std;

void startPrompt();
void testPL();

NetworkLayer nl;

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
			m->setCmd();
			
			
		} else{
			cout << "Bad command! Try 'help'." << endl;
		}
		
		startPrompt();
	}
	
}

void testPL(){
	PhysicalLayer pl = PhysicalLayer("localhost");
}
