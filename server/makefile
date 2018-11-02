#TODO one file to make them all
#compiler flags als konstanten
all: server 

server: server.cpp main.cpp
	g++ -o server server.cpp server.h main.cpp -lldap -llber

clean:
	rm -f server
