//
// Created by tracymac on 3/31/15.
//

#include "socket.h"
#include "CNP_Protocol.h"
#include <iostream>
#include "time_utils.h"
#include "log4z.h"
#include "proto_impl.h"

using namespace std;
using namespace zsummer::log4z;



cnp::WORD clientID = 0;
sock::TCPClientSocket cs;

bool sendConnectReq() {

    cnp::CONNECT_REQUEST cq;

    if ( cs.send(reinterpret_cast<char *>(&cq), sizeof(cq)) > 0)
        return true;

    return false;
}


bool sendLogonReq() {

    char buff[64] = {0};
    cnp::LOGON_REQUEST cq;

    do {
        cout << "Account NO. >>";
        cin.getline(buff, 64);
    } while (buff[0] == 0);

    cq.m_Request.m_PAN = atoll(buff);

    ::memset(buff, 0, 64);

    do {
        cout << "PIN (*) [4 digits] >>";
        cin.getline(buff, 8);
    } while (buff[0] == 0 || buff[4] != 0);

    cq.m_Request.m_wPIN = atoi(buff);

    cq.m_Hdr.m_wClientID = clientID;

    if ( cs.send(reinterpret_cast<char *>(&cq), sizeof(cq)) > 0)
        return true;

    return false;

}

bool sendCreatAccntReq() {

    cnp::CREATE_ACCOUNT_REQUEST cq;
    cnp::prim::_CREATE_ACCOUNT_REQUEST *_caq = &cq.m_Request;
    char buff[128] = {0};

    do {
        cout << "First Name(*) >>";
        cin.getline(_caq->m_szFirstName, 32);
    } while (_caq->m_szFirstName[0] == 0);

    do {
        cout << "Last Name (*) >>";
        cin.getline(_caq->m_szLastName, 32);
    }  while (_caq->m_szLastName[0] == 0);


    cout << "Email >>";
    cin.getline(_caq->m_szEmailAddress, 32);

    if(_caq->m_szEmailAddress[0] == 0) {
        ::strcpy(_caq->m_szEmailAddress, "No Email");
    }

    cout << "SSN >>";
    cin.getline(buff, 128);

    if (buff[0] != 0) {
        _caq->m_dwSSNumber = ::atoi(buff);
    }

    ::memset(buff, 0, 128);

    cout << "Driver License >>";
    cin.getline(buff, 128);

    if (buff[0] != 0) {
        _caq->m_dwSSNumber = ::atoi(buff);
    }

    ::memset(buff, 0, 128);

    do {
        cout << "PIN (*) [4 digits] >>";
        cin.getline(buff, 8);
    } while (buff[0] == 0 || buff[4] != 0);

    _caq->m_wPIN = atoi(buff);
    cq.m_Hdr.m_wClientID = clientID;

    if ( cs.send(reinterpret_cast<char *>(&cq), sizeof(cq)) > 0)
        return true;

    return false;
}


bool sendBalQueryReq() {

    cnp::BALANCE_QUERY_REQUEST cq(clientID);

    if ( cs.send(reinterpret_cast<char *>(&cq), sizeof(cq)) > 0)
        return true;

    return false;

}


