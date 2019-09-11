#pragma once

#include <stack>
#include <queue>
#include <mutex>
#include <condition_variable>

extern "C" {
#include "libavutil/frame.h"
}

namespace simple_player {
    class AVFrameQueue {
    public:
        AVFrameQueue() = default;
        ~AVFrameQueue() = default;
        bool init(unsigned int max_queue_size);
        void de_init();
        AVFrame* get();
        void put(AVFrame *frame);
        void push(AVFrame *frame);
        AVFrame* pop();
    private:
        std::mutex stack_mutex_;
        std::stack<AVFrame*> stack_;
        std::condition_variable stack_not_empty_;
        std::mutex queue_mutex_;
        std::queue<AVFrame *> queue_;
        std::condition_variable queue_not_empty_;
    };
}
