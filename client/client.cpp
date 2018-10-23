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

#define BUFFER_LENGTH 1024  //maximum number of characters that can be received at once

//ip loopback address 127.0.0.1

using namespace std;

client::client(int port, string IpAdr) {
    this->port = port;

    cout << "Establishing Client..." << endl; 

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
int client::sendMess(string outLine) {                                     //TO DO: actually implement output corresponding to command and control string length
    if(send(socketNum, outLine.c_str(), outLine.length(), 0) == -1) {
        cerr << "ERROR: Failed to send message" << endl;
        return 1;
    }
    else {
        cout << "Message sent" << endl;
        return 0;
    }
}

int client::communicate() {
    string outLine;
    getline(cin, outLine);
    if((outLine.length() % BUFFER_LENGTH) == 0) {        //makes sure there is no '.' right at the start of the next package
        cout << outLine.length() << endl;
        outLine += ' ';
        cout << outLine.length() << endl;
    }
    

    if(outLine == "QUIT" || outLine == "quit") {
        return 1;
    }
    else {
        if(sendMess(outLine) == 1) {
            return 2;
        }
        else {
            return 0;
        }
    }
}



/*                                                     //TO DO: implement this wrapper for larger messages?
int sendall(int sd, char *buf, int *len) {
    int total = 0; // how many bytes we’ve sent
    int bytesleft = *len; // how many we have left to send
    int n;
    while(total < *len) {
        Bachelor Informatik - Verteilte Systeme 24
        n = send(sd, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
    *len = total; // return number actually sent here
    return n==-1?-1:0; // return -1 on failure, 0 on success
}
*/