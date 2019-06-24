#pragma once

#include <string>

extern "C"{
#include "libavutil/intreadwrite.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
}

#include "PacketQueue.h"
#include "SDLDisplay.h"

namespace simple_player {
    class FFDecoder {
    public:
        FFDecoder();
        ~FFDecoder();
        bool init();
        bool open(enum AVCodecID codec_id, const AVCodecParameters *par);
        void de_init();
        void start();
        void receive_packet(AVPacket *pkt);

    private:
        SDLDisplay *display_;
        enum AVCodecID codec_id_;
        AVFrame *frame_;
        AVCodecContext* av_codec_ctx_;
        AVPacket *pAVPacket_;
        PacketQueue<AVFrame *> frame_queue_;
    };
}

