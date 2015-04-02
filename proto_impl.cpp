//
// Created by tracymac on 3/31/15.
//

#include "proto_impl.h"
#include "log4z.h"
using namespace zsummer::log4z;

void ProtoHandler::process_msg(std::istream &instrm) {

    if (!read_header(instrm))
        LOGW("read_header() failed.\n");

    if (!process_header())
        LOGW("process_header() failed.\n");

    if (!read_body(instrm))
        LOGW("read_body() failed.\n");

    if (!generate_content())
        LOGW("generate_content() failed.\n");

    create_response();

}

bool CNProtoHandler::read_header(std::istream &in) {

    size_t hd_size = sizeof(struct cnp::STD_HDR);
    in.read(reinterpret_cast<char *>(&_request), hd_size);

    /*request header incomplete: response will be error msg*/
    if (in.gcount() < hd_size) {
        _request.header.m_dwMsgType = cnp::MT_INVALID_ID;
        LOGW("incomplete header\n");
        return false;
    }

    _res_msg_size = hd_size;
    return true;
}


bool CNProtoHandler::process_header() {

    cnp::DWORD msgType = _request.header.get_MsgType();

    if (msgType != cnp::MT_INVALID_ID) {

        cnp::DWORD rspType;
        cnp::DWORD rspSize;

        switch (msgType) {

            case cnp::MT_CONNECT_REQUEST_ID:
                rspType = cnp::MT_CONNECT_RESPONSE_ID;
                rspSize = sizeof(_response.body.conn);
                break;
                /*case : ...*/
            default:
                rspType = cnp::MT_INVALID_ID;
                rspSize = sizeof(_response.body.conn);
        }

        _response.header.m_dwMsgType = rspType;
        _response.header.m_wDataLen = rspSize;
        return true;
    }

    return false;
}


bool CNProtoHandler::read_body(std::istream &instrm) {

    /*header is correctly read*/
    if (_request.header.m_dwMsgType != cnp::MT_INVALID_ID) {

        size_t dataLen = _request.header.m_wDataLen;
        instrm.read(reinterpret_cast<char *>(&_request.body), dataLen);

        if (instrm.gcount() < dataLen) {
            _response.header.m_dwMsgType = cnp::MT_INVALID_ID;
            _response.header.m_wDataLen = sizeof(_response.body.conn);
            LOGW("request body is not complete\n");
            return false;
        }
    }

    return true;
}


bool CNProtoHandler::generate_content() {

    _res_msg_size += _response.header.m_wDataLen;

    if (_response.header.m_dwMsgType != cnp::MT_INVALID_ID) {
        _response.body.conn.m_dwResult = cnp::CER_SUCCESS;
        _response.body.conn.m_wMajorVersion = 1;
        _response.body.conn.m_wMinorVersion = 2;
        _response.body.conn.m_wClientID = 10001;
        return true;
    }

    _response.body.conn.m_dwResult = cnp::CER_ERROR;
    _response.body.conn.m_wMajorVersion = 1;
    _response.body.conn.m_wMinorVersion = 2;
    _response.body.conn.m_wClientID = 0;
    return false;
}


void CNProtoHandler::create_response() {

    _buff = reinterpret_cast<char *>(&_response);
}


char* CNProtoHandler::get_res_msg() {
    return _buff;
}


size_t CNProtoHandler::res_msg_size() {
    return _res_msg_size;
}














