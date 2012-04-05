# Makefile

all: client server

client: client.o initClient.o PhysicalLayer.o
	g++ client.o initClient.o PhysicalLayer.o -o client

client.o: client.C did
	g++ -c client.C

initClient.o: initClient.C did
	g++ -c initClient.C

server: server.o PhysicalLayer.o did
	g++ -o server server.o PhysicalLayer.o did.h PhysicalLayer.h -Wall

server.o: server.C did
	g++ -c server.C

did: did.h Frame.h Packet.h DataLink.h PhysicalLayer.h

db.o: db.C did
	g++ -c -I /usr/local/mysql-current/include db.C

PhysicalLayer.o: PhysicalLayer.cpp did
	g++ -c PhysicalLayer.cpp -Wall
	
DLTest: DataLink.h DataLink.cpp Frame.cpp Frame.h Packet.h Packet.cpp main.cpp
	g++ main.cpp DataLink.cpp Frame.cpp Packet.cpp -o test -g

clean:
	rm *.o -f
	rm *~ -f
	rm *.h.gch -f
	rm client server -f
