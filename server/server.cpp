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

mailServer::mailServer() {
    struct sockaddr_in address;
    cout << "Testing server constructor" << endl; 
    socketNum = socket(AF_INET, SOCK_STREAM, 0);
    cout << "socketNum = " << socketNum << endl;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET; //IPv4
    address.sin_port = htons(6000); //Port nummer 6000
    address.sin_addr.s_addr = htonl (INADDR_ANY); //eigene IP Adresse
    if(bind(socketNum, (struct sockaddr *) &address, sizeof (address))!=0){
        cerr << "Unable to bind socket." << endl;
        exit(1);
    }
}

mailServer::~mailServer() {
    if(socketNum>0) {
        close(socketNum);
        cout << "Socket closed" << endl;
    }
}