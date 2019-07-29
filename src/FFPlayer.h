#pragma once

#include <string>

#include "AVPacketQueue.h"
#include "FrameQueue.h"
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
        int play_status_;
        FFSource *source_;
        FFDecoder *decoder_;
        SDLRender *render_;
        FrameQueue frame_queue_;
        AVPacketQueue pkt_queue_;
    };
}
