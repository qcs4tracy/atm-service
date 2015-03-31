//
// Created by tracymac on 3/31/15.
//

#include "socket.h"
#include "CNP_Protocol.h"
#include <iostream>
using namespace std;

int main() {

    sock::TCPClientSocket cs;
    if (cs.connect(INADDR_LOOPBACK, 5999)) {
        cnp::CONNECT_REQUEST cq(1,1,1,1,1);
        if ( cs.send(reinterpret_cast<char *>(&cq), sizeof(cq)) > 0) {
            cout << "success" << endl;
        }
    }

    return 0;
}