#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h> //socket(), bind()
#include <netinet/in.h> //struct sockaddr_in (see vsys_teil2 page 11)
#include <arpa/inet.h> // inet_ntoa()
#include <unistd.h> //read(), write(), close()
#include <errno.h> //global var errno

#include "client.h"

//ip loopback address 127.0.0.1

using namespace std;

client::client(int port, string IpAdr) {
    this->port = port;

    cout << "Testing client constructor" << endl; 

    socketNum = socket(AF_INET, SOCK_STREAM, 0);

    memset(&address, 0, sizeof(address)); //Sicherstellen dass Adresse 0 ist
    address.sin_family = AF_INET; //IPv4
    address.sin_port = htons(port); //Port nummer <port>

    inet_aton(IpAdr.c_str(), &address.sin_addr);

    cout << "Connecting to server with IP: " << IpAdr << "; Port: " << port << endl;

    if(connect(socketNum, (struct sockaddr *) &address, sizeof(address)) == 0) {
        cout << "Connection established" << endl;
    }
    else {
        cerr << "ERROR: Failed to connect. Error #" << errno << endl;
        exit(1);
    }
}
client::~client () {
	if(close(socketNum) == -1) {
        cerr << "ERROR: Unable to close socket. Error #" << errno << endl;
    }
    else {
            cout << "Socket closed" << endl;
    }
}




