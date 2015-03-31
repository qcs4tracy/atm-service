//
// Created by tracymac on 3/30/15.
//

#include "IOLoop.h"
#include "CNP_Protocol.h"

void ioloop::EventsManager::init(int nev) {

    if (nev > 0) {
        epl = epoll_create(nev);
        this->events = new ::epoll_event[nev];
        if(this->events && epl > 0) {
            this->initialized = true;
            this->nevents = nev;
        }
    };
}

bool ioloop::EventsManager::add_read_ev(int fd, sock::Socket *sk) {

    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = sk;

    if (initialized && fd > 0) {
        return epoll_ctl(epl, EPOLL_CTL_ADD, fd, &ev) == 0;
    }

    return false;
}

bool ioloop::IOLoop::setup_listen_sock(in_addr_t host, int port) {

    if (!this->listen.make_non_blocking(true)) {
        perror("make_non_blocking() failed");
        return false;
    }

    int opval = 1;
    setsockopt(this->listen.get_sock_fd(), SOL_SOCKET, SO_REUSEADDR, (const void *) &opval, sizeof(opval));

    if (!this->listen.bind(INADDR_LOOPBACK, 5999)) {
        perror("bind() failed!");
        return false;
    }

    return true;
}

bool ioloop::IOLoop::enable_listning(int backlog) {

    if (!this->listen.listen(backlog)) {
        perror("listen() failed!");
        return false;
    }

    return eventsM_.add_read_ev(this->listen.get_sock_fd(), &this->listen);
}


void ioloop::IOLoop::start() {

    int nev;
    epoll_event *events, ev;
    events = eventsM_.events;
    sock::TCPCommunicateSocket *cl_sk;

     while(true) {

        nev = epoll_wait(eventsM_.epl, events, eventsM_.nevents, -1);
        for (int i = 0; i < nev; ++i) {

            if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                /**/
                cl_sk = (sock::TCPCommunicateSocket *) events[i].data.ptr;
                delete cl_sk;
                continue;
            }

            if (events[i].events & EPOLLIN) {//read events

                if (events[i].data.ptr == (&this->listen)) {

                    while (true) {
                        cl_sk = this->listen.accept();
                        /*accept no connection*/
                        if (!cl_sk)
                            break;

                        cl_sk->make_non_blocking(true);
                        eventsM_.add_read_ev(cl_sk->get_sock_fd(), cl_sk);
                    }

                } else {

                    cnp::CONNECT_REQUEST cq;
                    cl_sk = (sock::TCPCommunicateSocket *) events[i].data.ptr;

                    int nr = cl_sk->recv();

                    if (nr > 0) {
                        istream *s = cl_sk->getIn();
                        s->read(reinterpret_cast<char *>(&cq), sizeof(cq));
                        cout << cq.m_Hdr.m_wClientID << " " << cq.m_Request.m_dwValidationKey << endl;
                    }

                    if (nr == 0) {
                        delete cl_sk;
                    }

                    if (nr == E_ERROR) {
                        perror("read error");
                        delete cl_sk;
                    }

                }
            }

        }

    }

}

