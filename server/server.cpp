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

static ssize_t my_read (int fd, char *ptr) {
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

int fileNameMax(string path) {
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

int fileCount(string path) {
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

mailServer::mailServer(int port, string pool) {
    this->port = port;
    this->poolPlace = pool;

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
    return true;
}

int mailServer::sendMess(string outLine) {     //actually sends the message 
    if(send(clientSocket, outLine.c_str(), outLine.length(), 0) == -1) {           //TO DO: test if works
        cerr << "ERROR: Failed to send message" << endl;
        return 1;
    }
    else {
        cout << "Message sent" << endl;
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

int mailServer::handleMess(string mess) {
    if (mess == "SEND") {
        cout << "SEND command received" << endl;
        gotSend();
    }
    else if (mess == "LIST") {
        cout << "LIST command received" << endl;
        gotList();
    }
    else if (mess == "READ") {
        cout << "READ command received" << endl;
        gotRead();
    }
    else if (mess == "DEL") {
        cout << "DEL command received" << endl;
        gotDel();
    }
    else {
        return 1;
    }
    
}

void mailServer::gotSend() {  //TO DO: split into smaller functions
    string contentPart = "";
    string contentFull;
    string fileName = "";
    int fileNum = 0;
    DIR *dp;
    struct dirent *dirp;
    bool recEx = false;

    string sender = receiveMess();
    cout << "Sender (max 8 chars): " << sender << endl;
    string recipient = receiveMess();
    cout << "Recipient (max 8 chars): " << recipient << endl;
    string subject = receiveMess();
    cout << "Subject (max 80 chars): " << subject << endl;
    cout << "Content (no max): ";
    while(contentPart != ".") {
        contentPart = receiveMess();
        cout << contentPart;
        if(contentPart != ".") {
            contentFull += contentPart + "\n";
        }
    }
    cout << endl << "Full content:" << endl << contentFull << endl;

    if((dp = opendir(poolPlace.c_str())) == NULL) {
        cerr << "ERROR: Failed to open mail pool directory" << endl;
        sendMess("ERR\n");
        return;
    }

    string fname;
    while ((dirp = readdir(dp)) != NULL) {
        fname = dirp->d_name;
        if(fname == recipient) {
            recEx = true;
        }
    }
    closedir(dp);

    if(!recEx) {
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
    ofstream outfile(fileName);
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

    string user = receiveMess();
    cout << "User to list: " << user << endl;
    string listPlace = poolPlace + '/' + user;
    highFile = fileNameMax(listPlace);
    numFiles = fileCount(listPlace);
    if(highFile == -1 || numFiles == -1) {
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
                string temp = "* " + line.substr(9)  + '\n';
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

    string user = receiveMess();
    cout << "User that wants to read file: " << user << endl;
    string readNum = receiveMess();
    cout << "File to read: " << readNum << endl;

    string fileName = poolPlace + '/' + user + '/' + readNum;
    ifstream opFile(fileName.c_str());
    for(int i = 0; getline(opFile, line); i++) {
        cout << line << endl;                               //!!! output to client
        //sendMess(line);
    }
    //sendMess(".\n");
    
    //Der Server antwortet bei korrekten Parametern mit OK
    //Komplettes file wird an client geschickt
    //Bei Fehlern schick ERR
}

void mailServer::gotDel() {
    DIR *dp;
    struct dirent *dirp;

    string user = receiveMess();
    cout << "User that wants to delete file: " << user << endl;
    string exterm = receiveMess();
    cout << "File to be deleted: " << exterm << endl;
    string delPlace = poolPlace + '/' + user;

    if((dp = opendir(delPlace.c_str())) == NULL) {
        cerr << "ERROR: Failed to open mail pool directory" << endl;
        return;
    }

    string fname;
    while ((dirp = readdir(dp)) != NULL) {
        fname = dirp->d_name;
        if(fname == exterm) {
            string temp = delPlace + '/' + exterm;
            if(remove(temp.c_str()) != 0) {
                cerr << "ERROR: Failed to delete file" << endl;
                //Schick ERR an client
            }
            else {
                cout << "File " << exterm << " successfully removed" << endl;
            }
        }
    }
    closedir(dp);
}