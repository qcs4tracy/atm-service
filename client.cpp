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


bool logged_on = false;
bool exiting = false;
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

bool sendDepositReq() {

    char buf[64];
    cnp::DEPOSIT_REQUEST cq;
    double amt = 0;
    int type = 0;

    do {
        cout << "Select Account Type (1:cash 2:check) >>";
        cin.getline(buf, 64);
    } while(buf[0] == 0 || ::atoi(buf) < 1 || ::atoi(buf) > 2);

    type = ::atoi(buf);

    if (type == 1) {
        cq.m_Request.m_wType = cnp::DT_CASH;
    } else {
        cq.m_Request.m_wType = cnp::DT_CHECK;
    }

    do {
        cout << "Deposit Amount:";
        cin.getline(buf, 64);
    } while (buf[0] == 0 || (amt = ::atof(buf)) <= 0);


    cq.m_Hdr.m_wClientID = clientID;
    cq.m_Request.m_dwAmount = (cnp::DWORD)(amt*100);

    if ( cs.send(reinterpret_cast<char *>(&cq), sizeof(cq)) > 0)
        return true;

    return false;

}


bool sendWithdrawReq() {

    char buf[64];
    cnp::WITHDRAWAL_REQUEST cq;
    double amt = 0;

    do {
        cout << "Withdraw Amount:";
        cin.getline(buf, 64);
    } while (buf[0] == 0 || (amt = ::atof(buf)) <= 0);


    cq.m_Hdr.m_wClientID = clientID;
    cq.m_Request.m_dwAmount = (cnp::DWORD)(amt*100);

    if ( cs.send(reinterpret_cast<char *>(&cq), sizeof(cq)) > 0)
        return true;

    return false;
}


bool sendLogOffReq() {

    cnp::LOGOFF_REQUEST cq(clientID, 0);

    if(!logged_on)
        cerr << "Error: Not yet logged on" << endl;

    if ( cs.send(reinterpret_cast<char *>(&cq), sizeof(cq)) > 0)
        return true;

    return false;
}

#define TRANS_TYPE(type) (type == atm_data::TransRec::TT_DEPOSIT? "Deposit": "Withdraw")

void processTransRecs(cnp::STD_HDR &header, istream &in) {

    cnp::TRANSACTION_QUERY_RESPONSE rsp;
    cnp::TRANSACTION *trans;
    int n = 0, k= 0, i;

    in.read(reinterpret_cast<char *>(&rsp.m_Response), sizeof(rsp.m_Response));

    if (rsp.m_Response.m_dwResult != cnp::CER_SUCCESS) {
        cerr << "Transaction Query Request Failed" << endl;
        return;
    }

    n = rsp.m_Response.m_wTransactionCount;

    if (n > 0) {

        trans = &rsp.m_Response.m_rgTransactions[0];
        cout << "| id: " << trans[0].m_dwID << " | amount: "
        << (double)trans[0].m_dwAmount/100 << " | type: " << TRANS_TYPE(trans[0].m_wType)
        << "| date: " << time_utils::TimeUtil::UTC2Local((const time_t *) &trans[0].m_qwDateTime) << " |" << endl;

        k = header.m_wDataLen - sizeof(cnp::prim::_TRANSACTION_QUERY_RESPONSE);

        if (k > 0) {

            trans = (cnp::TRANSACTION *) new char[k];
            in.read((char *) trans, k);

            for (i = 0; i < n - 1; ++i) {
                cout << "| id: " << trans[i].m_dwID << " | amount: "
                << (double)trans[i].m_dwAmount/100 << " | type: " << TRANS_TYPE(trans[i].m_wType)
                << " | date: " << time_utils::TimeUtil::UTC2Local((const time_t *) &trans[i].m_qwDateTime) << " |" << endl;
            }

            delete (char *)trans;
        }

        cout << endl << n << " transactions found" << endl;
        return;
    }

    cout << "No Transaction Recently" << endl;
}


