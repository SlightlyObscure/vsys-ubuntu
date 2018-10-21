#include <iostream>
#include <sstream>
#include <string.h>
#include <dirent.h>

#include <unistd.h>

#include "server.h"

using namespace std;


int main(int argc, char* argv[]) {
    int port;

    if(argc != 3) {
        cerr << "ERROR: Incorrect number of arguments" << endl;
        cout << "Correct Usage: ./server <port number> <path of mailspool directory>" << endl;
        exit(1);
    }
    stringstream s(argv[1]);
    s >> port;
    
    if((port == 0 && strcmp(argv[1], "0") != 0) || port < 0 || port > 65535){
        cerr << "ERROR: Invalid port number" << endl;
        cout << "Correct Usage: ./server <port number> <path of mailspool directory>" << endl;
        exit(1);
    }
    else if(opendir(argv[2]) == NULL) {
        cerr << "ERROR: Invalid path of mailspool directory" << endl;
        cout << "Correct Usage: ./server <port number> <path of mailspool directory>" << endl;
        exit(1);
    }


    cout << "Hello World!" << endl;
    mailServer* serf = new mailServer (port);
    usleep(60000);
    cout << "test" << endl;
    string fakeNews;
    cin >> fakeNews;
    delete serf;
}