#include "RTPQueue.h"
#include "common.h"

namespace simple_player {
    RTPQueue::RTPQueue() {
        max_queue_size_ = 0;
    }

    RTPQueue::~RTPQueue() = default;

    bool RTPQueue::init(int max_queue_size) {
        max_queue_size_ = max_queue_size;
        return true;
    }

    void RTPQueue::de_init() {
        while(!queue_.empty()) {
            RTPPacket *pkt = queue_.front();
            safe_deletep(pkt);
            queue_.pop();
        }
    }

    void RTPQueue::push(RTPPacket *pkt) {
        std::unique_lock<std::mutex> locker(queue_mutex_);
        bool not_time_out = not_full_.wait_for(locker, std::chrono::seconds(1), [this]{return queue_.size() < max_queue_size_;});
        if(not_time_out) {
            queue_.push(pkt);
        }
        not_empty_.notify_one();
    }

    RTPPacket * RTPQueue::pop() {
        std::unique_lock<std::mutex> locker(queue_mutex_);
        bool not_time_out = not_empty_.wait_for(locker, std::chrono::seconds(1), [this]{return !queue_.empty();});
        RTPPacket *pkt = nullptr;
        if(not_time_out) {
            pkt = queue_.front();
            queue_.pop();
        }
        not_full_.notify_one();
        return pkt;
    }
}
