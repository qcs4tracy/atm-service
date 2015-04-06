//
// Created by tracymac on 4/1/15.
//

#include "data_access.h"
#include "log4z.h"
#include <iostream>
using namespace zsummer::log4z;

DataBaseAccess::~DataBaseAccess() { ::mysql_close(conn); }
DataBaseAccess::DataBaseAccess(std::string db_host, std::string db_user, std::string db_passwd, std::string db_name,
                               int port): db_host_(db_host), db_name_(db_name) {
    open_conn(db_host, db_user, db_passwd, db_name, port);
}

bool DataBaseAccess::open_conn(std::string db_host, std::string db_user, std::string db_passwd, std::string db_name,
                               int port) {
    conn = ::init_conn(db_host.c_str(), db_user.c_str(), db_passwd.c_str(), db_name.c_str(), port);
    if (conn) {
        _conn_opened = true;
        return true;
    }
    LOGF("open connection to database " << db_name << "@" << db_host << " failed!:" << ::db_error());
    return false;
}


MYSQL_STMT* DataBaseAccess::prepare_stmt(std::string stmt) {

    MYSQL_STMT* rstmt;

    if (!_conn_opened) {
        LOGE("can not open statement on closed connection!:" << ::db_error());
        return NULL;
    }

    rstmt = ::init_stmt(conn, stmt.c_str(), stmt.size() + 1);
    if (!rstmt)
        LOGE("can not initialize prepare statement (" << stmt << ") :" << ::db_error());

    return rstmt;
}


bool DataBaseAccess::bind_params(MYSQL_STMT *mystmt, MYSQL_BIND binds[], int n) {

    if (::bind_stmt_params(mystmt, binds, n) == DB_ERROR) {
        LOGW("bind parameters error:" << ::db_error());
        return false;
    }

    return true;
}

bool DataBaseAccess::exec_prepare_stmt(MYSQL_STMT *mystmt) {

    if (::exec_stmt(mystmt) == DB_ERROR) {
        LOGE("execute statement failed:" << ::db_error());
        return false;
    }

    return true;
}

int DataBaseAccess::num_of_fields(MYSQL_STMT *mystmt) {
    int n = ::num_of_fields(mystmt);
    if (n == DB_ERROR)
        LOGE("can not fetch # of fields:" << ::db_error());
    return n > 0? n: 0;
}

int DataBaseAccess::num_of_rows(MYSQL_STMT *mystmt) {
    int n = ::num_of_rows(mystmt);
    if (n == DB_ERROR)
        LOGE("can not fetch # of rows:" << ::db_error());
    return n > 0? n: 0;
}


bool DataBaseAccess::bind_results_(MYSQL_STMT *mystmt, MYSQL_BIND binds[]) {
    return ::bind_stmt_result(mystmt, binds) == DB_OK;
}


bool DataBaseAccess::store_results_(MYSQL_STMT *mystmt) {

    if (::store_result(mystmt) == DB_ERROR) {
        LOGE("store result error:" << ::db_error());
        return false;
    }
    return true;
}

bool DataBaseAccess::fetch_row_(MYSQL_STMT *mystmt) {
    return ::stmt_fetch_row(mystmt) == DB_OK;
}

bool DataBaseAccess::close_stmt(MYSQL_STMT *mystmt) {
    if (::close_stmt(mystmt) == DB_ERROR){
        LOGE("close statement failed:" << ::db_error());
        return false;
    }
    return true;
}

AtmDataAccesss::AtmDataAccesss(std::string db_host, std::string db_user, std::string db_passwd, std::string db_name,
                               int port): dataAc_(db_host, db_user, db_passwd, db_name, port) {}

struct AccntInfo* AtmDataAccesss::getAccntByID(ulonglong accnt_no) {

    struct AccntInfo * accntInfo = new struct AccntInfo(NULL, NULL, NULL);
    MYSQL_STMT *stmt1 = dataAc_.prepare_stmt(getAccntStmt);

    //parameter bind
    MYSQL_BIND acn[1];
    //result binds
    ::MYSQL_BIND binds[7];
    my_bool is_null[7];
    my_bool errs[7];
    unsigned long lens[7];

