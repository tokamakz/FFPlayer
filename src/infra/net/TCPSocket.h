#pragma once

#include <monetary.h>

#include <string>
#include "BasicSocket.h"

namespace Infra {
namespace Net {
    class TCPSocket : public BasicSocket{
    public:
        TCPSocket();
        ~TCPSocket();
        int open() override;
        int listen(int backlog) const;
        int accept() const;
        ssize_t send(const void *buf, size_t len) const;
        ssize_t recv(void *buf, size_t nbyte) const;
    };
}
}
