//
// Created by tracymac on 3/31/15.
//

#include "proto_impl.h"
#include "log4z.h"
using namespace zsummer::log4z;

void ProtoHandler::process_msg(std::istream &instrm) {

    _res_msg_size = 0;

    if (!read_header(instrm)) {
        LOGW("read_header() failed.\n");
    }

    if (!process_header())
        LOGW("process_header() failed.\n");

    if (!read_body(instrm))
        LOGW("read_body() failed.\n");

    if (!generate_content())
        LOGW("generate_content() failed.\n");

}


bool CNProtoHandler::read_header(std::istream &in) {

    size_t hd_size = sizeof(struct cnp::STD_HDR);
    in.read(reinterpret_cast<char *>(&_request), hd_size);

    /*request header incomplete: response will be error msg*/
    if (in.gcount() < hd_size) {
        _request.header.m_dwMsgType = cnp::MT_INVALID_ID;
        //reset istream
        LOGW("incomplete header\n");
        return false;
    }

    return true;
}


bool CNProtoHandler::process_header() {

    cnp::DWORD msgType = _request.header.get_MsgType();

    if (msgType != cnp::MT_INVALID_ID)
        return true;

    LOGW("invalid request");
    return false;

}


bool CNProtoHandler::read_body(std::istream &in) {

    /*header is correctly read*/
    if (_request.header.m_dwMsgType != cnp::MT_INVALID_ID) {

        size_t dataLen = _request.header.m_wDataLen;
        in.read(reinterpret_cast<char *>(&_request.body), dataLen);

        if (in.gcount() < dataLen) {
            _request.header.m_dwMsgType = cnp::MT_INVALID_ID;
            LOGW("request body is not complete\n");
            return false;
        }
    }

    return true;
}


void CNProtoHandler::_handleTransLookup() {

    unsigned long n = 0;
    size_t _tsz = sizeof(_response.body.trans_q);
    atm_data::TransRecSet *transRecs;
    n = _request.body.trans_q.m_wTransactionCount;
    _uResponse *rsp = &_response;
    cnp::TRANSACTION *trans;

    transRecs = _da->getRecentTransRec(4388576071061090, n);

    //Error if NULL is returned here
    if (!transRecs) {
        _response.header.m_dwMsgType = cnp::MT_TRANSACTION_QUERY_RESPONSE_ID;
        _response.header.m_wDataLen = _tsz;
        _res_msg_size += _tsz;
        _response.body.trans_q.m_dwResult = cnp::CER_ERROR;
        _buff = (char *) rsp;
        return;
    }

    n = transRecs->count;
    if (n > 1) {//get some here

        _tsz = sizeof(cnp::TRANSACTION_QUERY_RESPONSE) + sizeof(struct cnp::TRANSACTION) * (n-1);

        if(_has_dyn_buff) {
            delete _dyn_buff;
        }

        _dyn_buff = new char[_tsz];
        rsp = (struct _uResponse *)_dyn_buff;

        if(!_has_dyn_buff) {
            _has_dyn_buff = true;
        }

        _tsz -= sizeof(struct cnp::STD_HDR);
    }

    trans = rsp->body.trans_q.m_rgTransactions;
    //populate the transactions
    for (int j = 0; j < n ; ++j) {
        trans[j].m_dwID = transRecs->records[j].id;
        trans[j].m_dwAmount = transRecs->records[j].amount;
        trans[j].m_qwDateTime = transRecs->records[j].date;
        trans[j].m_wType = transRecs->records[j].type;
    }

    rsp->header.m_dwMsgType = cnp::MT_TRANSACTION_QUERY_RESPONSE_ID;
    rsp->header.m_wDataLen = _tsz;
    _res_msg_size += _tsz;

    rsp->body.trans_q.m_dwResult = cnp::CER_SUCCESS;
    rsp->body.trans_q.m_wTransactionCount = n;
    _buff = (char *)rsp;
}


void CNProtoHandler::_handleConnect() {

    ClientsManager::cid_t cid = 0;
    size_t _sz = sizeof(_response.body.conn);
    //set response type and body size
    _response.header.m_dwMsgType = cnp::MT_CONNECT_RESPONSE_ID;
    _response.header.m_wDataLen = _sz;
    //get client ID
    cid = _cm->nextClientID();
    _cm->setClientState(cid, ClientEntity::CL_CONNECTED);
    _response.body.conn.m_dwResult = cnp::CER_SUCCESS;
    _response.body.conn.m_wMajorVersion = cnp::g_wMajorVersion;
    _response.body.conn.m_wMinorVersion = cnp::g_wMinorVersion;
    _response.body.conn.m_wClientID = cid;

    _res_msg_size += _sz;
    _buff = (char *)&_response;

}

