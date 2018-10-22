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

    cout << "Constructing server..." << endl; 

    socketNum = socket(AF_INET, SOCK_STREAM, 0);

    memset(&address, 0, sizeof(address)); //Sicherstellen dass Adresse 0 ist
    address.sin_family = AF_INET; //IPv4
    address.sin_port = htons(port); //Port nummer <port>
    address.sin_addr.s_addr = htonl (INADDR_ANY); //eigene IP Adresse

    if(bind(socketNum, (struct sockaddr *) &address, sizeof (address))!=0){
        cerr << "ERROR: Unable to bind socket. Error #" << errno << endl;
        exit(1);
    }
    else {
        cout << "Server successfully bound to socket" << endl; 
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

bool mailServer::acceptance() { 
    socketLength = sizeof(address);
    int accSockNum = accept(socketNum, (struct sockaddr *) &address, &socketLength);
    if(accSockNum > 0) {
        cout << "Client accepted with socket descriptor: " << accSockNum << endl;
        
    }
    else {
        cerr << "Client cast out (failed to accept client)" << endl;
        
    }
    return 0;
}