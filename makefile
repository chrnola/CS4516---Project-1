# Makefile

all: client server

client: client.cpp PhysicalLayer.cpp NetworkLayer.cpp Message.cpp Utils.cpp Packet.cpp DataLink.cpp Frame.cpp did
	g++ client.cpp PhysicalLayer.cpp NetworkLayer.cpp Message.cpp Packet.cpp Utils.cpp DataLink.cpp Frame.cpp -o client -g -pthread

server: server.cpp PhysicalLayer.cpp db.cpp Message.cpp NetworkLayer.cpp Frame.cpp Packet.cpp Utils.cpp DataLink.cpp did
	g++ -I/usr/local/mysql-current/include -o server -g server.cpp PhysicalLayer.cpp db.cpp Message.cpp NetworkLayer.cpp Frame.cpp Packet.cpp Utils.cpp DataLink.cpp -Wall -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl -pthread

did: db.h did.h Frame.h Packet.h DataLink.h NetworkLayer.h PhysicalLayer.h Message.h Utils.h

ray: DataLink.cpp Frame.cpp Packet.cpp Message.cpp ray.cpp did
	g++ ray.cpp DataLink.cpp Frame.cpp Packet.cpp Message.cpp Utils.cpp -o test -g -pthread

chris: db.cpp Message.cpp chris.cpp Utils.cpp Message.cpp did
	g++ chris.cpp db.cpp Message.cpp Utils.cpp -o chris -g -L /usr/local/mysql-current/lib/mysql -lmysqlclient -ldl

aaron: aaron.cpp PhysicalLayer.cpp did
	g++ aaron.cpp PhysicalLayer.cpp -o aaron -g -pthread

aaronserver: aaronserver.cpp PhysicalLayer.cpp did
	g++ aaronserver.cpp PhysicalLayer.cpp Frame.cpp Utils.cpp -o aaronserver -pthread

aaronclient: aaronclient.cpp did PhysicalLayer.cpp Utils.cpp Frame.cpp
	g++ aaronclient.cpp PhysicalLayer.cpp Frame.cpp Utils.cpp -o aaronclient -pthread

clean:
	rm *.o *~ *.h.gch -f
	rm client server test aaron aaronserver aaronclient chris -f
