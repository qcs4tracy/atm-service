//
// Created by tracymac on 3/31/15.
//

#ifndef CNP_ATM_PROTO_IMPL_H
#define CNP_ATM_PROTO_IMPL_H

#include "CNP_Protocol.h"
#include "ClientsManager.h"
#include <iostream>
#include "AtmDataAccess.h"

/*universal request struct that can used to represent any request*/
struct _uRequest {

    struct cnp::STD_HDR header;/**header for the msg*/

    union _uReqBody {//body encompass all type of msg
        _uReqBody(): conn() {}
        struct cnp::prim::_CONNECTION_REQUEST conn;
        struct cnp::prim::_LOGON_REQUEST logon;
        struct cnp::prim::_BALANCE_QUERY_REQUEST balance_q;
        struct cnp::prim::_DEPOSIT_REQUEST deposit;
        struct cnp::prim::_WITHDRAWAL_REQUEST withdrawal;
        struct cnp::prim::_CREATE_ACCOUNT_REQUEST creat_accnt;
        struct cnp::prim::_STAMP_PURCHASE_REQUEST stmp_purchase;
        struct cnp::prim::_TRANSACTION_QUERY_REQUEST trans_q;
        struct cnp::prim::_LOGOFF_REQUEST logoff;
    } body;

    _uRequest(): header(), body() {}
};

/*universal response struct that can be used to represent any response*/
struct _uResponse {

    struct cnp::STD_HDR header;/** header of response msg*/

    union _uRespBody {
        _uRespBody():conn() {}
        struct cnp::prim::_CONNECTION_RESPONSE conn;
        struct cnp::prim::_LOGON_RESPONSE logon;
        struct cnp::prim::_BALANCE_QUERY_RESPONSE balance_q;
        struct cnp::prim::_DEPOSIT_RESPONSE deposit;
        struct cnp::prim::_WITHDRAWAL_RESPONSE withdrawal;
        struct cnp::prim::_CREATE_ACCOUNT_RESPONSE creat_accnt;
        struct cnp::prim::_STAMP_PURCHASE_RESPONSE stmp_purchase;
        struct cnp::prim::_TRANSACTION_QUERY_RESPONSE trans_q;
        struct cnp::prim::_LOGOFF_RESPONSE logoff;
        struct cnp::prim::_ERROR_RESPONSE error;
    } body;

    _uResponse(): header(), body() {};

};


/*interface hook for protocol handler*/
class ProtoHandler {

public:
    virtual void process_msg(std::istream &instrm);
    virtual size_t res_msg_size() = 0;
    virtual char *get_res_msg() = 0;

protected:
    virtual bool read_header(std::istream &in) = 0;
    virtual bool process_header() = 0;
    virtual bool read_body(std::istream &in) = 0;
    virtual bool generate_content() = 0;
    //virtual void create_response() = 0;

    size_t _res_msg_size = 0;
};


class CNProtoHandler : public ProtoHandler {

public:
    CNProtoHandler();
    virtual ~CNProtoHandler(){
        delete _da;
        if (_has_dyn_buff && _dyn_buff != NULL)
            delete _dyn_buff;
    }
    size_t res_msg_size();
    char *get_res_msg();

protected:
     bool read_header(std::istream &in);
     bool process_header();
     bool read_body(std::istream &in);
     bool generate_content();
     //void create_response();

private:
    void _handleTransLookup();
    void _handleConnect();
    void _handleLogOn();
    void _handleAccntCreat();
    void _handleDeposit();
    void _handleWithdraw();
    void _handleBalanceLookup();
    void _handleLogOff();
    struct _uRequest _request;
    struct _uResponse _response;
    char * _buff;
    bool _has_dyn_buff = false;
    char * _dyn_buff = NULL;
    ClientsManager *_cm;
    atm_data::AtmDataAccess *_da;
};


#endif //CNP_ATM_PROTO_IMPL_H
