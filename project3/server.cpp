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
#define BUFLEN 1024

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
		SockAddress.sin_family = AF_INET;
		SockAddress.sin_addr.s_addr = INADDR_ANY;
		SockAddress.sin_port = htons(PORTNUM);
	};

	Address(unsigned short portNum) 
	{
		memset(&SockAddress, 0, sizeof(SockAddress));
		SockAddress.sin_family = AF_INET;
		SockAddress.sin_addr.s_addr = INADDR_ANY;
		SockAddress.sin_port = htons(portNum);
	};
	
	const struct sockaddr_in * getaddr() const
	{
		return &SockAddress;
	}	
};

class Server: public Socket, public Address {
public:
	Server(unsigned short portNum): Socket(), Address(portNum) {};
	
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

class Client: public Address {
	unsigned int len;
	int sockfd;
public:
	Client(): Address() 
	{
		len = sizeof(*( getaddr() ));
	}

	void Accept(Server serv)
	{
		try
		{
			if ((sockfd = accept (serv.getnum(), (struct sockaddr*) getaddr(), &len)) < 0)
			{
				throw 3;
			}
		}

		catch (int i)
		{
			cout<<"error accepting connection"<<endl;
		}
	}

	int Request(char* buf, Server serv)
	{
		int len, i, filefd, p;
		try
		{
			if ((p = fork()) == -1)
			{
				throw serv;
			}
			else if (!p)
			{
				close(serv.getnum());
				if ((len = recv(sockfd, buf, BUFLEN-1, 0)) < 0)
				{
					throw 12;
				}
				buf[BUFLEN] = 0;
				fprintf(stderr, "Request=%s", buf);
				if (strncmp(buf, "GET /", 5))
				{
					if (send(sockfd, BRSTR, strlen(BRSTR) + 1, 0) != strlen(BRSTR) + 1) 
					{
						throw 'a';
					}
					shutdown(sockfd, 1);
					close(sockfd);
					return 0;
				}
				for (i = 5; buf[i] && (buf[i] > ' '); i++);
				buf[i] = 0;
				if ((filefd = open(buf+5, O_RDONLY)) < 0)
				{
					if (send(sockfd, FNFSTR, strlen(FNFSTR) + 1, 0) != strlen(FNFSTR) + 1)
					{
						throw 'a';
					}
					shutdown(sockfd, 1);
					close(sockfd);
					return 0;
				}
				while ((len = read(filefd, &buf, BUFLEN)) > 0)
				{
					if (send(sockfd, buf, len, 0) < 0) 
					{
						throw 'a';
					}
				}
				close(filefd);
				shutdown(sockfd, 1);
				close(sockfd);
				return 0; 
			}
			close(sockfd);
		}

		catch (Server serv)
		{
			cout << "can't make a process" << endl;
		}
		catch (char x)
		{
			cout << "error writing socket" << endl;
		}
		catch (int x)
		{
			cout << "error reading socket" << endl;
		}
		return 0;
	}

	int getsock()
	{
		return sockfd;
	}

}; 



int main(int argc, char** argv) {
	char buf[BUFLEN];
	Server serv1(1234);
	serv1.Bind();
	serv1.Listen();
	while(1)
	{
		Client newclient;
		newclient.Accept(serv1);
		newclient.Request(buf, serv1);
	}
	return 0;
}
