#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <sys/socket.h> //socket(), bind()
#include <netinet/in.h> //struct sockaddr_in (see vsys_teil2 page 11)
#include <arpa/inet.h> // inet_ntoa()
#include <unistd.h> //read(), write(), close()
#include <errno.h> //global var errno

#include "client.h"

#define BUFFER_LENGTH 1024  //maximum number of characters that can be received at once

//ip loopback address 127.0.0.1

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

client::client(int port, string IpAdr) {
    string inLine;
    this->port = port;

    cout << "Establishing Client..." << endl; 

    socketNum = socket(AF_INET, SOCK_STREAM, 0);

    memset(&address, 0, sizeof(address)); //Sicherstellen dass Adresse 0 ist
    address.sin_family = AF_INET; //IPv4
    address.sin_port = htons(port); //Port Nummer <port>

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
	if(close(socketNum) == -1) {    //try to close socket 
        cerr << "ERROR: Unable to close socket. Error #" << errno << endl;
    }
    else {
        cout << "Socket closed" << endl;
    }
}


int client::sendMess(string outLine) {     //actually sends the message
    outLine += '\n';    //data is sent after each \n or if buffer is full
    if(send(socketNum, outLine.c_str(), outLine.length(), 0) == -1) {
        cerr << "ERROR: Failed to send message" << endl;
        return 1;
    }
    else {
        //cout << "Message sent" << endl;
        return 0;
    }
}

string client::receiveMess() {
    char mess[BUFFER_LENGTH] = "";
    int len = 0;
    len = readline(socketNum, mess, BUFFER_LENGTH-1); 
    if(len == -1) {     //client (probably) closed socket
        throw 1;
    }
    else if(len == 0) {     //other errors
        throw 2;
    }
    else {
        mess[len] = '\0';
    }
    string toStr(mess);
    return toStr;
}

int client::communicate() {
    string outLine;

    cout << "Command: ";
    getline(cin, outLine);

    if(outLine == "QUIT" || outLine == "quit") {    //quitting out of connection
        return 1;
    }
    else if (username == "") {
        if(outLine == "LOGIN" || outLine == "login" ){    //if a successful login hasn't happened only LOGIN and QUIT are valid
            if(sendMess(outLine) == 1) {
                cerr << "LOGIN command unsuccessful" << endl;
            }
            else if(execLogin()!=0) {
                return 2;
            }
        }
        else {
            cout << "ERROR: Invalid command" << endl;
            cout << "Valid Commands until login: LOGIN, QUIT" << endl;
        }
    }
    else {
        if(outLine == "SEND" || outLine == "send" ){    //each valid command leads to a different function; once completed the loop in main.cpp returns to here
            if(sendMess(outLine) == 1) {
                cerr << "SEND command unsuccessful" << endl;
            }
            execSend(); 
        } 
        else if(outLine == "LIST" || outLine == "list" ) {
            if(sendMess(outLine) == 1) {
                cerr << "LIST command unsuccessful" << endl;
            };
            execList();
        } 
        else if (outLine == "READ" || outLine == "read" ) {
            if(sendMess(outLine) == 1) {
                cerr << "READ command unsuccessful" << endl;
            }
            execRead();
        } 
        else if (outLine == "DEL" || outLine == "del" ) {
            if(sendMess(outLine) == 1) {
                cerr << "DEL command unsuccessful" << endl;
            }
            execDel();
        }
        else {
            cout << "ERROR: Invalid command" << endl;
            cout << "Valid Commands: SEND, LIST, READ, DEL, QUIT" << endl;
        }
    }
    
    return 0;
}

int client::execLogin(){
    string inLine, outLine, tempUse;
    int prog = 0;

    inLine = receiveMess();  
    cout << inLine << endl;
    if(inLine == "ERR") {
        return 1;
    }

    //enter username
    while(prog==0){
        cout <<  "[LOGIN] LDAP Username (max 8 chars): ";
        getline(cin, outLine);
        if(outLine.length()<= 8 && outLine.length()!=0){
            if(sendMess(outLine) == 0) {
                tempUse = outLine;
                prog++;
            }
            else {
                cerr << "ERROR: Failed to send message" << endl;
            }
        }
        else {
            cout << "[LOGIN] Too Long..." << endl;
        }
    }

    while(prog==1){
        //cout <<  "[LOGIN] LDAP Password (max 42 chars): ";
        //getline(cin, outLine);
        //lese passwort ein und konvertier von c_string zu string
        outLine = string(getpass("[LOGIN] LDAP Password (max 42 chars): "));
        if(outLine.length()<= 42 && outLine.length()!=0){
            if(sendMess(outLine) == 0) {
                prog++;
            }
            else {
                cerr << "ERROR: Failed to send message" << endl;
            }
        }
        else {
            cout << "[LOGIN] Too Long..." << endl;
        }
    }

    //receive OK or ERR from server
    inLine = receiveMess();  
    cout << inLine << endl;
    if(inLine == "OK") {
        username = tempUse;
    }

    return 0;
}

