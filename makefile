# Makefile

all: client server

client: client.o initClient.o PhysicalLayer.o
	g++ client.o initClient.o PhysicalLayer.o -o client

client.o: client.cpp did.h
	g++ -c client.cpp

initClient.o: initClient.cpp did.h
	g++ -c initClient.cpp

server: server.o PhysicalLayer.o db.o
	g++ -o server server.o PhysicalLayer.o db.o -Wall -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl

server.o: server.cpp did.h
	g++ -c server.cpp

db.o: db.cpp did.h
	g++ -c -I /usr/local/mysql-current/include db.cpp

did: did.h Frame.h Packet.h DataLink.h PhysicalLayer.h Message.h Utils.h

PhysicalLayer.o: PhysicalLayer.cpp did.h
	g++ -c PhysicalLayer.cpp -Wall
	
ray: DataLink.cpp Frame.cpp Packet.cpp Message.cpp ray.cpp did
	g++ ray.cpp DataLink.cpp Frame.cpp Packet.cpp Message.cpp Utils.cpp -o test -g
	
chris: DataLink.cpp Frame.cpp Packet.cpp Message.cpp chris.cpp did
	g++ chris.cpp DataLink.cpp Frame.cpp Packet.cpp Message.cpp Utils.cpp -o test -g
	
aaron: aaron.cpp PhysicalLayer.o did
	g++ aaron.cpp PhysicalLayer.o -o aaron -g

clean:
	rm *.o *~ *.h.gch -f
	rm client server test aaron -f
