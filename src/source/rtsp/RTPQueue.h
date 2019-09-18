#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

#include "RTPPacket.h"

namespace simple_player {
    class RTPQueue {
    public:
        RTPQueue();
        ~RTPQueue();
        bool init(int max_queue_size);
        void de_init();
        void push(RTPPacket *pkt);
        RTPPacket* pop();

    private:
        int max_queue_size_;
        std::queue<RTPPacket*> queue_;
        std::condition_variable not_empty_;
        std::condition_variable not_full_;
        std::mutex queue_mutex_;
    };
}