bool sendTransQueryReq() {

    char buff[64] = {0};
    int n;
    cnp::TRANSACTION_QUERY_REQUEST cq;

    do {
        cout << "Number of Most Recent Transactions to Look Up >>";
        cin.getline(buff, 64);
    } while(buff[0] == 0 || (n =::atoi(buff)) <= 0);

    cq.m_Hdr.m_wClientID = clientID;
    cq.m_Request.m_wTransactionCount = n;

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

    if (_nrecv < 0) {
        LOGE("Data Not Received.");
        return;
    }

    if (_nrecv == 0) {
        LOGW("Server Disconnected");
        cs.close();
        exit(2);
    }

    in.read(reinterpret_cast<char *>(&rsp.header), hd_sz);

    if (in.gcount() < hd_sz) {
        LOGE("ERROR: Invalid Message Received!");
        return;
    }

    if (rsp.header.m_dwMsgType == cnp::MT_TRANSACTION_QUERY_RESPONSE_ID) {
        processTransRecs(rsp.header, in);
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
                LOGE("Connect request failed, please try again.");
                exit(2);
            }

            clientID = rsp.body.conn.m_wClientID;
            cout << "Connected: Client ID [ " << clientID << " ]" << endl;
            break;

        case cnp::MT_LOGOFF_RESPONSE_ID:

            if (rsp.body.logoff.m_dwResult != cnp::CER_SUCCESS) {
                cout << "Invalid client ID or Account not logged on" << endl;
                break;
            }

            cout << "Log off succeed" << endl;
            logged_on = false;
            break;

        case cnp::MT_CREATE_ACCOUNT_RESPONSE_ID:

            if (rsp.body.creat_accnt.m_dwResult != cnp::CER_SUCCESS) {
                LOGE("Create Account Failed");
                break;
            }

            cout << "Account Created: the Account NO. assigned is " << rsp.body.creat_accnt.m_qwPAN << endl;
            break;

        case cnp::MT_LOGON_RESPONSE_ID:

            if (rsp.body.logon.m_dwResult == cnp::CER_SUCCESS) {
                cout << "Logged on." << endl;
                logged_on = true;
                break;
            }

            if (rsp.body.logon.m_dwResult == cnp::CER_INVALID_NAME_PIN) {
                cerr << "Logon failed: Invalid Account NO. or PIN" << endl;
            } else if (rsp.body.logon.m_dwResult == cnp::CER_INVALID_CLIENTID) {
                cerr << "Logon failed: Invalid Client ID" << endl;
            }

            break;

        case cnp::MT_BALANCE_QUERY_RESPONSE_ID:

            if (rsp.body.balance_q.m_dwResult != cnp::CER_SUCCESS) {
                cerr << "Query Balance failed" << endl;
                break;
            }

            cout << "cash balance: " << (double)rsp.body.balance_q.m_dwCashBalance/100
                << ", check balance: " << (double)rsp.body.balance_q.m_dwCheckBalance/100 << endl;
            break;

        case cnp::MT_DEPOSIT_RESPONSE_ID:

            if (rsp.body.deposit.m_dwResult != cnp::CER_SUCCESS) {
                cerr << "Deposit Request failed" << endl;
                break;
            }

            cout << "Deposit Succeed" << endl;

            break;

        case cnp::MT_WITHDRAWAL_RESPONSE_ID:

            if (rsp.body.withdrawal.m_dwResult == cnp::CER_INSUFFICIENT_FUNDS) {
                cerr << "Account Balance is Not Sufficient." << endl;
                return;
            }

            if (rsp.body.withdrawal.m_dwResult == cnp::CER_ERROR) {
                cerr << "Withdrawal Request Error. Please Try again." << endl;
                return;
            }

            cout << "Withdrwal Succeed" << endl;
            break;

        default:

            cerr << "Unknown Error" << endl;
            break;
    }

    cout << endl;
}


void level1_menu() {

    char buf[64] = {0};
    int opt;

    do {
        cout << "Please Choose Option: 1:Log On 2:Create an Account 3:Exit [type in 1 , 2 or 3]>>";
        cin.getline(buf, 64);
    } while(buf[0] == 0 || (::atoi(buf) > 3) || (::atoi(buf) < 1));

    opt = ::atoi(buf);

    switch (opt) {

        case 1:
            sendLogonReq();
            break;

        case 2:
            sendCreatAccntReq();
            break;

        case 3:
            cs.close();
            exit(0);
            break;

        default:
            break;
    }

}


void level2_menu() {

    char buf[64] = {0};
    int opt;

    do {
        cout << "Please Choose Option: 1:Withdraw  2:Deposit 3:Balance Query  4:Transactions Query 5: Logoff>>";
        cin.getline(buf, 64);
    } while(buf[0] == 0 || (::atoi(buf) > 5) || (::atoi(buf) < 1));

    opt = ::atoi(buf);

    switch (opt) {

        case 1:
            sendWithdrawReq();
            break;

        case 2:
            sendDepositReq();
            break;

        case 3:
            sendBalQueryReq();
            break;

        case 4:
            sendTransQueryReq();
            break;

        case 5:
            sendLogOffReq();
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

    if (!sendConnectReq()) {
        LOGF("Client Connection Can Not Be Setup");
        exit(2);
    }

    recvRsp();

    do {

        if (!logged_on) {
            level1_menu();
        } else {
            level2_menu();
        }

        recvRsp();

    } while (!exiting);

    return 0;
}