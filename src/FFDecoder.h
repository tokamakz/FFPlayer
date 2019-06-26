#pragma once

#include <string>

extern "C"{
#include "libavutil/intreadwrite.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
}

#include "BufferQueue.h"
#include "SDLDisplay.h"

namespace simple_player {
    class FFDecoder {
    public:
        FFDecoder();
        ~FFDecoder();
        bool open(enum AVCodecID codec_id, const AVCodecParameters *par);
        void start();
        void receive_packet(AVPacket *pkt);

    private:
        SDLDisplay *display_;
        enum AVCodecID codec_id_;
        AVFrame *frame_;
        AVCodecContext* av_codec_ctx_;
        BufferQueue<AVFrame *> frame_queue_;
    };
}

