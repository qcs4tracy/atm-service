//
// Created by tracymac on 3/31/15.
//

#include "socket.h"
#include "CNP_Protocol.h"
#include <iostream>
using namespace std;

int main() {

    sock::TCPClientSocket cs;
    cnp::CONNECT_REQUEST cq(1,1,1,1,1);
    cnp::CONNECT_RESPONSE rsp;

    if (cs.connect(INADDR_LOOPBACK, 5999)) {

        if ( cs.send(reinterpret_cast<char *>(&cq), sizeof(cq)) > 0) {
            cout << "success" << endl;
        }

        size_t nrc = cs.recv();

        if (nrc > 0){
            cout << nrc << endl;
            cout << sizeof(rsp.m_Response) << endl;
            cs.getIn().read(reinterpret_cast<char *>(&rsp), sizeof(rsp));
            cout << rsp.m_Hdr.m_wDataLen  << endl;
        }

    }

    return 0;
}