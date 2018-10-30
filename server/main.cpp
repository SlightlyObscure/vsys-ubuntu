#include <iostream>
#include <sstream>
#include <string.h>
#include <dirent.h>

#include "server.h"

#define CLIENT_CAP 7

using namespace std;

void usage() {
    cout << "Correct Usage: ./server <port number> <path of mailspool directory>" << endl;
    return;
}


int main(int argc, char* argv[]) {
    int port;                        //port of server
    string poolPlace(argv[2]);      //directory of mailspool

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
    else if(opendir(argv[2]) == NULL) {                                    
        cerr << "ERROR: Invalid path of mailspool directory" << endl;
        usage();
        exit(1);
    }

    mailServer* serf = new mailServer (port, poolPlace);

    /*pid_t procID; //process ID
    
    //Prefork so that up to CLIENT_CAP clients can use the server at once 
    for (int i = 0; i < CLIENT_CAP; i++) {
        procID = fork();
        if(procID == -1) {  //failed to fork
            exit(1);
        }
        else if(procID == 0) { //this is a child process -> stop forking
            break;
        }
        else {
            cout << "Is it someone new? Process ID: " << procID << endl;
        }
    }
    if(procID == 0) { //child process
        while(serf->acceptance()) {

        }
    }
    else {
        string fakeNews; //temporary replacement for waitpid
        cin >> fakeNews;
        delete serf;
    }*/

    string fakeNews = "";

    while(serf->acceptance()) {          //server accepts connection and starts receiving data
        try {
            while (7) {
                fakeNews = serf->receiveMess();
                if(fakeNews != "") {
                    //cout << fakeNews << endl;
                    serf->handleMess(fakeNews);
                }
            }   
        }
        catch (int e) {
            if(e==1) {
                cerr << "ERROR: Unexpected connection error. Error #" << errno << endl;
            }
            else if(e==2) {
                cerr << "Warning: Connection to client lost. Error #" << errno << endl;
            }
        }
    }

    delete serf;

    return 0;
}