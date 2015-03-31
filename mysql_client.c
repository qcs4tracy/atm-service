//
// Created by tracymac on 3/29/15.
//

#include "mysql_client.h"
#include <stdio.h>
#include <stdlib.h>

char err_msg[512];

#define SET_ERROR_MSG(fmt, ...) sscanf(err_msg, fmt, ##__VA_ARGS__)

char *db_error() {
    return err_msg;
}

MYSQL *init_conn(char const *host, char const *usrname, char const *passwd,
              char const *dbname, unsigned int port) {
    MYSQL *conn;

    if ( !(conn = mysql_init(NULL)) ) {
        SET_ERROR_MSG("mysql_init() failed: %s\n", mysql_error(conn));
        return NULL;
    }

    if ( !mysql_real_connect(conn, host, usrname, passwd, dbname, port, NULL, 0)) {
        SET_ERROR_MSG("mysql_real_connect() failed: %s\n", mysql_error(conn));
    }

    return conn;

}

MYSQL_STMT *init_stmt(MYSQL *conn, char const *stmt, size_t len) {

    MYSQL_STMT *mystmt;

    if (!(mystmt = mysql_stmt_init(conn))) {
        SET_ERROR_MSG("mysql_stmt_init() failed: %s\n", mysql_stmt_error(mystmt));
        return NULL;
    }

    if (mysql_stmt_prepare(mystmt, stmt, len) != 0) {
        SET_ERROR_MSG("mysql_stmt_prepare() failed: %s\n", mysql_stmt_error(mystmt));
        return NULL;
    }

    return mystmt;
}

int bind_stmt_params(MYSQL_STMT *mystmt, MYSQL_BIND binds[], int n) {

    int paramc = mysql_stmt_param_count(mystmt);
    if ( n < 0 || paramc != n) {
        SET_ERROR_MSG("Incorrect parameter count. expected %d, but given %d\n", paramc, n);
        return DB_ERROR;
    }

    if (mysql_stmt_bind_param(mystmt, binds) != 0)
        SET_ERROR_MSG("mysql_stmt_bind_param() failed: %s\n", mysql_stmt_error(mystmt));

    return DB_OK;
}

int num_of_fields(MYSQL_STMT *mystmt) {

    /* Fetch result set meta information */
    MYSQL_RES *prepare_meta_result = mysql_stmt_result_metadata(mystmt);
    int c;

    if (!prepare_meta_result) {
        SET_ERROR_MSG("mysql_stmt_result_metadata() failed: %s\n", mysql_stmt_error(mystmt));
        return DB_ERROR;
    }

    /* Get total columns in the query */
    c = mysql_num_fields(prepare_meta_result);
    mysql_free_result(prepare_meta_result);

    return c;
}


int exec_stmt(MYSQL_STMT *mystmt) {

    if (mysql_stmt_execute(mystmt)) {
        SET_ERROR_MSG("mysql_stmt_execute() failed: %s\n", mysql_stmt_error(mystmt));
        return DB_ERROR;
    }

    return DB_OK;
}


int bind_stmt_result(MYSQL_STMT *mystmt, MYSQL_BIND binds[]) {

    if (mysql_stmt_bind_result(mystmt, binds)) {
        SET_ERROR_MSG("mysql_stmt_bind_result() failed: %s\n", mysql_stmt_error(mystmt));
        return DB_ERROR;
    }

    return DB_OK;
}

int store_result(MYSQL_STMT *mystmt) {

    if (mysql_stmt_store_result(mystmt)) {
        SET_ERROR_MSG("mysql_stmt_store_result() failed: %s\n", mysql_stmt_error(mystmt));
        return DB_ERROR;
    }

    return DB_OK;
}

int stmt_fetch_row(MYSQL_STMT *mystmt) {

    if (mysql_stmt_fetch(mystmt)) {
        SET_ERROR_MSG("mysql_stmt_fetch() failed: %s\n", mysql_stmt_error(mystmt));
        return DB_ERROR;
    }

    return DB_OK;

}



