OPTIONS = -Wall -Werror

all: server analizator

analizator:  analizator.o
	g++ analizator.o -o analizator

analizator.o: analizator.cpp
	g++ -g $(OPTIONS) -c analizator.cpp

server: server.o 
	g++ server.o -o server

server.o: server.cpp
	g++ -g $(OPTIONS) -c server.cpp

run: server
	./server 2233

an: analizator
	./analizator

clean:
	rm -rf *.o *.out server analizator
