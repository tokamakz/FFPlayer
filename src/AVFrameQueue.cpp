#include "AVFrameQueue.h"

namespace simple_player {
    bool AVFrameQueue::init(unsigned int pool_size) {
        while(pool_size--) {
            AVFrame *frame = ::av_frame_alloc();
            stack_.push(frame);
        }
        return true;
    }

    void AVFrameQueue::de_init() {
        while(!stack_.empty()) {
            AVFrame* frame = stack_.top();
            ::av_frame_free(&frame);
            stack_.pop();
        }
    }

    AVFrame* AVFrameQueue::get() {
        std::unique_lock<std::mutex> lock(stack_mutex_);
        stack_not_empty_.wait(queue_mutex_, [this]{return !stack_.empty();});
        AVFrame* frame = stack_.top();
        stack_.pop();
        return frame;
    }

    void AVFrameQueue::put(AVFrame *frame) {
        std::lock_guard<std::mutex> lock(stack_mutex_);
        stack_.push(frame);
        stack_not_empty_.notify_one();
    }

    void AVFrameQueue::push(AVFrame *frame) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        queue_.push(frame);
        queue_not_empty_.notify_one();
    }

    AVFrame* AVFrameQueue::pop() {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        queue_not_empty_.wait(queue_mutex_, [this]{return !queue_.empty();});
        AVFrame *frame = queue_.front();
        queue_.pop();
        return frame;
    }
}

