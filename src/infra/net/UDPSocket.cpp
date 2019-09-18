#include "UDPSocket.h"

#include <sys/socket.h>
#include <arpa/inet.h>

#include <cstring>

namespace Infra {
namespace Net {
    UDPSocket::UDPSocket() {}

    UDPSocket::~UDPSocket() {}

    int UDPSocket::open() {
        return socket_fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    }

    ssize_t UDPSocket::sendto(const void *buf, size_t len, const std::string&& ip_addr, unsigned int port) const {
        if (ip_addr.empty()) {
            return ::sendto(socket_fd_, buf, len, 0, (struct sockaddr *)nullptr, 0);
        }

        struct sockaddr_in t_sockaddr;
        std::memset(&t_sockaddr, 0, sizeof(t_sockaddr));

        t_sockaddr.sin_family = AF_INET;
        t_sockaddr.sin_port = htons(port);
        t_sockaddr.sin_addr.s_addr = inet_addr(ip_addr.c_str());

        return ::sendto(socket_fd_, buf, len, 0, (struct sockaddr *) &t_sockaddr, sizeof(t_sockaddr));
    }

    ssize_t UDPSocket::recvfrom(void *buf, size_t nbyte, std::string&& ip_addr, unsigned int port) const {
        struct sockaddr_in t_sockaddr;
        std::memset(&t_sockaddr, 0, sizeof(t_sockaddr));

        socklen_t src_addr_len = sizeof(t_sockaddr);
        ssize_t recv_len = ::recvfrom(socket_fd_, buf, nbyte, 0, (struct sockaddr*)&t_sockaddr, &src_addr_len);
        if (recv_len >= 0) {
            ip_addr.assign(::inet_ntoa(t_sockaddr.sin_addr));
            port = ::ntohs(t_sockaddr.sin_port);
        }
        return recv_len;
    }
}
}
