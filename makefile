# Makefile

client: client.o initClient.o
	g++ client.o initClient.o -o client

client.o: client.C did.h
	g++ -c client.C

initClient.o: initClient.C did.h
	g++ -c initClient.C

clean:
	rm *.o
