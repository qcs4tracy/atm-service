#include <iostream>
#include "CNP_Protocol.h"
using namespace std;

#include "socket.h"
#include "IOLoop.h"


int main () {


    ioloop::IOLoop ioloop(1024);
    ioloop.setup_listen_sock(INADDR_LOOPBACK, 5999);
    ioloop.enable_listning(1024);
    ioloop.start();

    return 0;
}

