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

struct ArrOfPids{
    int size;
    int* array;
    
    ArrOfPids()
    {
        size = 0;
        array = new int[0];
    }
    
    void Add(int pid)
    {
        int i;
        int* newArr = new int[size + 1];
        for (i = 0; i < size; i++)
        {
            newArr[i] = array[i];
        }
        newArr[size] = pid;
        size++;
        delete [] array;
        array = newArr;
    }
    
    void Resize(int pid)
    {
        int* newArr = new int[size - 1];
        int i = 0;
        for(i = 0; array[i] != pid; i++) newArr[i] = array[i];
        for(int j = i + 1; j < size; j++) newArr[j - 1] = array[i];
        size--;
        delete [] array;
        array = newArr;
    }
    
    int &operator[] (int i)
    {
        return array[i];
    }
    
    ~ArrOfPids()
    {
        delete [] array;
    }
};

ArrOfPids pids;

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
    
    bool is_cgi()
    {
        char* last = get_last();
        for(unsigned int i = 0; i < strlen(last); i++)
        {
            if(last[i] == '?')
            {
                delete [] last;
                return true;
            }
        }
        delete [] last;
        return false;
    }
    
    int get_amps()
    {
        char* last = get_last();
        int i = 0, n = 0;
        while(last[i] != '?') i++;
        while(last[i] != 0)
        {
            if(last[i] == '&') n++;
            i++;
        }
        delete [] last;
        return n;
    }
    
    char* get_argv0()
    {
        char* last = get_last();
        int i = 0;
        while(last[i] != '?') i++;
        last[i] = 0;
        return last;
    }
    
    char* get_all_env()
    {
        char* all_env = new char[BUFLEN];
        int i = strlen(buf) - 1, k = 0;
        while(buf[i] != '?')
        {
            if(buf[i--] == 0)
            {
                all_env[0] = 0;
                return all_env;
            }
        }
        for(unsigned int j = i+1; j < strlen(buf); j++) all_env[k++] = buf[j];
        return all_env;
    }
    
    char* get_first() const
    {
        char* first = new char[BUFLEN];
        strcpy(first, buf + 5);
        int i = 0;
        while(first[i++] != '?');
        first[i - 1] = 0;
        return first;
    }
    
    char* get_last() const
    {
        char* last = new char[BUFLEN];
        int i = 5, j = 0;
        while(buf[i] != 0)
        {
            if(buf[i] != '/') last[j++] = buf[i];
            else j = 0;
            i++;
        }
        last[j] = 0;
        return last;
    }
    
    void str_put(char* dest, const char* src)
    {
        dest = new char[strlen(src) + 1];
        strcpy(dest, src);
        dest[strlen(src)] = 0;
    }
    
    void str_put2(char* dest, const char* src1, const char* src2)
    {
        dest = new char[strlen(src1) + strlen(src2) + 1];
        strcpy(dest, src1);
        strcat(dest, src2);
        dest[strlen(src1) + strlen(src2)] = 0;
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
    
    int Read()
    {
        if((len = recv(sockfd, &buf, BUFLEN, 0)) < 0)
        {
            shutdown(sockfd, 1);
            close(sockfd);
            cerr << "Error with reading socket" << endl;
            exit(1);
        }
        
        if(!isalpha(buf[0])) return -1;
        
        if(strncmp(buf, "GET /", 5))
        {
            Send("501.html", "HTTP/1.0 501 NotImplemented");
            shutdown(sockfd, 1);
            close(sockfd);
            cerr << "Error: BadRequest" << endl;
            return 501;
        }
        
        int i = 5;
        while(buf[i] && (buf[i++] > ' '));
        buf[i - 1] = 0;
        return 0;
    }
    
    int Request(int port)
    {
        Lok = sockfd;
        if(Read() == -1) 
            return 0;
        if(is_cgi())
        {
            int pid = fork();
            if(pid < 0)
            {
                cerr << "Не удалось создать процесс" << endl;
                exit(6);
            }
            else if (pid == 0)
            {
                char** argv = new char*[2];
                argv[0] = get_argv0();
                argv[1] = NULL;
                char** env = new char*[7];
                str_put(env[0], "SERVER_ADDR=127.0.0.1");
                str_put(env[1], "CONTENT_TYPE=text/plain");
                str_put(env[2], "SERVER_PROTOCOL=HTTP/1.0");
                char* str_first = get_first();
                str_put2(env[3], "SCRIPT_NAME=", str_first);
                delete [] str_first;
                char* str_port = itoa(port);
                str_put2(env[4], "SERVER_PORT=", str_port);
                delete [] str_port;
                char* str_all_env = get_all_env();
                str_put2(env[5], "QUERRY_STRING=", str_all_env);
                delete [] str_all_env;
                env[6] = NULL;
                char* str_pid = itoa(getpid());
                char* filename = new char[strlen(str_pid) + 8];
                strcpy(filename, "tmp");
                strcat(filename, str_pid);
                strcat(filename, ".txt");
                filename[strlen(str_pid) + 7] = 0;
                creat(filename, 0666);
                int fd = open(filename, O_TRUNC | O_WRONLY);
                if(fd < 0)
                {
                    cout << "Ошибка при создании файла" << endl;
                    exit(9);
                }
                dup2(fd, 1);
                close(fd);
                execvpe(argv[0], argv, env);
                cerr << "Не удалось запустить cgi" << endl;
                delete [] str_pid;
                delete [] filename;
                delete [] argv[0];
                delete [] argv[1];
                for(int i = 0; i < 7; i++) delete [] env[i];
                delete [] argv;
                delete [] env;
                exit(7);
            } 
            else
            {
                pids.Add(pid);
                return 1;
            }
        } else
        {
            if(strlen(buf) > 5 && (fd = open(buf + 5, O_RDONLY)) < 0)
            {
                Send("404.html", "HTTP/1.0 404 PageNotFound");
                shutdown(sockfd, 1);
                close(sockfd);
                cerr << "Error: 404" << endl;
                return 404;
            }
            if(!strcmp(buf+5, "")) strcpy(buf + 5, "index.html");
            Send(buf + 5, "HTTP/1.0 200 MyServer");
            shutdown(sockfd, 1);
            close(sockfd);
        }
        return 0;
    }
    void Send(const char* file, const char* header)
    {
        int fileLength;
        char* strFileLength;
        fd = open(file, O_RDONLY);
        fileLength = GetLength(fd);
        strcpy(buf, header);
        strcat(buf, "\nAllow: GET\nServer: MyServer/0.1\nConnection: keep-alive\nContet-length: ");
        strFileLength = itoa(fileLength);
        strcat(buf, strFileLength);
        strcat(buf, "\n\n");
        len = strlen(buf);
        send(sockfd, &buf, len, 0);
        while((len = read(fd, buf, BUFLEN)) > 0)
        {
            send(sockfd, &buf, len, 0);
        }
        delete [] strFileLength;
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
    Client client;
    
    server.Bind();
    server.Listen();
    
    fd_set readset;
    timeval timeout;
    int activity, status;
    
    while(1)
    {
        FD_ZERO(&readset);
        FD_SET(server.getnum(), &readset);
        
        if(pids.size == 0)
        {
            activity = select(server.getnum() + 1, &readset, NULL, NULL, NULL);
        } else 
        {
            timeout.tv_sec = 0;
            timeout.tv_usec = 500000;
            activity = select(server.getnum() + 1, &readset, NULL, NULL, &timeout);
        }
        
        if(activity < 0)
        {
            cerr << "Error with select" << endl;
            exit(4);
        } 
        else if(activity > 0)
        {
            if(FD_ISSET(server.getnum(), &readset))
            {
                fcntl(client.get_socket(), F_SETFL, O_NONBLOCK);
                client.Accept(server.getnum());
                int req = client.Request(server.getport());
                if(req == 1) continue;
            }
        }
        
        for(int i = 0; i < pids.size; i++)
        {
            if(waitpid(pids[i], &status, WNOHANG))
            {
                if(WIFEXITED(status)) 
                {
                    char* str_pid = itoa(pids[i]);
                    char* filename = new char[strlen(str_pid) + 8];
                    strcpy(filename, "tmp");
                    strcat(filename, str_pid);
                    strcat(filename, ".txt");
                    client.Send(filename, "HTTP/1.0 200 MyServer");
                    remove(filename);
                    delete[] str_pid;
                    delete[] filename;
                } 
                else 
                {
                    client.Send("cgi.html", "HTTP/1.0 500 MyServer");
                }
                pids.Resize(pids[i]);
                shutdown(client.get_socket(), 1);
                close(client.get_socket());
            }
        }
    }
    return 0;
}
