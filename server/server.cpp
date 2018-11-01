#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h> //socket(), bind()
#include <netinet/in.h> //struct sockaddr_in (see vsys_teil2 page 11)
#include <arpa/inet.h> // inet_ntoa()
#include <unistd.h> //read(), write(), close()
#include <errno.h> //global var errno
#include <ctime>  //get time for email file names
#include <dirent.h>
#include <sys/stat.h> //for making directories
#include <fstream> //for making, reading and writing to files
#include <sstream> //string stream


#include "server.h"

#define BUFFER_LENGTH 1024   //maximum number of characters that can be received at once

using namespace std;

static ssize_t my_read (int fd, char *ptr) { //reads messages from socket
	static int   read_cnt = 0 ; 
	static char  *read_ptr ; 
	static char  read_buf[BUFFER_LENGTH]; 
	if (read_cnt <= 0) { 
		again: 
		if ( (read_cnt = read(fd,read_buf,sizeof(read_buf))) < 0) { 
			if (errno == EINTR) 
				goto again ; 
			return (-1) ; 
		} else if (read_cnt == 0) 
		return (0) ; 
		read_ptr = read_buf ; 
	} ; 
	read_cnt-- ; 
	*ptr = *read_ptr++ ; 
	return (1) ; 
}

ssize_t readline (int fd, void *vptr, size_t maxlen) { // Returns read buffer until _next_ character is a newline
	ssize_t   n, rc ; 
	char      c, *ptr ; 
	ptr = (char*)vptr ; 
	for (n = 1 ; n < maxlen ; n++) { 
		if ( (rc = my_read(fd,&c)) == 1 ) { 
			if (c == '\n')
				break; // Newline is not included in the returned string!
			*ptr++ = c ; 
		} else if (rc == 0) { 
			if (n == 1) 
		         return (0) ;         // EOF, no data read 
		     else 
		         break ;              // EOF, some data was read 
		 } else 
		       return (-1) ;          // error, errno set by read() in my_read() 
		   } ; 
	*ptr = 0 ;                   // null terminate 
	return (n) ; 
} 

int fileNameMax(string path) { //returns the name of the mail file with the largest number as its name
    DIR *dp;
    struct dirent *dirp;
    int fileNum = 0;
    int curFileNum = 0;
    string fname;

    if((dp = opendir(path.c_str())) == NULL) {
        cerr << "ERROR: Failed to open directory" << endl;
        return -1;
    }
    while ((dirp = readdir(dp)) != NULL) {
        fname = dirp->d_name;
        stringstream s(fname);
        s >> curFileNum;
        if(curFileNum > fileNum) {
            fileNum = curFileNum;
        }
    }
    closedir(dp);
    return fileNum;
}

int fileCount(string path) { //returns number of files in a folder
    DIR *dp;
    struct dirent *dirp;
    int count = -2;

    if((dp = opendir(path.c_str())) == NULL) {
        cerr << "ERROR: Failed to open directory" << endl;
        return -1;
    }
    while ((dirp = readdir(dp)) != NULL) {
        count++;
    }
    closedir(dp);
    return count;

}

mailServer::mailServer(int port, string pool, string block) {
    this->port = port;
    this->poolPlace = pool;
    this->blockEntry = block;

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
        string IPad = inet_ntoa(address.sin_addr);
        cout << "Connection accepted: " << IPad << endl;
    }
    else {
        cerr << "Client cast out (failed to accept client)" << endl;
        
    }
    return true;
}

int mailServer::sendMess(string outLine) {     //actually sends the message 
    if(send(clientSocket, outLine.c_str(), outLine.length(), 0) == -1) {  //data is sent after each \n or if buffer is full
        cerr << "ERROR: Failed to send message" << endl;
        return 1;
    }
    else {
        //cout << "Message sent" << endl;
        //cout << "Sent: " << outLine;
        return 0;
    }
}

string mailServer::receiveMess() {
    char mess[BUFFER_LENGTH] = "";
    int len = 0;
    len = readline(clientSocket, mess, BUFFER_LENGTH-1);
    if(len == -1) {
        throw 1;
    }
    else if(len == 0) {
        throw 2;
    }
    else {
        mess[len] = '\0';
    }
    string toStr(mess);
    return toStr;
}

