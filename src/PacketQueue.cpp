#include "PacketQueue.h"

#include <thread>

namespace simple_player {
    PacketQueue::PacketQueue() {

    }

    PacketQueue::~PacketQueue() {

    }

    bool PacketQueue::start() {
        std::thread th([&]() {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            while(!pkt_queue_.empty()) {
                auto pkt = pkt_queue_.front();
                receive_packet(pkt);
            }
        });
        th.detach();

        return true;
    }

    void PacketQueue::send_packet(AVPacket *pkt) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        pkt_queue_.push(pkt);
    }
}