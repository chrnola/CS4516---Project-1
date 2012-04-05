#include "did.h"

int main(void) {
	Packet* p = new Packet();
	p->type = data;
	p->seq = 24856;
	p->end = true;
	p->payload = (unsigned char*) calloc(45, sizeof(unsigned char));
	string s = "This is a test packet. Yay it works.\n";
	char* str = const_cast<char*>(s.c_str());
	p->payload = (unsigned char*) strcpy((char*)p->payload, str);
	
	unsigned char* string = p->Serialize();
	//Packet* p1 = DataLink::UnserializeP((char*) string);
	//cout << "This is a test: " << string;
	//bool p1type = (p1->type+0 == data);
	//cout << "Packet is data: " << p1type;
	//cout << "\nPacket sequence number: " << p1->seq;
	//cout << "\nPacket is end: " << p1->end;
	//cout << "\nPacket message: " << p1->payload << "\n";
	cout << "Some signals are: " << SIGABRT << ";" << SIGALRM << ";" << SIGBUS << ";" << SIGCHLD << ";" << SIGCONT << ";" << SIGFPE << ";" << SIGHUP << ";" << SIGILL << ";" << SIGINT << ";" << SIGKILL << ";" << SIGPIPE << ";" << SIGQUIT << ";" << SIGSEGV << ";" << SIGSTOP << ";" << SIGTERM << ";" << SIGTSTP << ";" << SIGTTIN << ";" << SIGTTOU << ";" << SIGUSR1 << ";" << SIGUSR2 << ";" << SIGPOLL << ";" << SIGPROF << ";" << SIGSYS << ";" << SIGTRAP << ";" << SIGURG << ";" << SIGVTALRM << ";" << SIGXCPU << ";" << SIGXFSZ;
	
	DataLink* d = new DataLink();
	d->GoBack1();
}


