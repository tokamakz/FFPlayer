#include "AVFrameQueue.h"


namespace simple_player {
    bool AVFrameQueue::init(unsigned int max_queue_size) {
        while(max_queue_size--) {
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

    //申请
    AVFrame* AVFrameQueue::get() {
        std::unique_lock<std::mutex> locker(stack_mutex_);
        stack_not_empty_.wait(locker, [this]{return !stack_.empty();});
        AVFrame* frame = stack_.top();
        stack_.pop();
        return frame;
    }

    //回收
    void AVFrameQueue::put(AVFrame *frame) {
        std::lock_guard<std::mutex> locker(stack_mutex_);
        stack_.push(frame);
        stack_not_empty_.notify_one();
    }

    //解码后放到队列里面去；
    void AVFrameQueue::push(AVFrame *frame) {
        std::lock_guard<std::mutex> locker(queue_mutex_);
        queue_.push(frame);
        queue_not_empty_.notify_one();
    }

    //渲染前从队列里取出来；
    AVFrame* AVFrameQueue::pop() {
        std::unique_lock<std::mutex> locker(queue_mutex_);
        queue_not_empty_.wait(locker, [this]{return !queue_.empty();});
        AVFrame *frame = queue_.front();
        queue_.pop();
        return frame;
    }
}

