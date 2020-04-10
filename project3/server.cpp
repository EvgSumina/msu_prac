#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

#include <iostream>
using namespace std;

#define PORTNUM 8080
#define BACKLOG 5
#define BUFLEN 1024

#define FNFSTR "404 Error File Not Found"
#define BRSTR "Bad Request"

char buffer[] = "HTTP/1.1 200 Ok\nContent-Type: text/html";

vector <string> CreatVect(char *str, int Count)
{
	int i = 0, k, is, j;
	vector <string> ivector;
	string x;

	while(i != (Count + 1))
	{
		if (str[i] == ' ' || str[i] == '\n' || i == Count)
		{
			k = 0;
			for (j = (is+1); j < i; j++)
			{
				x[k] = str[j];
				k++;
			}
			x[k] = '\0';
			ivector.push_back(x);
			is = i;
		}
	i++;
	}
	return (ivector);
}

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
	friend vector <string> CreatVect(char *str, int Count);
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
		int len, p;
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
				cout << endl;
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
				vector <string> ivector = ::CreatVect(buf, BUFLEN);
				for (unsigned int i = 0; i < ivector.size(); i++)
					cout << ivector[i] << endl;
				/*if ((filefd = open(buf+5, O_RDONLY)) < 0)
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
				close(filefd);*/
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



/*string OpRF(string name)
{
	char c;
	FILE *fp;
	unsigned int i = 0, l;
	string str;
	for (i = 0; i < name.length(); i++) 
		name[i] = name[i+1];
	if (name[0] == '\0') 
		name = "index.htm\0";
	try
	{
		if ((fp = fopen(name,"r")) == NULL)
		{
			throw 4;
		}
	}
	
	catch (int i)
	{
		cout << "Reading error" << endl;
	}
	
	fseek(fp, 0, SEEK_END);
	l = ftell(fp);
	close(fp);
	i = 0;
	fp=fopen(name,"r");
	while((c = fgetc(fp)) != EOF)
	{
		str[i] = c;
		i++;
	}
	str[i]='\0';
	close(fp);
	return str;
}*/



int main(int argc, char** argv) {
	if (argc != 2)
	{
		cout << "I need PortNumber" << endl;
		return 0;
	}
	int portnum = atoi(argv[1]);
	char *buf = new char[BUFLEN];
	Server serv1(portnum);
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
