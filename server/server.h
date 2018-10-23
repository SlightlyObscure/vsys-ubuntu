#include <sys/socket.h> //socket(), bind()
#include <netinet/in.h> //struct sockaddr_in (see vsys_teil2 page 11)
#include <arpa/inet.h> // inet_ntoa()
#include <unistd.h> //read(), write(), close()
#include <errno.h> //global var errno
#include <string>

using namespace std;

class mailServer {
private:
    string poolPlace;
    int port;
    int socketNum;
    int clientSocket;
    struct sockaddr_in address;
    socklen_t socketLength;
    

public:
    mailServer(int port, string pool);
    ~mailServer();
    bool acceptance();
    string receiveMess();
    int handleMess(string mess);
    void gotSend();
};