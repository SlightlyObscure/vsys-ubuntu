#include <string.h>
#include <string>
#include <sys/socket.h> //socket(), bind()
#include <netinet/in.h> //struct sockaddr_in (see vsys_teil2 page 11)
#include <arpa/inet.h> // inet_ntoa()
#include <unistd.h> //read(), write(), close()
#include <errno.h> //global var errno

using namespace std;

ssize_t readline (int fd, void *vptr, size_t maxlen);
static ssize_t my_read (int fd, char *ptr);

class client {
private:
    int socketNum;
    int serverSpeekSocket;
    struct sockaddr_in address;
    int port;
public:
    client(int port, string IpAdr);
    ~client();
    int communicate();
    int sendMess(string outLine);
    string receiveMess();
    void execSend();
    void execRead();
    void execList();
    void execDel();
};