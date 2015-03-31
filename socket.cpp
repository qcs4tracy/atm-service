//
// Created by tracymac on 3/30/15.
//

#include "socket.h"


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

sock::TCPCommunicateSocket::TCPCommunicateSocket(int fd): TCPSocket(fd) {
    this->strm_in = new istream(&buff_in);
}

sock::TCPCommunicateSocket::TCPCommunicateSocket(int fd, struct sockaddr_in &addr,
                                                 socklen_t len): TCPSocket(fd) {
    this->addr_ = addr;
    this->sock_len = len;
    this->strm_in = new istream(&buff_in);
}

ssize_t sock::TCPCommunicateSocket::recv() {

    char * buf = this->raw_buf_;
    size_t nrecv = 0;
    size_t buf_size = this->raw_buf_size_;

    do {

        nrecv = ::recv(this->sock_fd, buf, buf_size, 0);

        if (nrecv == 0) {/*the remote side close the connection*/
            return nrecv;
        } else if (nrecv > 0) {/*read from the stream*/
            this->buff_in.sputn(buf, nrecv);
            this->nrecv_ = nrecv;
            if (nrecv < buf_size) {
                return nrecv;
            }
            return E_AGAIN;
        }

    } while (errno == EINTR);

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
            /*log error*/
            return n;
        }

        if (errno == EINTR || errno == EAGAIN) {
            if (errno == EAGAIN)
                return E_AGAIN;
        } else {
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
        perror("accept() failed!");
    return NULL;
}


bool sock::TCPClientSocket::connect(in_addr_t host, int port) {

    addr_.sin_addr.s_addr = htonl(host);
    addr_.sin_port = htons(port);
    addr_.sin_family = AF_INET;
    sock_len = sizeof(addr_);

    if (is_valid()) {
        if (::connect(this->sock_fd, (struct sockaddr *) &addr_, sock_len) == 0) {
            connected = true;
            return true;
        }
    }

    return false;
}

