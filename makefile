# Makefile

all: client server

client: client.cpp PhysicalLayer.cpp NetworkLayer.cpp Message.cpp Utils.cpp Packet.cpp DataLink.cpp Frame.cpp did
	g++ client.cpp PhysicalLayer.cpp NetworkLayer.cpp Message.cpp Packet.cpp Utils.cpp DataLink.cpp Frame.cpp -o client -g

server: server.cpp PhysicalLayer.cpp db.cpp
	g++ -I/usr/local/mysql-current/include -o server -g server.cpp PhysicalLayer.cpp db.cpp Message.cpp -Wall -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl

did: db.h did.h Frame.h Packet.h DataLink.h NetworkLayer.h PhysicalLayer.h Message.h Utils.h
	
ray: DataLink.cpp Frame.cpp Packet.cpp Message.cpp ray.cpp did
	g++ ray.cpp DataLink.cpp Frame.cpp Packet.cpp Message.cpp Utils.cpp -o test -g -pthread
	
chris: db.cpp Message.cpp chris.cpp Utils.cpp Message.cpp did
	g++ chris.cpp db.cpp Message.cpp Utils.cpp -o chris -g -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl
	
aaron: aaron.cpp PhysicalLayer.cpp did
	g++ aaron.cpp PhysicalLayer.cpp -o aaron -g -pthread

clean:
	rm *.o *~ *.h.gch -f
	rm client server test aaron chris -f
