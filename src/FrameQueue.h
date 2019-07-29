#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

extern "C" {
#include "libavutil/frame.h"
}

namespace simple_player {
    class FrameQueue {
    public:
        FrameQueue() {}
        ~FrameQueue(){};
        bool init(int max_queue_size);
        void de_init();
        void push(AVFrame *frame);
        AVFrame * pop();

    private:
        int max_queue_size_;
        std::queue<AVFrame *> queue_;
        std::condition_variable_any not_empty_;
        std::condition_variable_any not_full_;
        std::mutex queue_mutex_;
    };
}