void CNProtoHandler::_handleLogOn() {

    size_t _sz = sizeof(_response.body.logon);
    atm_data::AccntInfo *acc;
    ClientsManager::cid_t cid = 0;

    _response.header.m_dwMsgType = cnp::MT_LOGON_RESPONSE_ID;
    _response.header.m_wDataLen = _sz;
    _res_msg_size += _sz;

    cid = _request.header.m_wClientID;
    acc = _da->getAccntByID(_request.body.logon.m_PAN, 0);
    //the PAN is not exist
    if (!acc) {
        _response.body.logon.m_dwResult = cnp::CER_INVALID_NAME_PIN;
        return;
    }

    if ((cnp::WORD)acc->PIN != _request.body.logon.m_wPIN) {
        _response.body.logon.m_dwResult = cnp::CER_INVALID_NAME_PIN;
        return;
    }

    if (!_cm->setClientAcc(cid, *acc)) {
        _response.body.logon.m_dwResult = cnp::CER_INVALID_CLIENTID;
        return;
    }

    _cm->setClientState(cid, ClientEntity::CL_LOGON);
    _response.body.logon.m_dwResult = cnp::CER_SUCCESS;
    _buff = (char *)&_response;
    LOGI("account[" << acc->accnt_no << "] logged on.");
}


void CNProtoHandler::_handleAccntCreat() {

    struct cnp::prim::_CREATE_ACCOUNT_REQUEST *creat_ac;
    atm_data::AccntInfo *acc;

    size_t _sz = sizeof(_response.body.creat_accnt);
    _response.header.m_dwMsgType = cnp::MT_CREATE_ACCOUNT_RESPONSE_ID;
    _response.header.m_wDataLen = _sz;
    _res_msg_size += _sz;

    creat_ac = &_request.body.creat_accnt;
    _buff = (char *)&_response;

    if( _da->createAccnt( atm_data::AccntInfo(creat_ac->m_szFirstName, creat_ac->m_szLastName,
                                              creat_ac->m_szEmailAddress, creat_ac->m_dwSSNumber,
                                              creat_ac->m_dwDLNumber, creat_ac->m_wPIN) ) ) {

        acc = _da->getAccntByID(_request.body.creat_accnt.m_dwSSNumber, 1);

        if(acc) {
            _response.body.creat_accnt.m_dwResult = cnp::CER_SUCCESS;
            _response.body.creat_accnt.m_qwPAN = acc->accnt_no;
            delete acc;
            return;
        }

    }

    _response.body.creat_accnt.m_dwResult = cnp::CER_ERROR;

}


void CNProtoHandler::_handleDeposit() {

    atm_data::AccntBalance::ACCNT_TYPE accntType;
    atm_data::AccntBalance *accBl;
    atm_data::ulonglong newBl;
    ClientsManager::cid_t cid = 0;
    atm_data::ulonglong accno;

    size_t _sz = sizeof(_response.body.deposit);
    _response.header.m_dwMsgType = cnp::MT_DEPOSIT_RESPONSE_ID;
    _response.header.m_wDataLen = _sz;
    _res_msg_size += _sz;

    cid = _request.header.m_wClientID;
    const ClientEntity &ce = _cm->getByID(cid);

    if (ce.state_ != ClientEntity::CL_LOGON) {
        _response.body.deposit.m_dwResult = cnp::CER_CLIENT_NOT_LOGGEDON;
        return;
    }

    accno = ce.accnt_.accnt_no;

    accBl = _da->getAccntBalance(accno);
    accntType = _request.body.deposit.m_wType == cnp::DT_CASH? atm_data::AccntBalance::AT_CASH: atm_data::AccntBalance::AT_CHECK;

    if (accBl) {

        newBl = accntType == atm_data::AccntBalance::AT_CASH? (accBl->cash + _request.body.deposit.m_dwAmount): (accBl->check + _request.body.deposit.m_dwAmount);
        delete accBl;

        if (_da->updateAccntBalance(accno, newBl, accntType)) {

            if ( _da->addTransRec(atm_data::TransRec(0, accno, atm_data::TransRec::TT_DEPOSIT, 0, _request.body.deposit.m_dwAmount)) ) {
                _response.body.deposit.m_dwResult = cnp::CER_SUCCESS;
            }
        }
    }

    _response.body.deposit.m_dwResult = cnp::CER_ERROR;

}


