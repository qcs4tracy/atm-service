//
// Created by tracymac on 3/30/15.
//

#ifndef CNP_ATM_IOLOOP_H
#define CNP_ATM_IOLOOP_H

extern "C" {
    #include <sys/epoll.h>
    #include <unistd.h>
}

#include "socket.h"
#include "proto_impl.h"

namespace ioloop {

    class IOLoop;

    class EventsManager {

    friend class IOLoop;

    public:
        virtual ~EventsManager() {
            if (events != NULL) {
                delete events;
            }
            if (epl > 0)
                close(epl);
        }

        EventsManager() { }

        EventsManager(int nev) { init(nev); }

        void init(int nev);
        bool add_read_ev(int fd, sock::Socket *sk);
        void add_write_ev(int fd, sock::Socket *sk) {}
        void remove_ev(int fd) {}

    private:
        /*epoll handle*/
        epoll_event *events = NULL;
        int epl = -1;
        int nevents = 0;
        bool initialized = false;
    };


    class IOLoop {

    public:
        IOLoop(int nev = 1024): eventsM_(nev) {}
        bool setup_listen_sock(in_addr_t host, int port);
        bool enable_listning(int backlog);
        void start();

    private:
        sock::TCPServerSocket listen;
        EventsManager eventsM_;
        CNProtoHandler handler_;
    };



}
#endif //CNP_ATM_IOLOOP_H
