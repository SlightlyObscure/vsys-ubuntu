#include <sys/socket.h> //socket(), bind()
#include <netinet/in.h> //struct sockaddr_in (see vsys_teil2 page 11)
#include <arpa/inet.h> // inet_ntoa()
#include <unistd.h> //read(), write(), close()
#include <errno.h> //global var errno

class mailServer {
private:
    int socketNum;
    struct sockaddr_in address;
    int port;
    socklen_t socketLength;

public:
    mailServer(int port);
    ~mailServer();
    bool acceptance();
    bool receive();
};