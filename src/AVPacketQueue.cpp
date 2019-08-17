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
        while(!queue_.empty()) {
            AVPacket *pkt = queue_.front();
            ::av_packet_unref(pkt);
            queue_.pop();
        }
    }

    void AVPacketQueue::push(AVPacket *pkt) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        not_full_.wait(queue_mutex_, [this]{return queue_.size() < max_queue_size_;});
        queue_.push(pkt);
        not_empty_.notify_one();
    }

    AVPacket * AVPacketQueue::pop() {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        not_empty_.wait(queue_mutex_, [this]{return !queue_.empty();});
        AVPacket *pkt = queue_.front();
        queue_.pop();
        not_full_.notify_one();
        return pkt;
    }
}
