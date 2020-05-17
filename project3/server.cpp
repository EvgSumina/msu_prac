#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>
#include <set>
#include <vector>
#include <algorithm>
#include <sstream>

#include <iostream>
using namespace std;

#define BUFLEN 1024
#define BACKLOG 5

int Lok;

void SigHndlr(int s)
{
    cout << "\nEnd of the work" << endl;
    shutdown(Lok, 0);
    close(Lok);
    signal(SIGINT,SIG_DFL); 
}

void SigChldr(int s)
{
    int status;
    wait(&status);
    signal(SIGCHLD,SigChldr); 
}

char* itoa (int n)
{
    char* buf = new char[10];
    sprintf(buf, "%d", n);
    strcat(buf, "\0");
    return buf;
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
    
    int getport() const
    {
        return SockAddress.sin_port;
    }
    
    const struct sockaddr_in * getaddr() const
    {
        return &SockAddress;
    }    
};


struct Server: public Socket, public Address {

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
        
    ~Server()
    {
        close(getnum());
    }
};


class Client: public Address {
    unsigned int len;
    int sockfd;
    int fd;
    char buf[BUFLEN];
    
    int GetLength(int fd)
    {
        char c;
        int len = 0;
        while(read(fd, &c, 1)) len++;
        lseek(fd, 0, 0);
        return len;
    }
    
    bool is_cgi(char * str)
    {
        if (strncmp(str, "cgi-bin", 7))
            return false;
        else
            return true;
    }
    
    char* get_all_env(char* str)
    {
        char* all_env = new char[BUFLEN];
        int i = strlen(buf) - 1, k = 0;
        while(str[i] != '?')
        {
            if(str[i--] == 0)
            {
                all_env[0] = 0;
                return all_env;
            }
        }
        for(unsigned int j = i + 1; j < strlen(str); j++) all_env[k++] = str[j];
        return all_env;
    }
    
    char* get_first(char* str) const
    {
        char* first = new char[BUFLEN];
        strcpy(first, str);
        int i = 0;
        while(first[i++] != '?');
        first[i-1] = 0;
        return first;
    }
 
public:

    Client(): Address(){};

    void Accept(int servnum)
    {
        sockfd = accept (servnum, (struct sockaddr*) getaddr(), &len);
        if (servnum < 0)
        {
            cout << "error accepting" << endl;
        }
    }
    
    int Request(int port)
    {
        Lok = sockfd;
        bool home = true;
        if((len = recv(sockfd, &buf, BUFLEN, 0)) < 0)
        {
            shutdown(sockfd, SHUT_RDWR);
            close(sockfd);
            cerr << "Error with reading socket" << endl;
            exit(3);
        }
        
        if(strncmp(buf, "GET", 3))
        {
            Send("501.html", "HTTP/1.0 501 NotImplemented");
            shutdown(sockfd, SHUT_RDWR);
            close(sockfd);
            cerr << "Error: BadRequest" << endl;
            return 501;
        }
        else 
        {
            int i = 5;
            char c = buf[i];
            while(c != ' ')
            {
                i++;
                c = buf[i];
            }
            char path[i - 3];
            if(i != 5)
            {
                home = false;
                copy(&buf[5], &buf[i], &path[0]);
                path[i - 5] = 0;
            }
            else
            {
                path[0] = '/';
                path[1] = 0;
            }
            cout << "File: " << path << endl;
            if (is_cgi(path))
            {
                cout << "It is cgi" << endl;
                chdir("./cgi-bin");
                int status;
                int pid;
                string name = to_string(getpid()) + ".txt";
                int fd = open(name.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644);
                dup2(fd, 1);
                if((pid = fork()) < 0)
                {
                    cerr << "Can't make process" << endl;
                    exit(6);
                }
                else if (pid == 0)
                {
                    //chdir("./cgi-bin");
                    string processName = "./";
                    processName += get_first(path);
                    char *argv[] = { (char*)processName.c_str(), NULL };
                    string QUERY_STRING = "QUERY_STRING=";
                    QUERY_STRING += get_all_env(path);
                    char *env[] = { (char*)QUERY_STRING.c_str(), NULL};
                    execve(processName.c_str(), argv, env);
                    perror("execve() ");
                    delete [] argv[0];
                    delete [] env[0];
                    exit(7);
                }
                wait(&status);
                close(fd);
                if(WIFEXITED(status))
                {
                    if(WEXITSTATUS(status) == 0)
                    {
                        Send(name.c_str(), "HTTP/1.0 200 MyServer");
                    }
                    else
                    {
                        cerr << "CGI finished with status " << WEXITSTATUS(status) << endl;
                        Send("cgi.html", "HTTP/1.0 500 MyServer");
                    }
                }
                else if(WIFSIGNALED(status))
                {
                    cerr << "CGI sent signal " << WIFSIGNALED(status) << endl;
                    Send("cgi.html", "HTTP/1.0 500 MyServer");
                }
            }
            else
            {
                if(!home && (fd = open(path, O_RDONLY)) < 0)
                {
                    Send("404.html", "HTTP/1.0 404 PageNotFound");
                    shutdown(sockfd, SHUT_RDWR);
                    close(sockfd);
                    cerr << "Error: 404" << endl;
                    return 404;
                }
                if(home) Send("index.html", "HTTP/1.0 200 MyServer");
                else Send(path, "HTTP/1.0 200 MyServer");
            }
        }
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        return 0;
    }
    
    void Send(const char* file, const char* header)
    {
        int fileLength;
        string strFileLength;
        fd = open(file, O_RDONLY);
        fileLength = GetLength(fd);
        strcpy(buf, header);
        strcat(buf, "\nAllow: GET\nServer: MyServer/0.1\nConnection: keep-alive\nContet-length: ");
        strFileLength = to_string(fileLength);
        strcat(buf, strFileLength.c_str());
        strcat(buf, "\n\n");
        len = strlen(buf);
        send(sockfd, &buf, len, 0);
        while((len = read(fd, buf, BUFLEN)) > 0)
        {
            send(sockfd, &buf, len, 0);
        }
        close(fd);
    }
    
    char* get_buf()
    {
        return buf;
    }
    
    int get_socket() const
    {
        return sockfd;
    }
};


int main(int argc, char* argv[]) {
    if (argc != 2)
    {
        cout << "I need PortNumber" << endl;
        return 0;
    }
    
    signal(SIGINT,SigHndlr);
    signal(SIGCHLD,SigChldr);
    int portnum = atoi(argv[1]); 
    
    Server server(portnum);
    
    server.Bind();
    server.Listen();
    
    while(1)
    {
        Client client;
        client.Accept(server.getnum());
        client.Request(portnum);
    }
    return 0;
}
