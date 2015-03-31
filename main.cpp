#include <iostream>
#include "CNP_Protocol.h"
using namespace std;

#include "socket.h"
#include "IOLoop.h"


int main () {

    ioloop::IOLoop ioloop(1024);
    ioloop.setup_listen_sock(INADDR_LOOPBACK, 5999);
    ioloop.enable_listning(1024);
    ioloop.start();

}




//
//extern "C" {
//    #include "mysql_client.h"
//    #include <string.h>
//    #include <stdlib.h>
//    #include <stdio.h>
//}
//
//
//#define QUERY_STMT "SELECT * FROM accnt_info"
//
//int main() {
//    MYSQL *conn;
//    MYSQL_RES *res;
//    MYSQL_ROW row;
//    MYSQL_BIND binds[3];
//    my_bool is_null[3];
//    my_bool errs[3];
//    unsigned long lens[3];
//    MYSQL_STMT *stmt = NULL;
//
//    int i;
//
//    conn = init_conn(DB_HOST, DB_USERNAME, DB_PASSWD, DB_NAME, 0);
//    if (!conn)
//        printf("1");
//    fprintf(stderr, "%s", db_error());
//
//    if ( !(stmt = init_stmt(conn, QUERY_STMT, strlen(QUERY_STMT))) ) {
//        fprintf(stderr, "%s", mysql_error(conn));
//        return 2;
//    }
//
//    int k = num_of_fields(stmt);
//    printf("num of fields: %d\n", k);
//
//    if (exec_stmt(stmt) == DB_ERROR) {
//        fprintf(stderr, "%s", db_error());
//    }
//
//    for ( i = 0; i < k; ++i) {
//        binds[i].buffer_type= MYSQL_TYPE_STRING;
//        binds[i].buffer = (char *)malloc(20);
//        binds[i].buffer_length= 20;
//        binds[i].is_null= &is_null[i];
//        binds[i].length= &lens[i];
//        binds[i].error= &errs[i];
//    }
//
//    if (bind_stmt_result(stmt, binds) == DB_ERROR) {
//        printf("%s\n", mysql_stmt_error(stmt));
//        return 2;
//    }
//
//    if ( store_result(stmt) ) {
//        printf("%s\n", mysql_stmt_error(stmt));
//        return 2;
//    }
//
//    while (!stmt_fetch_row(stmt)) {
//        for (i = 0; i < k; ++i) {
//            fprintf(stdout, "%s\t", binds[i].buffer);
//        }
//        fprintf(stdout, "\n");
//    }
//
//    mysql_stmt_close(stmt);
//    mysql_close(conn);
//
//    return 0;
//}

