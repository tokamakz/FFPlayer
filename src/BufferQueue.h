#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <atomic>
#include <condition_variable>

extern "C" {
#include "libavcodec/avcodec.h"
}

namespace simple_player {
    template<class T>
    class BufferQueue {
    public:
        BufferQueue() {
            stop_status_ = false;
        }

        ~BufferQueue(){};

        void push(const T& value) {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            buffer_queue_.push(value);
        }

        void front() {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            buffer_queue_.front();
        }

        bool stop() {
            stop_status_ = true;
            {
                std::unique_lock<std::mutex> locker(stop_mutex_);
                stop_cond_.wait(locker);
            }
            stop_status_ = false;
            return true;
        }

        void send_packet(T pkt) {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            pkt_write_queue_.push(pkt);
        }

    private:
        std::queue<T> buffer_queue_;
        std::queue<T> pkt_write_queue_;
        std::mutex queue_mutex_;

        std::atomic<bool> stop_status_;
        std::condition_variable stop_cond_;
        std::mutex stop_mutex_;
    };
}
