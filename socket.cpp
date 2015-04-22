//
// Created by tracymac on 3/30/15.
//

#include "socket.h"
#include "log4z.h"
#include <arpa/inet.h>
#include <cstring>

using namespace zsummer::log4z;

bool sock::Socket::close() { return ::close(this->sock_fd) == 0; }

bool sock::Socket::make_non_blocking(bool yes)
{
    int flags;

    if ((flags = fcntl (this->sock_fd, F_GETFL, 0)) < 0) {
        return false;
    }

    flags &= ~O_NONBLOCK;

    if (yes)
        flags |= O_NONBLOCK;

    if (fcntl (this->sock_fd, F_SETFL, flags) < 0) {
        return false;
    }

    return true;
}

sock::TCPCommunicateSocket::TCPCommunicateSocket(int fd): buff_in(new stringbuf),
                                                          buff_out(new stringbuf),TCPSocket(fd) {}

sock::TCPCommunicateSocket::TCPCommunicateSocket(int fd, struct sockaddr_in &addr,
                                                 socklen_t len): buff_in(new stringbuf),
                                                                 buff_out(new stringbuf), TCPSocket(fd) {
    this->addr_ = addr;
    this->sock_len = len;
}

sock::TCPCommunicateSocket::~TCPCommunicateSocket() {
    delete buff_in.rdbuf();
    delete buff_out.rdbuf();
}


ssize_t sock::TCPCommunicateSocket::recv() {

    char * buf = this->raw_buf_;
    size_t nrecv = 0;
    size_t buf_size = this->raw_buf_size_;
    this->nrecv_ = 0;
    bool _again = false;

    do {

        nrecv = ::recv(this->sock_fd, buf, buf_size, 0);

        if (nrecv == 0) {/*the remote side close the connection*/

            return this->nrecv_;

        } else if (nrecv > 0) {/*read from the stream*/

            this->buff_in.rdbuf()->sputn(buf, nrecv);
            this->nrecv_ += nrecv;

            if (nrecv < buf_size) {//all data read
                return this->nrecv_;
            }
            //probably still have data to read
            _again = true;
        }

    } while (errno == EINTR || _again);

    LOGE("receive data error:" << std::strerror(errno));
    return E_ERROR;
}


ssize_t sock::TCPCommunicateSocket::send(char *buf, size_t len) {

    size_t n;

    for (;;) {

        n = ::send(this->sock_fd, buf, len, 0);

        if (n > 0) {
            return n;
        }

        if (n == 0) {
            LOGE("no data being write.");
            return n;
        }

        if (errno == EINTR || errno == EAGAIN) {
            if (errno == EAGAIN) {
                /*buffer the data for latter write*/
                buff_out.rdbuf()->sputn(buf, len);
                pending_write = true;
                return E_AGAIN;
            }
        } else {
            LOGE("write socket error:" << strerror(errno));
            return E_ERROR;
        }

    }

}


bool sock::TCPServerSocket::bind(in_addr_t host, int port) {

    if (!is_valid())
        return false;
    this->addr_.sin_family = AF_INET;
    this->addr_.sin_addr.s_addr = htonl(host);
    this->addr_.sin_port = htons(port);
    this->sock_len = sizeof(this->addr_);
    this->addr_set_ = true;
    bool ret = ::bind(this->sock_fd, (struct sockaddr*)&this->addr_, this->sock_len) == 0;
    if (ret) {
        state_ = BIND;
        return ret;
    }
    return false;
}

bool sock::TCPServerSocket::listen(int backlog) {
    if (is_valid())
        return ::listen(this->sock_fd, backlog) == 0;
    return false;
}


sock::TCPCommunicateSocket* sock::TCPServerSocket::accept() {
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);
    int cfd = ::accept(this->sock_fd, (struct sockaddr *)&caddr, &len);
    if (cfd > 0) {
        return new TCPCommunicateSocket(cfd, caddr, len);
    }
    if (errno != EWOULDBLOCK && errno != EAGAIN)
        LOGE("accept() failed.");
    return NULL;
}

// Function to fill in address structure given an address and port
static void fillAddr(const string &address, unsigned short port,
                     sockaddr_in &addr) {
    memset(&addr, 0, sizeof(addr));  // Zero out address structure
    addr.sin_family = AF_INET;       // Internet address

    if (!inet_aton(address.c_str(), &addr.sin_addr)) {
        LOGE("Invalid IP address: " << address);
    }

    addr.sin_port = htons(port);     // Assign port in network byte order
}

bool sock::TCPClientSocket::connect(std::string host, unsigned short port) {

    fillAddr(host, port, addr_);
    sock_len = sizeof(addr_);

    if (is_valid()) {
        if (::connect(this->sock_fd, (struct sockaddr *) &addr_, sock_len) == 0) {
            connected = true;
            return true;
        }
    }

    return false;
}

