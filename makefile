# Makefile

client: initClient.o client.o
	g++ -o client initClient.o client.o

initClient.o: initClient.C did.h
	g++ -c initClient.C

client.o: client.C did.h
	g++ -c client.C
