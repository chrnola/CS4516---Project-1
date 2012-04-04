# Makefile

all: client server

client: client.o initClient.o
	g++ client.o initClient.o -o client

client.o: client.cpp did.h
	g++ -c client.cpp

initClient.o: initClient.cpp did.h
	g++ -c initClient.cpp

server: server.o db.o
	g++ -o server server.o db.o -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl

server.o: server.cpp did.h
	g++ -c server.cpp

db.o: db.cpp did.h
	g++ -c -I /usr/local/mysql-current/include db.cpp

PhysicalLayer.o: PhysicalLayer.cpp PhysicalLayer.h
	g++ -c PhysicalLayer.cpp PhysicalLayer.h
	
DLTest: DataLink.h DataLink.cpp Frame.cpp Frame.h Packet.h Packet.cpp main.cpp
	g++ main.cpp DataLink.cpp Frame.cpp Packet.cpp -o dltest -g

clean:
	rm *.o -f
	rm *~ -f
