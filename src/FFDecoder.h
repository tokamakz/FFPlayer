#pragma once

extern "C"{
#include "libavcodec/avcodec.h"
}

namespace simple_player {
    class FFDecoder {
    public:
        FFDecoder();
        ~FFDecoder();
        bool open(enum AVCodecID codec_id, const AVCodecParameters *par);
        void close();
        bool decode(AVPacket *pkt, AVFrame* frame);

    private:
        AVCodecContext* av_codec_ctx_;
    };
}

