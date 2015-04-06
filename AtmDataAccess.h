//
// Created by tracymac on 4/2/15.
//

#ifndef CNP_ATM_ATMDATAACCESS_H
#define CNP_ATM_ATMDATAACCESS_H

extern "C" {
#include <string.h>
}

#include <string>
#include <cppconn/resultset.h>
#include <cppconn/connection.h>
#include "CNP_Protocol.h"
#include <vector>

namespace atm_data {

#define DB_PROTO "tcp"
#define DB_HOST "104.131.6.249"
#define DB_USERNAME "qcs4tracy"
#define DB_PASSWD "qcs6426zqq"
#define DB_NAME "atm"
#define DB_PORT 3306

    typedef unsigned long long ulonglong;
    typedef unsigned short ushort;
    typedef unsigned int uint_;

    struct AccntInfo {
#define MAXLEN 32
#define N_FILEDS 7

        AccntInfo() {}
        AccntInfo(char const fn[], char const ln[], char const eml[], unsigned int ssn_ = 0,
                  unsigned int dl = 0, unsigned int pin = 0, unsigned long long accno = 0): accnt_no(accno),
                                                                                            ssn(ssn_), DL(dl), PIN(pin) {
            if (fn)
                ::strncpy(first_name, fn, MAXLEN);
            if (ln)
                ::strncpy(last_name, ln, MAXLEN);
            if (eml)
                ::strncpy(email, eml, MAXLEN);
        }

        AccntInfo(const AccntInfo &rhs);
        AccntInfo& operator=(AccntInfo rhs);
        void swap_(AccntInfo &rhs);

        char first_name[MAXLEN];
        char last_name[MAXLEN];
        unsigned long long accnt_no;
        char email[MAXLEN];
        unsigned int ssn;
        unsigned int DL;
        unsigned int PIN;
    };


    struct AccntBalance {
        enum ACCNT_TYPE{
            AT_CASH = cnp::DT_CASH,
            AT_CHECK = cnp::DT_CHECK,
        };
        AccntBalance(ulonglong cash_ = 0, ulonglong chk_ = 0): cash(cash_), check(chk_) {}
        ulonglong check;
        ulonglong cash;
    };


    struct TransRec {

        enum TRANS_TYPE {
            TT_INVALID = cnp::TRANSACTION_TYPE::TT_INVALID,
            TT_DEPOSIT = cnp::TRANSACTION_TYPE::TT_DEPOSIT ,
            TT_WITHDRAW = cnp::TRANSACTION_TYPE::TT_WITHDRAWAL,
        };

        TransRec(int id_ = 0, ulonglong accno_ = 0, TRANS_TYPE type_ = TT_INVALID, ulonglong date_ = 0, uint_ amt_ = 0):
                id(id_), accnt_no(accno_), type(type_), date(date_), amount(amt_) {}

        int id;
        ulonglong accnt_no;
        TRANS_TYPE type;
        ulonglong date;
        uint_ amount;
    };

    //transaction records collection
    struct TransRecSet {
        TransRecSet(int cnt_ = 0): count(cnt_), records(cnt_) {}
        int count;
        std::vector<TransRec> records;
    };



    class AtmDataAccess {

    public:
        virtual ~AtmDataAccess() {
            if (_conn)
                delete _conn;
        }

        AtmDataAccess(std::string db_host, std::string db_user, std::string db_passwd, std::string db_name, int port);
        AccntInfo * getAccntByID(ulonglong id, uint_ id_type_);
        bool createAccnt(const AccntInfo &acc);
        bool updateAccntBalance(ulonglong accnt_no, ulonglong amount, AccntBalance::ACCNT_TYPE type);
        AccntBalance *getAccntBalance(ulonglong accnt_no);
        bool addTransRec(const TransRec &record);
        TransRecSet *getRecentTransRec(ulonglong accnt_no, uint_ cnt);
        void startTransaction();
        void commit();
        void rollback();

    private:

        sql::Connection *_conn;
        std::string _db_host;
        std::string _db_user;
        std::string _db_name;
        int _port;
        bool _conn_opened = false;
    };
}

#endif //CNP_ATM_ATMDATAACCESS_H
