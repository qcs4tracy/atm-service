//
// Created by tracymac on 3/30/15.
//

#ifndef CNP_ATM_SOCKET_H
#define CNP_ATM_SOCKET_H

extern "C" {
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
}

#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
using namespace std;

namespace sock {

    class Socket {

    public:
        virtual ~Socket() { this->close(); }
        Socket() {}

        Socket(int fd):sock_fd(fd) {
            if (fd > 0)
                initialized = true;
        }

        bool is_valid() {
            return initialized;
        }

        virtual void init() {}

        void set_sock_fd(int fd) {
            if (fd > 0) {
                this->sock_fd = fd;
                this->initialized = true;
            }
        };

        int get_sock_fd() { return sock_fd; }

        bool make_non_blocking(bool yes);
        bool close();

    protected:
        int sock_fd = -1;
        bool initialized = false;
    };


    class TCPSocket: public Socket {

    public:
        TCPSocket(){}
        TCPSocket(int fd):Socket(fd) { }
        virtual ~TCPSocket() {}

        virtual void init() {
            this->sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (this->sock_fd > 0)
                this->initialized = true;
        }

    protected:
        bool addr_set_ = false;
        struct sockaddr_in addr_;
        socklen_t sock_len;
    };

    class TCPCommunicateSocket;

    class TCPServerSocket: public TCPSocket {

    private:
        enum SOCK_STATE {INITIAL, BIND, LISTENING};
        SOCK_STATE state_;
        int port_;
        int accept_counter_ = 0;

    public:
        virtual ~TCPServerSocket(){}
        TCPServerSocket():TCPSocket() { init(); }
        bool bind(in_addr_t host, int port);
        bool listen(int backlog);
        TCPCommunicateSocket *accept();

    };

#define E_AGAIN -1
#define E_ERROR -2
#define BUFF_SIZE 512
    class TCPCommunicateSocket: public TCPSocket {

    public:
        virtual ~TCPCommunicateSocket(){ delete strm_in; }
        TCPCommunicateSocket() { this->strm_in = new istream(&buff_in); }
        TCPCommunicateSocket(int fd);
        TCPCommunicateSocket(int fd, struct sockaddr_in &addr, socklen_t len);
        istream &getIn() { return *this->strm_in; }
        ssize_t send(char *buf, size_t len);
        ssize_t recv();

    protected:
        stringbuf buff_in;
        stringbuf buff_out;
        istream *strm_in;
        char raw_buf_[BUFF_SIZE];
        size_t raw_buf_size_ = BUFF_SIZE;
        size_t nrecv_;
        bool pending_write = false;
    };


    class TCPClientSocket: public TCPCommunicateSocket {

    private:
        bool connected = false;

    public:
        virtual ~TCPClientSocket() {}

        TCPClientSocket() {
            init();
        }

        //bool connect(struct sockaddr_in *addr, socklen_t len);
        bool connect(in_addr_t host, int port);
    };

}



#endif //CNP_ATM_SOCKET_H
