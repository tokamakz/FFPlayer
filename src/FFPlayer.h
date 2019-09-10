#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <condition_variable>

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
        unsigned int render_interval_;

        std::atomic_bool play_status_;

        bool receive_stream_thread_cancel_flag_;
        std::mutex receive_stream_mutex_;
        std::condition_variable receive_stream_cond_;

        bool video_decode_thread_cancel_flag_;
        std::mutex video_decode_mutex_;
        std::condition_variable video_decode_cond_;

        bool image_render_thread_cancel_flag_;
        std::mutex image_render_mutex_;
        std::condition_variable image_render_cond_;

        FFSource *source_;
        FFDecoder *decoder_;
        SDLRender *render_;
        AVFrameQueue frame_queue_;
        AVPacketQueue pkt_queue_;
    };
}
