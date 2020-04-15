#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <locale.h>
#include <time.h>
#include <signal.h>

#include <iostream>
using namespace std;

#define PORTNUM 8080
#define BACKLOG 5
#define BUFLEN 1024

#define FNFSTR "404 Error File Not Found"
#define BRSTR "Bad Request"


int Lok;
char Allow[] = "Allow: GET, HEAD\n",
ServerN[] = "Server: Model HTTP Server/0.1\n";
char LastM[50];


void SigHndlr(int s)
{
	int status;
	cout << "End of the work" << endl;
	shutdown(Lok, 0);
	close(Lok);
	while (wait(&status) != -1);
	signal(SIGINT,SIG_DFL); 
}


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
		if (!strcmp((head->next) -> str, "/\0"))
		{
			(head->next) -> str = new char[10];
			sprintf((head->next) -> str,"/index.html");
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
	
	word* gethead()
	{
		return head;
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


char *OpRF(char *name, unsigned long *len)
{
	char c;
	FILE *fp;
	unsigned long int i = 0, l;
	if ((fp = fopen(name, "rb")) == NULL)
		return NULL;
	fseek(fp, 0, SEEK_END);
	l = ftell(fp);
	*len = l;
	char *str = new char[l+1];
	fclose(fp);
	fp = fopen(name, "rb");
	while(!feof(fp))
	{
		fread(&c, sizeof(char), 1, fp);
		str[i] = c;
		i++;
	}
	str[i] = '\0';
	fclose(fp);
	return str;
}

char *GetEx(char *str)
{
	int n = strlen(str);
	int i, j, k = 0;

	if (strchr(str,'.') != NULL)
	{
		for (i = (strlen(str) - 1); i >= 0; i--)
			if (str[i] == '.')
			{
				j = i;
				break;
			}
		char *ex = new char[n - j];
		for (i = j + 1; i <= n; i++)
		{
			ex[k] = str[i];
			k++;
		}
		return ex;
	}
	else 
	{
		char *ex = new char[1];
		ex[0] = '\0';
		return ex;
	};
}



char *HeadS(List *head, int *CODE)
{
	struct tm *Date, *Last;
	long timeT, timeL;
	char DateS[50], LastS[60];
	unsigned int i = 0;
	char *name = (head -> getnext()) -> str;
	char *exs;
	char *type = new char[26];
	char *http;
	struct stat stbuf;
	char cod[5];
	char comm[22];

	for (i = 0; i < strlen(name); i++) 
		name[i] = name[i + 1];

	if (strcmp("GET\0", (head -> gethead()) -> str) && strcmp("HEAD\0", (head -> gethead()) -> str))
	{
		strcpy(cod, "501\0");
		*CODE = 501;
		strcpy(comm, "Service Unavailable\0");
	}
	else if (strchr(name,'?') != NULL)
	{
		strcpy(cod, "400\0");
		*CODE = 400;
		strcpy(comm, "Syntax error\0");
	}
	else if (stat(name, &stbuf) == -1)
	{
		strcpy(cod,"404\0");
		*CODE = 404;
		strcpy(comm,"Not found file\0");
	}
	else if ((((stbuf.st_mode) >> 2) | 1) == 0)
	{
		strcpy(cod, "403\0");
		*CODE = 403;
		strcpy(comm,"Forbidden\0");
	}
	else
	{
		strcpy(cod, "200\0");
		*CODE = 200;
		strcpy(comm, "Ok\0");
	}
	if (!strcmp((head -> gethead()) -> str, "GET\0"))
	{
		exs = GetEx(name);
		if (!strcmp(exs, "html\0"))
			sprintf(type, "text/html");
		else sprintf(type, "text/plain");
		if (*CODE == 200)
		{
			if (!strcmp(exs, "jpg\0"))
				sprintf(type, "image/jpeg");
			if (!strcmp(exs, "gif\0"))
				sprintf(type, "image/gif");
			if (!strcmp(exs, "png\0"))
				sprintf(type, "image/png");
			stat(name, &stbuf);
			timeL = stbuf.st_mtime;
			Last = gmtime(&timeL);
			strftime(LastS, 50, "Last-modified: %a, %d %b %Y %H:%M: %S GMT", Last);
		}
	}
	
	time(&timeT);
	Date = gmtime(&timeT);
	strftime(DateS, 50, "Date: %a, %d %b %Y %H:%M: %S GMT", Date);
	http = new char[strlen(cod)+40+strlen(comm)+strlen(DateS)+strlen(type)+strlen(LastS)+strlen(ServerN)+strlen(Allow)];
	if (*CODE == 404 || *CODE == 400 || *CODE == 403 || *CODE == 500 ||*CODE == 501 || *CODE == 503) sprintf(type, "text/html");
	if (*CODE == 501) sprintf(http, "HTTP/1.1 %s %s\nMIME-version: 1.1\n%s\nContent-Type: %s\n%s\n", cod, comm, DateS, type, Allow);
	if (*CODE == 200) sprintf(http, "HTTP/1.1 %s %s\nMIME-version: 1.1\n%s\nContent-Type: %s\n%s\n", cod, comm, DateS, type, LastS);
	else sprintf(http, "HTTP/1.1 %s %s\nMIME-version: 1.1\n%s\nContent-Type: %s\n", cod, comm, DateS, type);
	return http;
}


char *GetBody(List *head, int CODE, char *headM, unsigned long *length)
{
	char *str;
	unsigned long len;

	if (CODE == 200)
	{
		str = OpRF( (head -> getnext()) -> str, &len );
		*length = len;
	}
	if (CODE == 404)
	{
		str = new char[strlen( (head -> getnext()) -> str ) + 150];
		sprintf(str, "<HTML><HEAD><TITLE>ModalServer 1.1b</TITLE></HEAD>\n<BODY>\n<H1>Error 404\n Can't find: %s</H1>\n</HTML>", (head -> getnext()) -> str);
		*length = strlen(str) * sizeof(char);
	}
	if (CODE == 400){
		str = new char[strlen((head -> getnext()) -> str) + 150];
		sprintf(str, "<HTML><HEAD><TITLE>ModalServer 1.1b</TITLE></HEAD>\n<BODY>\n<H1>Syntax error!</H1>\n</HTML>");
		*length = strlen(str) * sizeof(char);
	}
	if (CODE == 403){
		str = new char[strlen((head -> getnext()) -> str) + 150];
		sprintf(str, "<HTML><HEAD><TITLE>ModalServer 1.1b</TITLE></HEAD>\n<BODY>\n<H1>Bad request You haven't permission!</H1>\n</HTML>");
		*length = strlen(str) * sizeof(char);
	}
	if (CODE == 500){
		str = new char[strlen((head -> getnext()) -> str) + 150];
		sprintf(str, "<HTML><HEAD><TITLE>ModalServer 1.1b</TITLE></HEAD>\n<BODY>\n<H1>Forbidden You haven't permission!</H1>\n</HTML>");
		*length = strlen(str) * sizeof(char);
	}
	if (CODE == 501){
		str = new char[strlen((head -> getnext()) -> str) + 150];
		sprintf(str, "<HTML><HEAD><TITLE>ModalServer 1.1b</TITLE></HEAD>\n<BODY>\n<H1>Not Implemented.</H1>\n</HTML>");
		*length = strlen(str) * sizeof(char);
	}
	if (CODE == 503){
		str = new char[strlen((head -> getnext()) -> str) + 150];
		sprintf(str, "<HTML><HEAD><TITLE>ModalServer 1.1b</TITLE></HEAD>\n<BODY>\n<H1>Service Unavailable.</H1>\n</HTML>");
		*length = strlen(str) * sizeof(char);
	}
	return str;
}


char *AddCL(char *str, unsigned long len)
{
	char *str1 = new char[35+strlen(ServerN)+strlen(str)];
	sprintf(str1, "%sContent-Length: %li\nServer: Model HTTP Server/0.1\n\n", str, len);
	return str1;
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


class Client: public Address {
	unsigned int len;
	int sockfd;
public:
	Client(): Address() 
	{
		len = sizeof(*( getaddr() ));
	}

	void Accept(int servnum)
	{
		sockfd = accept (servnum, (struct sockaddr*) getaddr(), &len);
	}

	void Request(char* buf)
	{
		int CODE;
		long unsigned int len;
		char *st, *HEAD;
		Lok = sockfd;
		if ((len = recv(sockfd, buf, BUFLEN-1, 0)) < 0)
		{
			cout << "error reading socket" << endl;
			return;
		}
		buf[BUFLEN] = 0;
		cout << "Server request:" << endl;
		cout << buf << endl;
		List newl;
		newl.CreatList(buf, strlen(buf));
		HEAD = HeadS(&newl, &CODE);
		st = GetBody(&newl, CODE, HEAD, &len);
		HEAD = AddCL(HEAD, len);
		cout << "Send:\n" << HEAD;
		send(sockfd, HEAD, strlen(HEAD) * sizeof(char), 0);
		if (!strcmp(newl.gethead() -> str,"GET\0"))
		{
			cerr << st << endl;
			send(sockfd, st, strlen(st) * sizeof(char), 0);
		}
		cout << "Answer was sent\n";
		cout << "\n--------------------------------------------------------\n";
		shutdown(sockfd, 0);
		close(sockfd);
	}

	int getsock()
	{
		return sockfd;
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
	
	int Get(char* buf)
	{
		int p;
		Client newclient;
		newclient.Accept(getnum());
		if ((p = fork()) == -1)
		{
			cout << "can't make a process" << endl;
			return 0;
		}
		else if (!p)
		{
			close(getnum());
			newclient.Request(buf);
			exit(0);  
		}
		close(newclient.getsock());
		return 0;
	}

	~Server()
	{
		close(getnum());
	}
};



int main(int argc, char** argv) 
{
	if (argc != 2)
	{
		cout << "I need PortNumber" << endl;
		return 0;
	}
	signal(SIGINT,SigHndlr);
	int portnum = atoi(argv[1]); 
	int status;
	char *buf = new char[BUFLEN];
	Server serv1(portnum);
	serv1.Bind();
	serv1.Listen();
	while(1)
	{
		serv1.Get(buf);
	}
	while (wait(&status) != -1);
	delete []buf;
	return 0;
}
