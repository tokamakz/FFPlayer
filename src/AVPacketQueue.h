#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

extern "C" {
#include "libavcodec/avcodec.h"
}

namespace simple_player {
    class AVPacketQueue {
    public:
        AVPacketQueue();
        ~AVPacketQueue();
        bool init(int max_queue_size);
        void de_init();
        void push(AVPacket *pkt);
        AVPacket * pop();

    private:
        int max_queue_size_;
        std::queue<AVPacket *> queue_;
        std::condition_variable_any not_empty_;
        std::condition_variable_any not_full_;
        std::mutex queue_mutex_;
    };
}