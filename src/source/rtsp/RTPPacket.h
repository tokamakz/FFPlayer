#pragma once

#include "common.h"

namespace simple_player {
    class RTPPacket {
    public:
        unsigned char *buf;
        unsigned int len;
        explicit RTPPacket() {
            buf = new unsigned char[RTSP_MTU];
            len = 0u;
        }

        ~RTPPacket() {
            safe_deletepa(buf);
            len = 0u;
        }
        static const unsigned int RTSP_MTU = 1500;
        static const unsigned char RTP_VERSION = 2;
    };
}