void recvRsp() {

    int _nrecv = cs.recv();
    _uResponse rsp;
    istream &in = cs.getIn();
    size_t hd_sz = sizeof(struct cnp::STD_HDR);
    size_t bd_sz = 0;

    if (_nrecv <= 0) {
        LOGE("Data Not Received.");
        return;
    }

    in.read(reinterpret_cast<char *>(&rsp.header), hd_sz);

    if (in.gcount() < hd_sz) {
        LOGE("ERROR: Invalid Message Received!");
        return;
    }

    bd_sz = rsp.header.m_wDataLen;
    in.read(reinterpret_cast<char *>(&rsp.body), bd_sz);
    if (in.gcount() < bd_sz) {
        LOGE("ERROR: Invalid Message Received!")
    }

    switch (rsp.header.m_dwMsgType) {

        case cnp::MT_CONNECT_RESPONSE_ID:

            if (rsp.body.conn.m_dwResult != cnp::CER_SUCCESS) {
                LOGE("Connect request failed");
                return;
            }

            clientID = rsp.body.conn.m_wClientID;
            cout << "Connected: Client ID [ " << clientID << " ]" << endl;
            break;

        case cnp::MT_CREATE_ACCOUNT_RESPONSE_ID:

            if (rsp.body.creat_accnt.m_dwResult != cnp::CER_SUCCESS) {
                LOGE("Create Account Failed");
                return;
            }

            LOGI("Account Created: the Account NO. assigned is " << rsp.body.creat_accnt.m_qwPAN);
            break;

        case cnp::MT_LOGON_RESPONSE_ID:

            if (rsp.body.logon.m_dwResult == cnp::CER_SUCCESS) {
                cout << "Logged on." << endl;
                return;
            }

            if (rsp.body.logon.m_dwResult == cnp::CER_INVALID_NAME_PIN) {
                cerr << "Logon failed: Invalid Account NO. or PIN" << endl;
            } else if (rsp.body.logon.m_dwResult == cnp::CER_INVALID_CLIENTID) {
                cerr << "Logon failed: Invalid Client ID" << endl;
            }

        case cnp::MT_BALANCE_QUERY_RESPONSE_ID:

            if (rsp.body.balance_q.m_dwResult != cnp::CER_SUCCESS) {
                cerr << "Query Balance failed" << endl;
                return;
            }

            cout << "cash balance: " << rsp.body.balance_q.m_dwCashBalance
                << ", check balance: " << rsp.body.balance_q.m_dwCheckBalance << endl;
            break;
        default:
            break;
    }

}





int main() {

    ILog4zManager::getInstance()->start();

    if (!cs.connect(INADDR_LOOPBACK, 5999)) {
        LOGF("Fail to connect the server");
        exit(2);
    }

    if (sendConnectReq()) {
        recvRsp();
    }

    if (sendLogonReq()) {
        recvRsp();
    }

    if (sendBalQueryReq()) {
        recvRsp();
    }




    return 0;
}





//
//int i = 0, n, k;
////cnp::CONNECT_REQUEST cq;
////cnp::CONNECT_RESPONSE rsp;
////cnp::LOGON_REQUEST cq(1000, 4388576071061090, 123545);
////cnp::LOGON_RESPONSE rsp;
////cnp::CREATE_ACCOUNT_REQUEST cq(1000, "JR.", "King", "jrking@sina.com", 123456, 45612314, 45678900, 0);
////cnp::CREATE_ACCOUNT_RESPONSE rsp;
////    cnp::DEPOSIT_REQUEST cq(1000, 200, cnp::DT_CASH);
////    cnp::DEPOSIT_RESPONSE rsp;
////    cnp::WITHDRAWAL_REQUEST cq(3000, 3000, 0);
////    cnp::WITHDRAWAL_RESPONSE rsp;
//cnp::TRANSACTION_QUERY_REQUEST cq(1000, 0, 5, 0);
//cnp::TRANSACTION_QUERY_RESPONSE rsp;
//cnp::TRANSACTION *trans;
//
//if (!cs.connect(INADDR_LOOPBACK, 5999))
//exit(2);
//
//if ( cs.send(reinterpret_cast<char *>(&cq), sizeof(cq)) > 0) {
//cout << "success" << endl;
//}
//
//size_t nrc = cs.recv();
//
//if (nrc > 0){
//
//cs.getIn().read(reinterpret_cast<char *>(&rsp), sizeof(rsp));
//n = rsp.m_Response.m_wTransactionCount;
//
//if (n > 0) {
//
//trans = &rsp.m_Response.m_rgTransactions[0];
//cout << "[id: " << trans[0].m_dwID << ", amount: "
//<< trans[0].m_dwAmount << ", type: " << trans[0].m_wType
//<< ", date: " << time_utils::TimeUtil::UTC2Local((const time_t *)&trans[0].m_qwDateTime) << "]" << endl;
//
//k = rsp.m_Hdr.m_wDataLen - sizeof(cnp::prim::_TRANSACTION_QUERY_RESPONSE);
//
//if (k > 0) {
//
//trans = (cnp::TRANSACTION *) new char[k];
//cs.getIn().read((char *)trans, k);
//
//for (i = 0; i < n - 1; ++i) {
//cout << "[id: " << trans[i].m_dwID << ", amount: "
//<< trans[i].m_dwAmount << ", type: " << trans[i].m_wType
//<< ", date: " << time_utils::TimeUtil::UTC2Local((const time_t *)&trans[i].m_qwDateTime) << "]" << endl;
//}
//}
//}
//
//
//}