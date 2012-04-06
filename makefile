# Makefile

all: client server

client: client.o PhysicalLayer.o
	g++ client.o PhysicalLayer.o -o client

client.o: client.cpp did.h
	g++ -c client.cpp

server: server.o PhysicalLayer.o db.o
	g++ -o server server.o PhysicalLayer.o db.o -Wall -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl

server.o: server.cpp did.h
	g++ -c server.cpp

db.o: db.cpp did
	g++ -c -I /usr/local/mysql-current/include db.cpp

did: did.h Frame.h Packet.h DataLink.h PhysicalLayer.h Message.h Utils.h

PhysicalLayer.o: PhysicalLayer.cpp did.h
	g++ -c PhysicalLayer.cpp -Wall
	
ray: DataLink.cpp Frame.cpp Packet.cpp Message.cpp ray.cpp did
	g++ ray.cpp DataLink.cpp Frame.cpp Packet.cpp Message.cpp Utils.cpp -o test -g -pthread
	
chris: db.o Message.o chris.o did
	g++ chris.o db.o Message.o Utils.cpp -o chris -g -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl
	
chris.o: chris.cpp
	g++ -c chris.cpp
	
Message.o: Message.cpp did
	g++ -c Message.cpp
	
aaron: aaron.cpp PhysicalLayer.o did
	g++ aaron.cpp PhysicalLayer.o -o aaron -g

clean:
	rm *.o *~ *.h.gch -f
	rm client server test aaron chris -f
