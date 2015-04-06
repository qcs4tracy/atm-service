//
// Created by tracymac on 4/3/15.
//

#ifndef CNP_ATM_CLIENTSMANAGER_H
#define CNP_ATM_CLIENTSMANAGER_H

#include "AtmDataAccess.h"
#include <map>

struct ClientEntity {

    enum CL_STATE {
        CL_UNUSED,
        CL_CONNECTED,
        CL_LOGON,
        CL_LOGOFF,
    };

    ClientEntity(CL_STATE _state = CL_UNUSED): state_(_state) {}
    ClientEntity(CL_STATE _state, atm_data::AccntInfo &_acc): state_(_state), accnt_(_acc) {}
    ClientEntity(const ClientEntity &rhs);
    CL_STATE state_;
    atm_data::AccntInfo accnt_;
};

ClientEntity::ClientEntity(const ClientEntity &rhs) = delete;

class ClientsManager;

class ClientsManager {

    public:
        typedef unsigned short cid_t;
        #define MAXCID ((unsigned short)-1)

        virtual ~ClientsManager() {
            if(_instance)
                delete _instance;
        }

        static ClientsManager& getInstance();
        void remove(cid_t clientID);
        bool add(cid_t clientID, atm_data::AccntInfo &acc, ClientEntity::CL_STATE st_);
        const ClientEntity& getByID(const cid_t clientID);
        cid_t nextClientID();
        bool setClientAcc(cid_t clientID, atm_data::AccntInfo &acc);
        void setClientState(cid_t clientID, ClientEntity::CL_STATE st_);

        unsigned int numOfClients() {
            return _nClients;
        }

    protected:
        ClientsManager(): _gcid(1000), _nClients(0) {}
        ClientsManager(const ClientsManager &rhs) = delete;
        ClientsManager& operator=(const ClientsManager &rhs) = delete;

    private:
        cid_t _gcid;
        unsigned int _nClients;
        static ClientsManager *_instance;
        std::map<cid_t, ClientEntity> _clients;
};

#endif //CNP_ATM_CLIENTSMANAGER_H
