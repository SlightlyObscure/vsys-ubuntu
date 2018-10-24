#include <string.h>
#include <string>
#include <sys/socket.h> //socket(), bind()
#include <netinet/in.h> //struct sockaddr_in (see vsys_teil2 page 11)
#include <arpa/inet.h> // inet_ntoa()
#include <unistd.h> //read(), write(), close()
#include <errno.h> //global var errno

using namespace std;

class client {
private:
    int socketNum;
    struct sockaddr_in address;
    int port;
public:
    client(int port, string IpAdr);
    ~client();
    int communicate();
    int sendMess(string outLine);
    void sendMessContent();
};