//
// Created by tracymac on 4/2/15.
//

#include "AtmDataAccess.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "log4z.h"

using namespace atm_data;
using namespace std;
using namespace zsummer::log4z;

AtmDataAccess::AtmDataAccess(std::string db_host, std::string db_user, std::string db_passwd, std::string db_name,
                             int port): _db_host(db_host), _db_user(db_user), _port(port), _db_name(db_name) {

    try {
        sql::Driver *driver = get_driver_instance();
        string host = string(DB_PROTO) + "://" + DB_HOST + ":" + to_string(port);
        _conn = driver->connect(host, db_user, db_passwd);
        _conn->setSchema(db_name);
        _conn_opened = true;
    } catch (sql::SQLException &e) {
        LOGF("connecting mysql database failed:" << e.what());
        _conn_opened = false;
    }

}

void AtmDataAccess::startTransaction() {
    _conn->setAutoCommit(false);
}

void AtmDataAccess::rollback() {
    _conn->rollback();
}

void AtmDataAccess::commit() {

    _conn->commit();

    if (!_conn->getAutoCommit())
        _conn->setAutoCommit(true);
}




AccntInfo* AtmDataAccess::getAccntByID(ulonglong id_, uint_ id_type_) {

    sql::PreparedStatement *stmt;
    sql::ResultSet *res;
    AccntInfo *acc = NULL;

    static const string _qByAccntNo = "SELECT * FROM accnt_info WHERE accnt_no=?";
    static const string _qBySSN = "SELECT * FROM accnt_info WHERE ssn=?";

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

    try {

        if(id_type_ == 0) {
            stmt = _conn->prepareStatement(_qByAccntNo);
            stmt->setUInt64(1, id_);
        } else {
            stmt = _conn->prepareStatement(_qBySSN);
            stmt->setUInt(1, (uint_)id_);
        }

        res = stmt->executeQuery();

        if (res->next()) {
            string fn = res->getString(1);
            string ln = res->getString(2);
            ulonglong acctno_ = res->getUInt64(3);
            string eml =  res->getString(4);
            uint pin_ = res->getInt(5);
            uint ssn_ = res->getInt(5);
            uint dl_ = res->getInt(5);
            acc = new AccntInfo(fn.c_str(), ln.c_str(), eml.c_str(), ssn_, dl_, pin_, acctno_);
        }

        delete stmt;
        delete res;

        if(acc)
            return acc;
    }  catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }

    return NULL;

}


bool AtmDataAccess::createAccnt(const AccntInfo &acc) {

    sql::PreparedStatement *stmt;
    int updateCnt_;
    try {
        stmt = _conn->prepareStatement("INSERT INTO accnt_info(first_name, last_name, email, pin, ssn, DL) "
                                               "VALUES(?,?,?,?,?,?)");
        //bind the parameters
        stmt->setString(1, acc.first_name);
        stmt->setString(2, acc.last_name);
        stmt->setString(3, acc.email);
        stmt->setUInt(4, acc.PIN);
        stmt->setUInt(5, acc.ssn);
        stmt->setUInt(6, acc.DL);
        updateCnt_ = stmt->executeUpdate();
        delete stmt;

        return updateCnt_ > 0;
    } catch (sql::SQLException &e) {
        LOGE("create account for user[" << acc.first_name <<acc.last_name << "] failed:" << e.what());
        return false;
    }

}


bool AtmDataAccess::updateAccntBalance(ulonglong accnt_no, ulonglong amount, AccntBalance::ACCNT_TYPE type) {

    static const string stmtChk = "UPDATE accnt_balance SET check_bl=? WHERE accnt_no=?";
    static const string stmtCash = "UPDATE accnt_balance SET cash_bl=? WHERE accnt_no=?";
    sql::PreparedStatement *stmt;
    int updateCnt;

    try {

        if (type == AccntBalance::AT_CASH) {
            stmt = _conn->prepareStatement(stmtCash);
        } else {
            stmt = _conn->prepareStatement(stmtChk);
        }

        stmt->setUInt64(1, amount);
        stmt->setUInt64(2, accnt_no);
        updateCnt = stmt->executeUpdate() > 0;
        delete stmt;

        return updateCnt > 0;
    } catch (sql::SQLException &e) {
        LOGE("update account[" << accnt_no << "] balance failed:" << e.what());
        return false;
    }

}


