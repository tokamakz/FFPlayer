#include "BasicSocket.h"

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <cstring>

namespace Infra {
namespace Net {
    BasicSocket::BasicSocket()
        : socket_fd_(-1) {
    }

    BasicSocket::~BasicSocket() {
    }

    int BasicSocket::connect(const char *ip_addr, int port) const {
        struct sockaddr_in t_sockaddr;
        std::memset(&t_sockaddr, 0, sizeof(t_sockaddr));

        t_sockaddr.sin_family = AF_INET;
        t_sockaddr.sin_port = htons(port);
        inet_pton(AF_INET, ip_addr, &t_sockaddr.sin_addr);

        return ::connect(socket_fd_, (struct sockaddr*)&t_sockaddr, sizeof(struct sockaddr));
    }

    int BasicSocket::bind(int port) const {
        struct sockaddr_in t_sockaddr;
        std::memset(&t_sockaddr, 0, sizeof(t_sockaddr));

        t_sockaddr.sin_family = AF_INET;
        t_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        t_sockaddr.sin_port = htons(port);

        return ::bind(socket_fd_, (struct sockaddr *)&t_sockaddr, sizeof(t_sockaddr));
    }

    int BasicSocket::close() {
        ::close(socket_fd_);
        socket_fd_ = -1;
        return 0;
    }
}
}