int mailServer::handleMess(string mess) {   //reaction to command sent by client
    if(username == "") {
        if (mess == "LOGIN" || mess == "login") {
            cout << "LOGIN command received" << endl;
            gotLogin();
        }
        else {
            cerr << "ERROR: Invalid command received. Only LOGIN and QUIT commands are valid before login" << endl;
            return 2;
        }
    }
    else {
        if (mess == "SEND" || mess == "send") {
            cout << "SEND command received" << endl;
            gotSend();
        }
        else if (mess == "LIST" || mess == "list") {
            cout << "LIST command received" << endl;
            gotList();
        }
        else if (mess == "READ" || mess == "read") {
            cout << "READ command received" << endl;
            gotRead();
        }
        else if (mess == "DEL" || mess == "del") {
            cout << "DEL command received" << endl;
            gotDel();
        }
        else {
            cerr << "ERROR: Invalid command received" << endl;
            return 1;
        }
    }
}

void mailServer::scribe(string IPad) {
    string searchName, fileName;
    bool foundThing = false;
    DIR *dp;
    struct dirent *dirp;

    if((dp = opendir(blockEntry.c_str())) == NULL) { //try to open blocked IP directory
        cerr << "ERROR: Failed to open blocked IP directory" << endl;
        return;
    }

    while ((dirp = readdir(dp)) != NULL) {  //look for file with IPad as name
        searchName = dirp->d_name;
        if(searchName == IPad) {
            foundThing = true;
        }
    }
    closedir(dp);

    if(foundThing) {
        //TO DO: edit file
    }
    else {
        fileName = blockEntry + '/' + IPad;
        ofstream outfile(fileName);
        outfile << "1" << endl;
        outfile.close();
    }
}

bool mailServer::bouncer(string IPad) {         //TO DO: block after 3 failed logins for x seconds
    string searchName;
    bool foundThing = false;
    DIR *dp;
    struct dirent *dirp;

    if((dp = opendir(blockEntry.c_str())) == NULL) { //try to open blocked IP directory
        cerr << "ERROR: Failed to open blocked IP directory" << endl;
        return false;
    }

    while ((dirp = readdir(dp)) != NULL) {  //look for file with IPad as name
        searchName = dirp->d_name;
        if(searchName == IPad) {
            foundThing = true;
        }
    }
    closedir(dp);

    if(foundThing) {
        return false;   //TO DO: check file content
    }
    else {
       return true; 
    }

    //return true;
}

void mailServer::gotLogin() {
    string IPad = inet_ntoa(address.sin_addr);
    if(bouncer(IPad)) {
        sendMess("OK\n");
    }
    else {
        sendMess("ERR\n");
    }

    string entUser = receiveMess(); //receive username
    cout << "Username (max 8 chars): " << entUser << endl;
    string entPass = receiveMess();
    cout << "Password (max 42 chars): " << entPass << endl;

    if (entPass == "friend") {  //TO DO: LDAP
        username = entUser;
        sendMess("OK\n");
    }
    else {
        sendMess("ERR\n");      //TO DO: write into blacklist
        //scribe(IPad);
    }

}

