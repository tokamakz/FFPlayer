#include "FrameQueue.h"

namespace simple_player {
    bool FrameQueue::init(unsigned int pool_size) {
        while(pool_size--) {
            AVFrame *frame = ::av_frame_alloc();
            stack_.push(frame);
        }
        return true;
    }

    void FrameQueue::de_init() {
    }

    AVFrame *FrameQueue::get() {
        std::lock_guard<std::mutex> lock(stack_mutex_);
        if (stack_.empty()) {
            stack_not_empty_.wait(queue_mutex_);
        }
        AVFrame* frame = stack_.top();
        stack_.pop();
        return frame;
    }

    void FrameQueue::put(AVFrame *frame) {
        std::lock_guard<std::mutex> lock(stack_mutex_);
        stack_.push(frame);
    }

    void FrameQueue::push(AVFrame *frame) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        queue_.push(frame);
        queue_not_empty_.notify_one();
    }

    AVFrame * FrameQueue::pop() {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        while(queue_.empty()) {
            queue_not_empty_.wait(queue_mutex_);
        }
        AVFrame *frame = queue_.front();
        queue_.pop();
        return frame;
    }
}

