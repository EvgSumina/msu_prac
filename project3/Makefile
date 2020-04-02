all: server

server: server.o 
	g++ server.o -o server

server.o: server.cpp
	g++ -g -Wall -c server.cpp

clean:
	rm -rf *.o server
