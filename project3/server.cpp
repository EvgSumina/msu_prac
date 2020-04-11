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


struct word
{
	char *str;
	struct word *next;
};

class List
{
	word* head;
public:
	List()
	{
		head = NULL;
	}
	
	void addNode(char *s, int len)
	{
		word *nd = new word;
		nd -> str = new char[strlen(s)+1];
		strcpy(nd -> str, s);
		nd -> next = NULL;
		
		if (head == NULL)
			head = nd;
		else 
		{
			word *current = head;
			
			while (current -> next != NULL)
				current = current -> next;
			current -> next = nd;
		}
	}
	
	void CreatList(char *str, int Count)
	{
		int i = 0, is = -1, j , k;

		while(i != (Count + 1))
		{
			if ( str[i] == ' ' || str[i] == '\n' || i == Count )
			{
				char s[i - is];
				k = 0;
				for (j = (is + 1); j < i; j++)
				{
					s[k] = str[j];
					k++;
				}
				s[k] = '\0';
				addNode(s, strlen(s));
				is=i;
			}
		i++;
		}
	}
	
	void print()
	{
		word *current = head;	
		while (current != NULL)
		{
			cout << current -> str << endl;
			current = current -> next;
		}
	}
	
	word* getnext()
	{
		return head -> next;
	}
	
	~List()
	{			
		while (head != NULL)
		{
			word *current = head -> next;
			delete [] head -> str;
			delete head;
			head = current;
		}
	}
};


char *OpRF(char *name)
{
	char c;
	FILE *fp;
	unsigned int i = 0;
	for (i = 0; i < strlen(name); i++) 
		name[i] = name[i + 1];
	if (name[0] == '\0') 
		strcpy(name,"index.htm\0");
	if ((fp = fopen(name, "r")) == NULL)
	{
		cout << "Reading error" << endl;
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	int l;
	l = ftell(fp);
	char *str = new char[l + 1];
	i = 0;
	fclose(fp);
	fp = fopen(name, "r");
	while((c = fgetc(fp)) != EOF)
	{
		str[i] = c;
		i++;
	}
	str[i] = '\0';
	fclose(fp);
	return str;
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
		if (bind(getnum(),(struct sockaddr*) getaddr(), sizeof(*(getaddr())) ) < 0)
		{
			cout << "Can't bind socket" << endl;
		}
		else 
			cout << "Bind is ok" << endl;
	}
	
	void Listen() const
	{
		if (listen(getnum(), BACKLOG) < 0)
		{
			cout << "Can't listen socket" <<endl;
		}
		else 
			cout << "Now I'm listening" << endl;
	};
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
		sockfd = accept (serv.getnum(), (struct sockaddr*) getaddr(), &len);
	}

	int Request(char* buf, Server serv)
	{
		int len, p;
		char *st;
		if ((p = fork()) == -1)
		{
			//cout << "can't make a process" << endl;
			return 0;
		}
		else if (!p)
		{
			close(serv.getnum());
			if ((len = recv(sockfd, buf, BUFLEN-1, 0)) < 0)
			{
				//cout << "error reading socket" << endl;
				return 0;
			}
			buf[BUFLEN] = 0;
			cout << "Server request:" << endl;
			cout << buf << endl;
			List newl;
			newl.CreatList(buf, strlen(buf));
			st = OpRF((newl.getnext()) -> str);
			cout << "-------------------------------------------------" << endl;
			send(sockfd,buffer,sizeof(buffer),0);
			if (st!=NULL)
				send(sockfd,st,strlen(st),0);
			cout << "Answer was sent\n";
			shutdown(sockfd, 0);
			close(sockfd);
			return 0; 
		}
		close(sockfd);
		return 0;
	}

	int getsock()
	{
		return sockfd;
	}

}; 


int main(int argc, char** argv) 
{
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
