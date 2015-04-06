//
// Created by tracymac on 4/3/15.
//

#include "ClientsManager.h"
#include "log4z.h"
using namespace zsummer::log4z;

ClientsManager* ClientsManager::_instance = nullptr;

bool ClientsManager::add(cid_t clientID, atm_data::AccntInfo &acc, ClientEntity::CL_STATE st_) {

    if (_clients.count(clientID) > 0) {
        if (_clients[clientID].state_ != ClientEntity::CL_UNUSED) {
            LOGE("clientID[" << clientID << "] already exist.");
            return false;
        }
        //it is valid to add now
        _clients[clientID].accnt_ = acc;
        _clients[clientID].state_ = st_;
        _nClients++;
        return true;
    }

    _clients[clientID] = ClientEntity(st_, acc);
    _nClients++;
    return true;
}


bool ClientsManager::setClientAcc(cid_t clientID, atm_data::AccntInfo &acc) {

    if (_clients.count(clientID) > 0) {
        if (_clients[clientID].state_ != ClientEntity::CL_CONNECTED) {
            LOGE("clientID [" << clientID << "] already exist or not connected.");
            return false;
        }
        //it is valid to add now
        _clients[clientID].accnt_ = acc;
        _nClients++;
        return true;
    }

    return false;
}


void ClientsManager::remove(cid_t clientID) {
    if(_clients.erase(clientID) > 0)
        _nClients--;
}

ClientsManager::cid_t ClientsManager::nextClientID() {

    cid_t next = _gcid;

    if (_gcid == MAXCID)
        _gcid = 1000;

    while (_clients.count(next) && _clients[next].state_ != ClientEntity::CL_UNUSED)
        next++;

    _gcid = next + 1;
    return next;
}


const ClientEntity& ClientsManager::getByID(const cid_t clientID) {
    return _clients[clientID];
}


ClientsManager& ClientsManager::getInstance() {
    if (!_instance)
        _instance = new ClientsManager;
    return *_instance;
}


void ClientsManager::setClientState(cid_t clientID, ClientEntity::CL_STATE st_) {
    _clients[clientID].state_ = st_;
}