AccntBalance* AtmDataAccess::getAccntBalance(ulonglong accnt_no) {

    sql::PreparedStatement *stmt;
    sql::ResultSet *res;
    AccntBalance* acc_bl = NULL;

    try {

        stmt = _conn->prepareStatement("SELECT cash_bl,check_bl FROM accnt_balance WHERE accnt_no=?");
        stmt->setUInt64(1, accnt_no);
        res = stmt->executeQuery();

        if (res->next())
            acc_bl = new AccntBalance(res->getUInt64(1), res->getUInt64(2));

        if (!acc_bl)
            LOGE("can not find account[" << accnt_no<< "]");

        return acc_bl;
    } catch (sql::SQLException &e) {
        LOGE("get balance of account[" << accnt_no<< "] failed:" << e.what());
        return NULL;
    }

}


bool AtmDataAccess::addTransRec(const TransRec &record) {

    if (record.type == TransRec::TT_INVALID) {
        LOGE("wrong transaction type for account[" << record.accnt_no << "]");
        return false;
    }

    sql::PreparedStatement *stmt;
    int updateCnt = 0;
    try {
        stmt = _conn->prepareStatement("INSERT INTO trans_rec (accnt_no, date, type, amount)"
                                               "VALUES(?, NOW(), ?, ?)");
        stmt->setUInt64(1, record.accnt_no);
        stmt->setInt(2, record.type);
        stmt->setUInt(3, record.amount);
        updateCnt = stmt->executeUpdate();

        delete stmt;
        return updateCnt > 0;

    } catch (sql::SQLException &e) {
        LOGE("insert transaction failed for account[" << record.accnt_no << "]:" << e.what());
        return false;
    }


}


bool AtmDataAccess::createBalanceRec(ulonglong accnt_no, ulonglong cash, ulonglong check) {

    sql::PreparedStatement *stmt;
    int updateCnt = 0;

    try {
        stmt = _conn->prepareStatement("INSERT INTO accnt_balance (accnt_no, cash_bl, check_bl) VALUES(?, ?, ?)");
        stmt->setUInt64(1, accnt_no);
        stmt->setUInt64(2, cash);
        stmt->setUInt64(3, check);

        updateCnt = stmt->executeUpdate();

        delete stmt;
        return updateCnt > 0;

    } catch (sql::SQLException &e) {
        LOGE("create balance record failed for account[" << accnt_no << "]:" << e.what());
        return false;
    }

}


TransRecSet* AtmDataAccess::getRecentTransRec(ulonglong accnt_no, uint_ cnt) {

    sql::PreparedStatement *stmt;
    sql::ResultSet *rs;
    TransRecSet *recSet = NULL;

    try {
        stmt = _conn->prepareStatement("SELECT id, type, amount, UNIX_TIMESTAMP(date) AS utc FROM trans_rec WHERE accnt_no=? "
                                               " ORDER BY date DESC LIMIT 0,?");
        stmt->setUInt64(1, accnt_no);
        stmt->setUInt(2, cnt);

        rs = stmt->executeQuery();
        cnt = cnt > rs->rowsCount()? rs->rowsCount(): cnt;
        recSet = new TransRecSet(cnt);

        int i = 0;
        while (rs->next()) {
            recSet->records[i++] = TransRec(rs->getInt(1), accnt_no, (TransRec::TRANS_TYPE)rs->getInt(2),
                                               rs->getUInt64(4), rs->getUInt(3));
        }

        delete stmt;
        delete rs;

        return recSet;
    } catch (sql::SQLException &e) {
        LOGE("query transaction records for account[" << accnt_no << "] failed:" << e.what());
        return NULL;
    }

}

//deep copy
AccntInfo::AccntInfo(const AccntInfo &rhs): accnt_no(rhs.accnt_no), ssn(rhs.ssn), PIN(rhs.PIN), DL(rhs.DL)  {
    ::strncpy(first_name, rhs.first_name, MAXLEN);
    ::strncpy(last_name, rhs.last_name, MAXLEN);
    ::strncpy(email, rhs.email, MAXLEN);
}

void AccntInfo::swap_(AccntInfo &rhs) {
    std::swap(first_name, rhs.first_name);
    std::swap(last_name, rhs.last_name);
    std::swap(email, rhs.email);
    accnt_no = rhs.accnt_no;
    ssn = rhs.ssn;
    PIN = rhs.PIN;
    DL = rhs.DL;
}

AccntInfo& AccntInfo::operator=(AccntInfo rhs) {
    swap_(rhs);
    return *this;
}
