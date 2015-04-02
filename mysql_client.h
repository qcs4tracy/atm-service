//
// Created by tracymac on 3/29/15.
//

#ifndef ATM_SERVICE_SOURCE_FILE_H
#define ATM_SERVICE_SOURCE_FILE_H

#include <mysql/mysql.h>

#define DB_HOST "104.131.6.249"
#define DB_USERNAME "qcs4tracy"
#define DB_PASSWD "qcs6426zqq"
#define DB_NAME "atm"

#define DB_ERROR -1
#define DB_OK 0

extern char *db_error();

MYSQL *init_conn(char const *host, char const *usrname, char const *passwd,
                 char const *dbname, unsigned int port);

MYSQL_STMT *init_stmt(MYSQL *conn, char const *stmt, size_t len);

int bind_stmt_params(MYSQL_STMT *mystmt, MYSQL_BIND binds[], int n);

int num_of_fields(MYSQL_STMT *mystmt);

int num_of_rows(MYSQL_STMT *mystmt);

int exec_stmt(MYSQL_STMT *mystmt);

int bind_stmt_result(MYSQL_STMT *mystmt, MYSQL_BIND binds[]);

int store_result(MYSQL_STMT *mystmt);

int stmt_fetch_row(MYSQL_STMT *mystmt);

int close_stmt(MYSQL_STMT *mystmt);

#endif //ATM_SERVICE_SOURCE_FILE_H
