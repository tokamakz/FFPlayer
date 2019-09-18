#pragma once

namespace Infra {
namespace Net {
    class BasicSocket {
    public:
        BasicSocket();
        virtual ~BasicSocket();
        virtual int connect(const char *ip_addr, int port) const;
        virtual int bind(int port) const;
        virtual int open() = 0;
        virtual int close();
    protected:
        int socket_fd_;
    };
}
}
