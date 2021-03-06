#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <regex>

#include "client.h"

using namespace std;

void usage() {      //correct input usage 
    cout << "Correct Usage: ./server <port number> <ip address of server>" << endl;
    return;
}



int main(int argc, char* argv[]) {  //below is regex for valid IP address (apparently this noticably slows down make)
    regex valID ("(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])");
    int port;
    string IP(argv[2]);
    bool running = true;

    if(argc != 3) {     //wrong number of arguments
        cerr << "ERROR: Incorrect number of arguments" << endl;
        usage();
        exit(1);
    }
    stringstream s(argv[1]);
    s >> port;
    
    if((port == 0 && strcmp(argv[1], "0") != 0) || port < 0 || port > 65535){   //checking for correct portnumber
        cerr << "ERROR: Invalid port number" << endl;
        usage();
        exit(1);
    }
    else if(!regex_match(IP, valID)) {             //checking for ip address                                          
        cerr << "ERROR: Invalid IP address" << endl;
        usage();
        exit(1);
    }
    string IpAdr(argv[2]);

    client* liege = new client (port, IpAdr);
    int fault;
    try {
        while(running) {
            if((fault = liege->communicate()) != 0) {
                running = false;                    
                if(fault == 1) {
                    cout << "Client quitting..." << endl;
                }
                else if(fault == 2) {
                    cerr << "ERROR: IP address rejected by server. This may be due to too many login attempts or a server error. Wait a few minutes for more login attempts" << endl;
                }
            }
        }
    }
    catch (int e) {
        cerr << "ERROR: Something bad happened. Error #" << errno << endl;
    }

    delete liege;
}