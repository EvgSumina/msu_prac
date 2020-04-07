OPTIONS = -Wall -Werror

all: server

server: server.o 
	g++ server.o -o server

server.o: server.cpp
	g++ -g $(OPTIONS) -c server.cpp

run: server
	./server	

clean:
	rm -rf *.o server
