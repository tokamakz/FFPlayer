#pragma once

#include <queue>
#include <mutex>

extern "C" {
#include "libavcodec/avcodec.h"
}

namespace simple_player {
    class PacketQueue {
    public:
        PacketQueue();
        ~PacketQueue();
        bool start();
        void send_packet(AVPacket *pkt);
        virtual void receive_packet(AVPacket *pkt) = 0;

    private:
        std::queue<AVPacket *> pkt_queue_;
        std::mutex queue_mutex_;
    };
}
