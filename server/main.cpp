#include <iostream>
#include "server.h"

using namespace std;


int main(int argc, char* argv[]) {
    cout << "Hello World!" << endl;
    mailServer* serf = new mailServer;
    delete serf;
}