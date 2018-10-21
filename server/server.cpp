#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h> //socket(), bind()
#include <netinet/in.h> //struct sockaddr_in (see vsys_teil2 page 11)
#include <arpa/inet.h> // inet_ntoa()
#include <unistd.h> //read(), write(), close()
#include <errno.h> //global var errno

#include "server.h"

using namespace std;

mailServer::mailServer(int port) {
    this->port = port;

    cout << "Testing server constructor" << endl; 

    socketNum = socket(AF_INET, SOCK_STREAM, 0);

    memset(&address, 0, sizeof(address)); //Sicherstellen dass Adresse 0 ist
    address.sin_family = AF_INET; //IPv4
    address.sin_port = htons(port); //Port nummer <port>
    address.sin_addr.s_addr = htonl (INADDR_ANY); //eigene IP Adresse

    if(bind(socketNum, (struct sockaddr *) &address, sizeof (address))!=0){
        cerr << "ERROR: Unable to bind socket. Error #" << errno << endl;
        exit(1);
    }

    listen(socketNum, 7);
}

mailServer::~mailServer() {
    if(close(socketNum) == -1) {
        cerr << "ERROR: Unable to close socket. Error #" << errno << endl;
    }
    else {
            cout << "Socket closed" << endl;
    }
}