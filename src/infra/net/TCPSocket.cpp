#include "TCPSocket.h"

#include <sys/socket.h>

#include <cstring>

namespace Infra {
namespace Net {
    TCPSocket::TCPSocket() {}

    TCPSocket::~TCPSocket() {}

    int TCPSocket::open() {
        return socket_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    }

    int TCPSocket::listen(int backlog) const {
        return ::listen(socket_fd_, backlog);
    }

    int TCPSocket::accept() const {
        return ::accept(socket_fd_, (struct sockaddr*)nullptr, nullptr);
    }

    ssize_t TCPSocket::send(const void *buf, size_t len) const {
        return ::send(socket_fd_, buf, len, 0);
    }

    ssize_t TCPSocket::recv(void *buf, size_t nbyte) const {
        return ::recv(socket_fd_, buf, nbyte, 0);
    }
}
}
