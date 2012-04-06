# Makefile

all: client server

client: client.o PhysicalLayer.o NetworkLayer.o Message.o Utils.o
	g++ client.o PhysicalLayer.o NetworkLayer.o Message.o Packet.o Utils.o -o client

client.o: client.cpp did
	g++ -c client.cpp
	
NetworkLayer.o: NetworkLayer.cpp did
	g++ -c NetworkLayer.cpp
	
Packet.o: Packet.cpp did
	g++ -c Packet.cpp
	
Message.o: Message.cpp did
	g++ -c Message.cpp
	
PhysicalLayer.o: PhysicalLayer.cpp did
	g++ -c PhysicalLayer.cpp
	
Utils.o: Utils.cpp did
	g++ -c Utils.cpp

server: server.o PhysicalLayer.o db.o
	g++ -o server server.o PhysicalLayer.o db.o -Wall -pthread -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl

server.o: server.cpp did.h
	g++ -c server.cpp

db.o: db.cpp did
	g++ -c -I /usr/local/mysql-current/include db.cpp

did: did.h Frame.h Packet.h DataLink.h NetworkLayer.h PhysicalLayer.h Message.h Utils.h
	
ray: DataLink.cpp Frame.cpp Packet.cpp Message.cpp ray.cpp did
	g++ ray.cpp DataLink.cpp Frame.cpp Packet.cpp Message.cpp Utils.cpp -o test -g -pthread
	
chris: db.o Message.o chris.o did
	g++ chris.o db.o Message.o Utils.cpp -o chris -g -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl
	
chris.o: chris.cpp
	g++ -c chris.cpp
	
aaron: aaron.cpp PhysicalLayer.o did
	g++ aaron.cpp PhysicalLayer.o -o aaron -pthread

clean:
	rm *.o *~ *.h.gch -f
	rm client server test aaron chris -f
