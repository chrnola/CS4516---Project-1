# Makefile

all: client server

client: client.o initClient.o PhysicalLayer.o
	g++ client.o initClient.o PhysicalLayer.o -o client

client.o: client.cpp did.h
	g++ -c client.cpp

initClient.o: initClient.cpp did.h
	g++ -c initClient.cpp

server: server.o PhysicalLayer.o did.h
	g++ -o server server.o PhysicalLayer.o did.h PhysicalLayer.h -Wall

server.o: server.cpp did.h
	g++ -c server.cpp

db.o: db.cpp did.h
	g++ -c -I /usr/local/mysql-current/include db.cpp

did: did.h Frame.h Packet.h DataLink.h PhysicalLayer.h

PhysicalLayer.o: PhysicalLayer.cpp did.h
	g++ -c PhysicalLayer.cpp -Wall
	
DLTest: DataLink.h DataLink.cpp Frame.cpp Frame.h Packet.h Packet.cpp main.cpp
	g++ main.cpp DataLink.cpp Frame.cpp Packet.cpp -o dltest -g

clean:
	rm *.o -f
	rm *~ -f
	rm *.h.gch -f
	rm client server -f
