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
    int port;


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

    mailServer* serf = new mailServer (port);

    pid_t procID; //process ID

    
    //Prefork so that up to CLIENT_CAP clients can use the server at once 
    /*for (int i = 0; i < CLIENT_CAP; i++) {
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
    bool gotLiege = false;

    while(1) {
        if(!gotLiege) {
            serf->acceptance();
            gotLiege = true;
        }
        cin >> fakeNews;
        if((fakeNews = serf->receiveMess()) != "") {
            cout << fakeNews << endl;
        }
        
        /*cin >> fakeNews;
        if(fakeNews == "quit"){
            break;
        } */   
    }

    delete serf;

    return 0;
}