    //bind parameter
    acn[0].buffer_type = MYSQL_TYPE_LONGLONG;
    acn[0].buffer = (char *)&accnt_no;
    acn[0].is_null = 0;
    acn[0].length = 0;
    dataAc_.bind_params(stmt1, acn, 1);

    if (!dataAc_.exec_prepare_stmt(stmt1)) {
        delete accntInfo;
        return NULL;
    }

//                          Account Information Table
//    +------------+---------------------+------+-----+---------+----------------+
//    | Field      | Type                | Null | Key | Default | Extra          |
//    +------------+---------------------+------+-----+---------+----------------+
//    | first_name | varchar(50)         | NO   |     | NULL    |                |
//    | last_name  | varchar(50)         | NO   |     | NULL    |                |
//    | accnt_no   | bigint(20) unsigned | NO   | PRI | NULL    | auto_increment |
//    | email      | varchar(60)         | YES  |     | NULL    |                |
//    | pin        | int(10) unsigned    | YES  |     | NULL    |                |
//    | ssn        | int(10) unsigned    | YES  |     | NULL    |                |
//    | DL         | int(10) unsigned    | YES  |     | NULL    |                |
//    +------------+---------------------+------+-----+---------+----------------+

    binds[0].buffer_type = MYSQL_TYPE_STRING;
    binds[0].buffer = (char *)accntInfo->first_name;
    binds[0].buffer_length = MAXLEN;
    binds[0].is_null = &is_null[0];
    binds[0].length = &lens[0];
    binds[0].error = &errs[0];

    binds[1].buffer_type = MYSQL_TYPE_STRING;
    binds[1].buffer = (char *)accntInfo->last_name;
    binds[1].buffer_length = MAXLEN;
    binds[1].is_null = &is_null[1];
    binds[1].length = &lens[1];
    binds[1].error = &errs[1];

    //BIGINT COLUMN
    binds[2].buffer_type = MYSQL_TYPE_LONGLONG;
    binds[2].buffer = (char *)&accntInfo->accnt_no;
    binds[2].is_null = &is_null[2];
    binds[2].length = &lens[2];
    binds[2].error = &errs[2];


    binds[3].buffer_type = MYSQL_TYPE_STRING;
    binds[3].buffer = accntInfo->email;
    binds[3].buffer_length = MAXLEN;
    binds[3].is_null = &is_null[3];
    binds[3].length = &lens[3];
    binds[3].error = &errs[3];


    /* INTEGER COLUMN */
    binds[4].buffer_type= MYSQL_TYPE_LONG;
    binds[4].buffer= (char *)&accntInfo->PIN;
    binds[4].is_null= &is_null[4];
    binds[4].length= &lens[4];
    binds[4].error= &errs[4];

    /* INTEGER COLUMN */
    binds[5].buffer_type= MYSQL_TYPE_LONG;
    binds[5].buffer= (char *)&accntInfo->ssn;
    binds[5].is_null= &is_null[5];
    binds[5].length= &lens[5];
    binds[5].error= &errs[5];

    /* INTEGER COLUMN */
    binds[6].buffer_type= MYSQL_TYPE_LONG;
    binds[6].buffer= (char *)&accntInfo->DL;
    binds[6].is_null= &is_null[6];
    binds[6].length= &lens[6];
    binds[6].error= &errs[6];

    dataAc_.bind_results_(stmt1, binds);
    dataAc_.store_results_(stmt1);

    if (!dataAc_.fetch_row_(stmt1)) {
        delete accntInfo;
        return NULL;
    }

    return accntInfo;
}


//for database test
int main () {

    ILog4zManager::getInstance()->start();
    AtmDataAccesss da(DB_HOST, DB_USERNAME, DB_PASSWD, DB_NAME, 0);
    struct AccntInfo *acc = da.getAccntByID(4388576071061090);

    if(acc)
        std::cout << acc->first_name << " " << acc->last_name << " (" << acc->accnt_no << ")" << std::endl;

    delete acc;
    return 0;
}