void CNProtoHandler::_handleWithdraw() {

    atm_data::AccntBalance *accBl;
    ClientsManager::cid_t cid = 0;
    atm_data::ulonglong accno;
    long long bal;

    size_t _sz = sizeof(_response.body.withdrawal);
    _response.header.m_dwMsgType = cnp::MT_WITHDRAWAL_RESPONSE_ID;
    _response.header.m_wDataLen = _sz;
    _res_msg_size += _sz;

    cid = _request.header.m_wClientID;
    const ClientEntity &ce = _cm->getByID(cid);

    if (ce.state_ != ClientEntity::CL_LOGON) {
        _response.body.deposit.m_dwResult = cnp::CER_CLIENT_NOT_LOGGEDON;
        return;
    }

    accno = ce.accnt_.accnt_no;
    accBl = _da->getAccntBalance(accno);
    _response.body.withdrawal.m_dwResult = cnp::CER_ERROR;

    if (accBl) {

        bal = accBl->cash - _request.body.withdrawal.m_dwAmount;
        if (bal >= 0) {

            if (_da->updateAccntBalance(accno, (atm_data::ulonglong) bal, atm_data::AccntBalance::AT_CASH)) {

                if ( _da->addTransRec( atm_data::TransRec(0, accno, atm_data::TransRec::TT_WITHDRAW,
                                                         0, _request.body.withdrawal.m_dwAmount) ) ) {
                    _response.body.withdrawal.m_dwResult = cnp::CER_SUCCESS;
                }
            }

        } else {
            _response.body.withdrawal.m_dwResult = cnp::CER_INSUFFICIENT_FUNDS;
        }

    }

}


void CNProtoHandler::_handleBalanceLookup() {

    atm_data::AccntBalance::ACCNT_TYPE accntType;
    atm_data::AccntBalance *accBl;
    atm_data::ulonglong newBl;
    ClientsManager::cid_t cid = 0;
    atm_data::ulonglong accno;

    size_t _sz = sizeof(_response.body.balance_q);
    _response.header.m_dwMsgType = cnp::MT_BALANCE_QUERY_RESPONSE_ID;
    _response.header.m_wDataLen = _sz;
    _res_msg_size += _sz;

    cid = _request.header.m_wClientID;
    const ClientEntity &ce = _cm->getByID(cid);

    if (ce.state_ != ClientEntity::CL_LOGON) {
        _response.body.deposit.m_dwResult = cnp::CER_CLIENT_NOT_LOGGEDON;
        return;
    }

    accno = ce.accnt_.accnt_no;
    accBl = _da->getAccntBalance(accno);

    if (accBl) {
        _response.body.balance_q.m_dwResult = cnp::CER_SUCCESS;
        _response.body.balance_q.m_dwCashBalance = accBl->cash;
        _response.body.balance_q.m_dwCheckBalance = accBl->check;
        _buff = (char *)&_response;
        delete accBl;
        return;
    }

    _response.body.deposit.m_dwResult = cnp::CER_ERROR;

}

bool CNProtoHandler::generate_content() {

    cnp::DWORD msgType = _request.header.get_MsgType();
    _res_msg_size = sizeof(struct cnp::STD_HDR);

    switch (msgType) {

        case cnp::MT_CONNECT_REQUEST_ID:
            _handleConnect();
            break;

        case cnp::MT_LOGON_REQUEST_ID:
            _handleLogOn();
            break;

        case cnp::MT_CREATE_ACCOUNT_REQUEST_ID:
            _handleAccntCreat();
            break;

        case cnp::MT_DEPOSIT_REQUEST_ID:
            _handleDeposit();
            break;

        case cnp::MT_WITHDRAWAL_REQUEST_ID:
            _handleWithdraw();
            break;

        case cnp::MT_TRANSACTION_QUERY_REQUEST_ID:
            _handleTransLookup();
            break;

        case cnp::MT_BALANCE_QUERY_REQUEST_ID:
            _handleBalanceLookup();
            break;

        default:
            _response.header.m_dwMsgType = cnp::MT_INVALID_ID;
            _response.header.m_wDataLen = sizeof(_response.body.error);
            _response.body.error.m_dwResult = cnp::CER_ERROR;
            _res_msg_size += _response.header.m_wDataLen;
            break;
    }

    return true;
}


char* CNProtoHandler::get_res_msg() {
    return _buff;
}


size_t CNProtoHandler::res_msg_size() {
    return _res_msg_size;
}


CNProtoHandler::CNProtoHandler():_buff(NULL) {
    _cm = &ClientsManager::getInstance();
    _da = new atm_data::AtmDataAccess(DB_HOST, DB_USERNAME, DB_PASSWD, DB_NAME, DB_PORT);
}
