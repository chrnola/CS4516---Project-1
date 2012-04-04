# Makefile

all: client server

client: client.o initClient.o
	g++ client.o initClient.o -o client

client.o: client.C did.h
	g++ -c client.C

initClient.o: initClient.C did.h
	g++ -c initClient.C

server: server.o db.o
	g++ -o server server.o db.o -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl

server.o: server.C did.h
	g++ -c server.C

db.o: db.C did.h
	g++ -c -I /usr/local/mysql-current/include db.C

PhysicalLayer.o: PhysicalLayer.cpp PhysicalLayer.h
	g++ -c PhysicalLayer.cpp PhysicalLayer.h
	
DLTest: DataLink.h DataLink.cpp Frame.cpp Frame.h Packet.h Packet.cpp main.cpp
	g++ main.cpp DataLink.cpp Frame.cpp Packet.cpp -o test -g

clean:
	rm *.o -f
	rm *~ -f
