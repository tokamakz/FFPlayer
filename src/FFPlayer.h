#pragma once

#include <string>
#include <atomic>

#include "AVPacketQueue.h"
#include "AVFrameQueue.h"
#include "FFSource.h"
#include "FFDecoder.h"
#include "SDLRender.h"

namespace simple_player {
    class FFPlayer {
    public:
        FFPlayer();
        ~FFPlayer();
        bool open(const std::string &url);
        bool close();

    private:
        void receive_stream_thread();
        void video_decode_thread();
        void image_render_thread();
        std::atomic<int> play_status_;
        unsigned int render_interval;

        FFSource *source_;
        FFDecoder *decoder_;
        SDLRender *render_;
        AVFrameQueue frame_queue_;
        AVPacketQueue pkt_queue_;

        std::mutex close_mutex_;
        int close_thread_count_;
        std::condition_variable_any close_cond_;
    };
}
