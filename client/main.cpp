#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <regex>

#include "client.h"

using namespace std;

void usage() {
    cout << "Correct Usage: ./server <port number> <ip address of server>" << endl;
    return;
}



int main(int argc, char* argv[]) {
    regex valID ("(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])");
    int port;
    string IP(argv[2]);
    bool running = true;

    if(argc != 3) {
        cerr << "ERROR: Incorrect number of arguments" << endl;
        usage();
        exit(1);
    }
    stringstream s(argv[1]);
    s >> port;
    
    if((port == 0 && strcmp(argv[1], "0") != 0) || port < 0 || port > 65535){
        cerr << "ERROR: Invalid port number" << endl;
        usage();
        exit(1);
    }
    else if(!regex_match(IP, valID)) {                                                       
        cerr << "ERROR: Invalid IP address" << endl;
        usage();
        exit(1);
    }
    string IpAdr(argv[2]);                        //TO DO: move elsewhere

    client* liege = new client (port, IpAdr);
    while(running) {
        if(liege->communicate() != 0) {
            running = false;                    //TO DO: maybe different handling for different return values?
        }
    }

    delete liege;
}