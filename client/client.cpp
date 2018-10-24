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
int client::sendMess(string outLine) {     //actually sends the message
    if(send(socketNum, outLine.c_str(), outLine.length(), 0) == -1) {
        cerr << "ERROR: Failed to send message" << endl;
        return 1;
    }
    else {
        cout << "Message sent" << endl;
        return 0;
    }
}

string client::receiveMess() {                 //TO DO: test if works
    char mess[BUFFER_LENGTH] = "";
    int len = 0;
    len = readline(socketNum, mess, BUFFER_LENGTH-1);
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


void client::execSend(){ // sending the right information with the right amount of characters, error handling missing
    string outLine;

    //checking every input with a loop, until they get it right!!

    //sending sender
    while(true){
    cout <<  "Sender (max 8 chars): "<< endl;
    getline(cin, outLine);
    if(outLine.length()<= 8){
        outLine += '\n';
        sendMess(outLine);
        break;
    }
    else {
        cout << "Too Long..." << endl;
    }}

    //sending recipient
    while(true){
    cout << "Recipient (max 8 chars): "<< endl;
    getline(cin, outLine);
    if(outLine.length()<= 8){
        outLine += '\n';
        sendMess(outLine);
        break;
    } else {
        cout << "Too Long..." << endl; 
    }
    }

    //sending subject
    while(true){
    cout << "Subject (max 80 chars): " << endl;
    getline(cin, outLine);
    if(outLine.length()<= 80){
        outLine += '\n';
        sendMess(outLine);
        break;
    } else {
        cout << "Too Long..." << endl;
    }
    }

    //sending message content
    cout << "Message Content (indefinite chars, has to end with a '.'): " << endl;
    while(true){
    getline(cin, outLine);
    outLine += '\n';
    if(sendMess(outLine) == 0){} 
    else {
        cerr << "ERROR: Failed to send message" << endl;
        execSend();    
    }
    if(outLine == ".\n"){
        break;
    }
    }
}

void client::execList(){ //TODO error handling missing 
    string outLine;

    while(true){
        cout <<  "[LIST] Username (max 8 chars): "<< endl;
        getline(cin, outLine);
        if(outLine.length()<= 8){
            outLine += '\n';
            sendMess(outLine);
        break;
    }
    else {
        cout << "Too Long..." << endl;
    }
    }

    //string test = receiveMess();
    //cout << test << endl;
    //server responds here with subjects
    //receive
}


void client::execRead(){
    string outLine;


    while(true){ //input username //TODO error handling missing
        cout <<  "[DEL] Username (max 8 chars): "<< endl;
        getline(cin, outLine);
        outLine += '\n';
        if(sendMess(outLine) == 0 ){
        break;
        }
    
    }

  
        cout <<  "Input Message Number: "<< endl;
        getline(cin, outLine);
        outLine += '\n';
        sendMess(outLine);

        //server answers with OK if correct paramters
        //receive message content as sent with send
        // error: ERR
    


}


void client::execDel(){
    string outLine;

    while(true){ //input username //TODO error handling missing
        cout <<  "[DEL] Username (max 8 chars): "<< endl;
        getline(cin, outLine);
        outLine += '\n';
        if(sendMess(outLine) == 0 ){
        break;
        }
    
    }

  
        cout <<  "Input Message Number: "<< endl;
        getline(cin, outLine);
        outLine += '\n';
        sendMess(outLine);

        //successfull delete: receive OK from server
        // if error : ERR

}


int client::communicate() {  //test
    string outLine;
    getline(cin, outLine);
    


    /*if( outLine != "QUIT" && outLine != "quit" && outLine != "SEND" && outLine != "send" &&
        outLine != "LIST" && outLine != "list" && outLine != "READ" && outLine != "read" &&
        outLine != "DEL" && outLine != "del")
    {
        cout << "Known Commands: SEND LIST READ DEL QUIT" << endl;
    }*/

    if(outLine == "QUIT" || outLine == "quit") {    //quitting out of connection
        return 1;
    }



    if((outLine.length() % BUFFER_LENGTH) == 0) {        //makes sure there is no '.' right at the start of the next package
        cout << outLine.length() << endl;               //TODO Test
        outLine += ' ';
        cout << outLine.length() << endl;
    }
    
    outLine += '\n';

    if(outLine == "SEND\n" || outLine == "send\n" ){ //attempting to send something (if they dare)
        sendMess(outLine);
        execSend(); 
    } else if(outLine == "LIST\n" || outLine == "list\n" ) {
        sendMess(outLine);
        execList();
    } else if (outLine == "READ\n" || outLine == "read\n" ) {
        sendMess(outLine);
        execRead();
    } else if (outLine == "DEL\n" || outLine == "del\n" ) {
        sendMess(outLine);
        execDel();
    }
    else {
        sendMess(outLine);
        
    }

    return 0;
    
    /*if(sendMess(outLine) == 1) {      //old remnant of a different time
        return 2;
    }
    else {
        return 0;
    }*/
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
