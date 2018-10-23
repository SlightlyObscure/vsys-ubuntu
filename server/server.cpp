#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
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

bool mailServer::acceptance() {                                      //TO DO: maybe different returns
    socketLength = sizeof(address);
    int accSockNum = accept(socketNum, (struct sockaddr *) &address, &socketLength);
    if(accSockNum > 0) {
        cout << "Client accepted with socket descriptor: " << accSockNum << endl;
        clientSocket = accSockNum;
    }
    else {
        cerr << "Client cast out (failed to accept client)" << endl;
        
    }
    return 0;
}

string mailServer::receiveMess() { 
    int buffLen = 100;       //TO DO: change this
    char mess[buffLen] = "";
    int len = 0;
    len = recv(clientSocket, mess, buffLen-1, 0);
    if(len == -1) {
        //cerr << "ERROR: Failed to receive message" << endl;
    }
    else {
        mess[len] = '\0';
        cout << "Message received: " << mess << endl; 
    }
    string toStr(mess);
    return toStr;


    /*size = readline (my_socket, buffer, BUF - 1);  //TO DO: work on this

	if (size > 0)
	{
		buffer[size] = '\0';

		return std::string(buffer);
	}*/
}