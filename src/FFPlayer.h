#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>

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
        std::thread *receive_stream_thread_;
        std::thread *video_decode_thread_;
        std::thread *image_render_thread_;

        void receive_stream_thread_body();
        void video_decode_thread_body();
        void image_render_thread_body();

        unsigned int render_interval_;

        std::atomic_bool play_status_;

        FFSource *source_;
        FFDecoder *decoder_;
        SDLRender *render_;

        AVFrameQueue frame_queue_;
        AVPacketQueue pkt_queue_;
    };
}
