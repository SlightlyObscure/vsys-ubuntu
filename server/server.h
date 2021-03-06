#include <sys/socket.h> //socket(), bind()
#include <netinet/in.h> //struct sockaddr_in (see vsys_teil2 page 11)
#include <arpa/inet.h> // inet_ntoa()
#include <unistd.h> //read(), write(), close()
#include <errno.h> //global var errno
#include <string>

using namespace std;

ssize_t readline (int fd, void *vptr, size_t maxlen);
static ssize_t my_read (int fd, char *ptr);
unsigned int getTime();

class mailServer {
private:
    string poolPlace;
    string blockEntry;
    int port;
    int socketNum;
    int clientSocket;
    struct sockaddr_in address;
    socklen_t socketLength;
    string username = "";

public:
    mailServer(int port, string pool, string block);
    ~mailServer();
    bool bouncer(string IPad);
    void scribe(string IPad);
    bool acceptance();
    int sendMess(string outLine);
    string receiveMess();
    int handleMess(string mess);
    void gotLogin();
    void gotSend();
    void gotList();
    void gotRead();
    void gotDel();
    void setUsername(string name);
};