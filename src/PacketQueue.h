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
    class PacketQueue {
    public:
        PacketQueue() {
            stop_status_ = false;
        }

        ~PacketQueue(){};

        bool start(std::function<void(T)> receive_packet) {
            std::thread th([&]() {
                while(true) {
                    if(pkt_read_queue_.empty()) {
                        std::lock_guard<std::mutex> lock(queue_mutex_);
                        if (pkt_write_queue_.empty()) {
                            continue;
                        }
                        pkt_read_queue_.swap(pkt_write_queue_);
                    }
                    auto pkt = pkt_read_queue_.front();
                    pkt_read_queue_.pop();
                    receive_packet(pkt);
                    if(stop_status_) {
                        std::lock_guard<std::mutex> locker(stop_mutex_);
                        stop_cond_.notify_one();
                        break;
                    }
                }
            });
            th.detach();
            return true;
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
        std::queue<T> pkt_read_queue_;
        std::queue<T> pkt_write_queue_;
        std::mutex queue_mutex_;

        std::atomic<bool> stop_status_;
        std::condition_variable stop_cond_;
        std::mutex stop_mutex_;
    };
}