void mailServer::gotSend() {       //TO DO: split into smaller functions
    string contentPart = "";
    string contentFull;
    string fileName = "";
    int fileNum = 1;
    DIR *dp;
    struct dirent *dirp;
    bool recEx = false;

    //string sender = receiveMess();  //receive sender
    string sender = username;

    cout << "Sender (max 8 chars): " << sender << endl;
    string recipient = receiveMess();   //receive recipient
    cout << "Recipient (max 8 chars): " << recipient << endl;
    string subject = receiveMess(); //receive message subject
    cout << "Subject (max 80 chars): " << subject << endl;
    cout << "Content (no max): ";   //receive content until "." is sent
    while(contentPart != ".") {
        contentPart = receiveMess();
        cout << contentPart;
        if(contentPart != ".") {
            contentFull += contentPart + "\n";
        }
    }
    cout << endl << "Full content:" << endl << contentFull << endl;

    if((dp = opendir(poolPlace.c_str())) == NULL) { //try to open mails pool directory
        cerr << "ERROR: Failed to open mail pool directory" << endl;
        sendMess("ERR\n");
        return;
    }

    string fname;
    while ((dirp = readdir(dp)) != NULL) {  //look for folder with name of recipient
        fname = dirp->d_name;
        if(fname == recipient) {
            recEx = true;
        }
    }
    closedir(dp);

    if(!recEx) {    //folder with name of recipient doesn't exist yet -> make one
        string path = poolPlace + '/' + recipient;
        mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    else {
        string recPlace = poolPlace + '/' + recipient;
        fileNum = fileNameMax(recPlace);
        if(fileNum < 0) {
            return;
        }
        fileNum++;
    }
    fileName = poolPlace + '/' + recipient + '/' + to_string(fileNum);
    ofstream outfile(fileName);             //make file for mail -> write content to file
    outfile << "Sender: " << sender << endl;
    outfile << "Subject: " << subject << endl;
    outfile << "Content: " << endl << contentFull;
    outfile.close();

    sendMess("OK\n");

}

void mailServer::gotList() {
    DIR *dp;
    struct dirent *dirp;
    int highFile, numFiles;
    string line;

    //string user = receiveMess();
    string user = username;

    cout << "User to list: " << user << endl;
    string listPlace = poolPlace + '/' + user;
    highFile = fileNameMax(listPlace);  //number of latest mail
    numFiles = fileCount(listPlace);    //number of files in folder
    if(highFile == -1 || numFiles == -1) {  //unable to get highFile/numFiles
        cerr << "Warning: Couldn't open user's directory. Maybe it doesn't exist?" << endl;
        numFiles = 0;
    }

    string mess = "Number of messages: " + to_string(numFiles) + '\n';
    cout << mess << endl; 
    sendMess(mess);

    for( ;highFile > 0; highFile--) {
        string fileName = poolPlace + '/' + user + '/' + to_string(highFile);
        ifstream opFile(fileName.c_str());
        for(int i = 0; getline(opFile, line); i++) {
            if(i==1) {
                string temp = to_string(highFile) + ": " + line.substr(9)  + '\n';
                cout << temp; 
                sendMess(temp);
            }
        }
    }
    sendMess(".\n");
}

void mailServer::gotRead() {
    DIR *dp;
    struct dirent *dirp;
    string line;
    bool subsequent = false;

    //string user = receiveMess();
    string user = username;

    cout << "User that wants to read file: " << user << endl;
    string readNum = receiveMess();
    cout << "File to read: " << readNum << endl;

    string fileName = poolPlace + '/' + user + '/' + readNum;
    ifstream opFile(fileName.c_str());
    for(int i = 0; getline(opFile, line); i++) {
        if(subsequent == false) {
            subsequent = true;
            sendMess("OK\n");
        }
        cout << line << endl;
        sendMess(line+ '\n');
    }
    if(subsequent == false) {
        sendMess("ERR\n");
    }
    else {
        sendMess(".\n");
    }
    
    
    //Der Server antwortet bei korrekten Parametern mit OK
    //Komplettes file wird an client geschickt
    //Bei Fehlern schick ERR
}

void mailServer::gotDel() {
    DIR *dp;
    struct dirent *dirp;
    bool fileFound = false;

    //string user = receiveMess();
    string user = username;
    
    cout << "User that wants to delete file: " << user << endl;
    string exterm = receiveMess();
    cout << "File to be deleted: " << exterm << endl;
    string delPlace = poolPlace + '/' + user;

    if((dp = opendir(delPlace.c_str())) == NULL) {
        cerr << "ERROR: Failed to open mail pool directory" << endl;
        sendMess("ERR\n");
        return;
    }

    string fname;
    while ((dirp = readdir(dp)) != NULL) {
        fname = dirp->d_name;
        if(fname == exterm) {
            fileFound = true;
            string temp = delPlace + '/' + exterm;
            if(remove(temp.c_str()) != 0) {
                cerr << "ERROR: Failed to delete file" << endl;
                sendMess("ERR\n");
            }
            else {
                cout << "File " << exterm << " successfully removed" << endl;
                sendMess("OK\n");
            }
        }
    }
    if(fileFound == false) {
        cerr << "ERROR: File not found" << endl;
        sendMess("ERR\n");
    }
    closedir(dp);
}

void mailServer::setUsername(string name) {
    username = name;
}