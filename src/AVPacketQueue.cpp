#include "AVPacketQueue.h"

namespace simple_player {
    AVPacketQueue::AVPacketQueue() {
        max_queue_size_ = 0;
    }

    AVPacketQueue::~AVPacketQueue() = default;

    bool AVPacketQueue::init(int max_queue_size) {
        max_queue_size_ = max_queue_size;
        return true;
    }

    void AVPacketQueue::de_init() {
        return ;
    }

    void AVPacketQueue::push(AVPacket *pkt) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        while(queue_.size() >= max_queue_size_) {
            not_full_.wait(queue_mutex_);
        }
        queue_.push(pkt);
        not_empty_.notify_one();
    }

    AVPacket * AVPacketQueue::pop() {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        while(queue_.empty()) {
            not_empty_.wait(queue_mutex_);
        }
        AVPacket *pkt = queue_.front();
        queue_.pop();
        not_full_.notify_one();
        return pkt;
    }
}
