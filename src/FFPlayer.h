#pragma once

#include <string>

#include "AVPacketQueue.h"
#include "FrameQueue.h"
#include "FFSource.h"
#include "FFDecoder.h"

namespace simple_player {
    class FFPlayer {
    public:
        FFPlayer();
        ~FFPlayer();
        bool open(const std::string &url);
        bool play();
        bool close();

    private:
        int play_status_;
        void receive_stream_thread();
        void video_decode_thread();
        void image_render_thread();
        FFSource *source_;
        FFDecoder *decoder_;
        SDLRender *render_;
        FrameQueue frame_queue_;
        AVPacketQueue pkt_queue_;
    };
}
