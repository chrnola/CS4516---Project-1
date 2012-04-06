# Makefile

all: client server

client: client.cpp PhysicalLayer.cpp NetworkLayer.cpp Message.cpp Utils.cpp Packet.cpp DataLink.cpp Frame.cpp did
	g++ client.cpp PhysicalLayer.cpp NetworkLayer.cpp Message.cpp Packet.cpp Utils.cpp DataLink.cpp Frame.cpp -o client -g

<<<<<<< HEAD
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
=======
server: server.cpp PhysicalLayer.cpp db.cpp
	g++ -o server -g server.cpp PhysicalLayer.cpp db.cpp -Wall -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl
>>>>>>> eac6d30e3f80fddbe2d356983bb99e58e0e64a8a

did: did.h Frame.h Packet.h DataLink.h NetworkLayer.h PhysicalLayer.h Message.h Utils.h
	
ray: DataLink.cpp Frame.cpp Packet.cpp Message.cpp ray.cpp did
	g++ ray.cpp DataLink.cpp Frame.cpp Packet.cpp Message.cpp Utils.cpp -o test -g -pthread
	
chris: db.cpp Message.cpp chris.cpp Utils.cpp did
	g++ chris.cpp db.cpp Message.cpp Utils.cpp -o chris -g -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl
	
aaron: aaron.cpp PhysicalLayer.cpp did
	g++ aaron.cpp PhysicalLayer.cpp -o aaron -g -pthread

clean:
	rm *.o *~ *.h.gch -f
	rm client server test aaron chris -f
