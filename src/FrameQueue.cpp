#include "FrameQueue.h"

namespace simple_player {
    void FrameQueue::push(AVFrame *frame) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        while(queue_.size() >= max_queue_size_) {
            not_full_.wait(queue_mutex_);
        }
        queue_.push(frame);
        not_empty_.notify_one();
    }


    AVFrame * FrameQueue::pop() {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        while(queue_.empty()) {
            not_empty_.wait(queue_mutex_);
        }
        AVFrame *frame = queue_.front();
        queue_.pop();
        not_full_.notify_one();
        return frame;
    }

    bool FrameQueue::init(int max_queue_size) {
        max_queue_size_ = max_queue_size;
        return true;
    }

    void FrameQueue::de_init() {
        return ;
    }
}