void client::execSend(){ // sending the right information with the right amount of characters
    string outLine;
    int prog = 0;

    //checking every input with a loop, until they get it right!!

    //sending sender
    /*while(prog==0){
        cout <<  "[SEND] Sender (max 8 chars): ";
        getline(cin, outLine);
        if(outLine.length()<= 8 && outLine.length()!=0){
            if(sendMess(outLine)==0) {      //everything went well -> procede to next loop
                prog++;
            }
            else {
                cerr << "ERROR: Failed to send message" << endl;
            }
        }
        else {
            cout << "[SEND] Too Long..." << endl;   //entered string was too long
        }
    }*/

    //sending recipient
    while(prog==0){
        cout << "[SEND] Recipient (max 8 chars): "; 
        getline(cin, outLine);                          
        if(outLine.length()<= 8 && outLine.length()!=0){
            if(sendMess(outLine)==0) {
                prog++;
            }
            else {
                cerr << "ERROR: Failed to send message" << endl;
            }
        } 
        else {
            cout << "[SEND] Too Long..." << endl; 
        }
    }

    //sending subject
    while(prog==1){
        cout << "[SEND] Subject (max 80 chars): ";
        getline(cin, outLine);
        if(outLine.length()<= 80 && outLine.length()!=0){
            if(sendMess(outLine)==0) {
                prog++;
            }
            else {
                cerr << "ERROR: Failed to send message" << endl;
            }
        } 
        else {
            cout << "[SEND] Too Long..." << endl;
        }
    }

    //sending message content
    cout << "[SEND] Message Content (no limit; send with input '.\\n'): " << endl;
    while(prog==2){
        getline(cin, outLine);
        if(outLine.length()!=0){
            if(sendMess(outLine) != 0){
                cerr << "ERROR: Failed to send message" << endl;
            } 
            if(outLine == "."){
                prog++;
            }
        }
    }

    //receive OK or ERR from server
    string inLine = receiveMess();  
    cout << inLine << endl;

}

void client::execList(){ 
    string outLine, inLine;

    //enter username
    /*while(true){
        cout <<  "[LIST] Username (max 8 chars): ";
        getline(cin, outLine);
        if(outLine.length()<= 8 && outLine.length()!=0){
            if(sendMess(outLine) == 0) {
                break;
            }
            else {
                cerr << "ERROR: Failed to send message" << endl;
            }
        }
        else {
            cout << "[LIST] Too Long..." << endl;
        }
    }*/
    
    //print list
    while(inLine!=".") {        //read from socket until server sends "."
        inLine = receiveMess();
        if(inLine!=".") {
            cout << inLine << endl;
        }
    }
}


void client::execRead(){
    string outLine, inLine;
    int prog = 0;
    int messNum;

    //input username
    /*while(prog==0){ 
        cout <<  "[READ] Username (max 8 chars): ";
        getline(cin, outLine);
        if(outLine.length()<= 8 && outLine.length()!=0){
            if(sendMess(outLine) == 0 ){
                prog++;
            }
            else {
                cerr << "ERROR: Failed to send message" << endl;
            }
        }
        else {
            cout << "[READ] Too Long..." << endl;
        }
    }*/

    //input number of the message; numbers are displayed via LIST
    while(prog==0) {
        cout <<  "[READ] Input Message Number: ";
        getline(cin, outLine);
        stringstream s(outLine);
        s >> messNum;
        if(messNum<=0) {
            cerr << "ERROR: Message number must be an integer greater than 0" << endl;
        }
        else {
            if(sendMess(outLine) == 0) {
                prog++;
            }
            else {
                cerr << "ERROR: Failed to send message" << endl;
            }
        }
    }

    //receive OK or ERR from server
    inLine = receiveMess();
    cout << inLine << endl;
    //if message was successfully found -> read from socket until server sends "."
    if(inLine == "OK") {
        while(inLine!=".") {
            inLine = receiveMess();
            if(inLine!=".") {
                cout << "   " << inLine << endl;
            }
        }
    }
}


void client::execDel(){
    string outLine, inLine;
    int prog = 0;
    int messNum;

    //input username
    /*while(prog==0){
        cout <<  "[DEL] Username (max 8 chars): "<< endl;
        getline(cin, outLine);
        if(outLine.length()<= 8 && outLine.length()!=0){
            if(sendMess(outLine) == 0 ){
                prog++;
            }
            else {
                cerr << "ERROR: Failed to send message" << endl;
            }
        }
        else {
            cout << "[DEL] Too Long..." << endl;
        }
    }*/

    //input number of the message; numbers are displayed via LIST
    while(prog==0) {
        cout <<  "[DEL] Input Message Number: ";
        getline(cin, outLine);
        stringstream s(outLine);
        s >> messNum;
        if(messNum<=0) {
            cerr << "ERROR: Message number must be an integer greater than 0" << endl;
        }
        else {
            if(sendMess(outLine) == 0) {
                prog++;
            }
            else {
                cerr << "ERROR: Failed to send message" << endl;
            }
        }
    }

    //receive OK or ERR from server
    inLine = receiveMess();
    cout << inLine << endl;

}


