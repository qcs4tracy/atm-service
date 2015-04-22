#include <iostream>
#include "AtmDataAccess.h"
using namespace std;
using namespace atm_data;
#include "socket.h"
#include "IOLoop.h"
#include "log4z.h"

using namespace zsummer::log4z;

int main () {

    ILog4zManager::getInstance()->start();
    ioloop::IOLoop ioloop(1024);
    ioloop.setup_listen_sock(INADDR_ANY, 5999);
    ioloop.enable_listning(1024);
    ioloop.start();

    return 0;
}

