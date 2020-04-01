#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
using namespace std;

#define PORTNUM 8080
#define BACKLOG 5

#define FNFSTR "404 Error File Not Found"
#define BRSTR "Bad Request"

class Socket {
	int numSocket;
public:
	Socket() 
	{
		try
		{
			if ((numSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
			{
				throw 1;
			}
			 
		}
		
		catch (int i) 
		{
			cout << "can't create socket\n";
		}
	}
	
	int getnum() const
	{
		return numSocket;
	}
};

class Address {
	struct sockaddr_in SockAddress;
public:
	Address() 
	{
		memset(&SockAddress, 0, sizeof(SockAddress));
	};

	Address(int i) 
	{
		memset(&SockAddress, 0, sizeof(SockAddress));
		SockAddress.sin_family = AF_INET;
		SockAddress.sin_addr.s_addr = INADDR_ANY;
		SockAddress.sin_port = htons(PORTNUM);
	};
	
	void SetAddr() const
	{
		
	}
	
	const struct sockaddr_in * getaddr() const
	{
		return &SockAddress;
	}	
};

class Server: public Socket, public Address {
public:
	Server(): Socket(), Address(3) {};
	
	void Bind() const
	{
		try
		{
			if (bind(getnum(),(struct sockaddr*) getaddr(), sizeof(*(getaddr())) ) < 0)
			{
				throw 3;
			}
		}
		
		catch (int i)
		{
			cout << "can't bind socket" << endl;
		}
	}
	
	void Listen() const
	{
		try
		{
			if (listen(getnum(), BACKLOG) < 0)
			{
				throw 4;
			}
		}
		
		catch (int i)
		{
			cout << "can't listen socket" <<endl;
		}
	}

};



int main(int argc, char** argv) {
	Server serv1;
	serv1.Bind();
	serv1.Listen();
	return 0;
}
