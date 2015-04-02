//
// Created by tracymac on 4/1/15.
//

#ifndef CNP_ATM_DATA_ACCESS_H
#define CNP_ATM_DATA_ACCESS_H

#include <string>

extern "C" {
    #include "mysql_client.h"
    #include <string.h>
}


struct AccntInfo {
#define MAXLEN 32
#define N_FILEDS 7
    AccntInfo(char fn[], char ln[], char eml[], unsigned int ssn_ = 0,
              unsigned int dl = 0, unsigned int pin = 0, unsigned long long accno = 0): accnt_no(accno),
                                                                                        ssn(ssn_), DL(dl), PIN(pin) {
        if (fn)
            ::strncpy(first_name, fn, MAXLEN);
        if (ln)
            ::strncpy(last_name, ln, MAXLEN);
        if (eml)
            ::strncpy(email, eml, MAXLEN);
    }

    char first_name[MAXLEN];
    char last_name[MAXLEN];
    unsigned long long accnt_no;
    char email[MAXLEN];
    unsigned int ssn;
    unsigned int DL;
    unsigned int PIN;
};


class DataBaseAccess {

public:
    virtual ~DataBaseAccess();
    DataBaseAccess(): conn(NULL){}
    DataBaseAccess(std::string db_host, std::string db_user, std::string db_passwd, std::string db_name, int port);
    bool open_conn(std::string db_host, std::string db_user, std::string db_passwd, std::string db_name, int port);
    MYSQL_STMT *prepare_stmt(std::string stmt);
    bool is_open() { return _conn_opened; }
    bool exec_prepare_stmt(MYSQL_STMT *mystmt);
    bool bind_params(MYSQL_STMT *mystmt, MYSQL_BIND binds[], int n);
    int num_of_fields(MYSQL_STMT *mystmt);
    int num_of_rows(MYSQL_STMT *mystmt);
    bool close_stmt(MYSQL_STMT *mystmt);
//protected:
    bool bind_results_(MYSQL_STMT *mystmt, MYSQL_BIND binds[]);
    bool store_results_(MYSQL_STMT *mystmt);
    bool fetch_row_(MYSQL_STMT *mystmt);

private:
    MYSQL *conn;
    std::string db_host_;
    std::string db_name_;
    std::string db_error;
    bool _conn_opened = false;

};


typedef unsigned long long ulonglong;
typedef unsigned short ushort;

class AtmDataAccesss {

public:
    virtual ~AtmDataAccesss() {}
    AtmDataAccesss(std::string db_host, std::string db_user, std::string db_passwd, std::string db_name, int port);
    AccntInfo * getAccntByID(ulonglong accnt_no);
    bool creatAccnt(struct AccntInfo &accnt);
    bool updateAccntBalance(ulonglong accnt_no, ulonglong amount);
    ulonglong getBalanceByID(ulonglong accnt_no, ushort type);


private:
    DataBaseAccess dataAc_;
    const std::string getAccntStmt = "SELECT * FROM accnt_info WHERE accnt_no=?";
};


#endif //CNP_ATM_DATA_ACCESS_H
