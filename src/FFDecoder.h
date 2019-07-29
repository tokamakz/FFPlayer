#pragma once

#include <string>

extern "C"{
#include "libavutil/intreadwrite.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
}

#include "FrameQueue.h"
#include "SDLRender.h"

namespace simple_player {
    class FFDecoder {
    public:
        FFDecoder();
        ~FFDecoder();
        bool open(enum AVCodecID codec_id, const AVCodecParameters *par);
        bool decode(AVPacket *pkt, AVFrame* frame);

    private:
        AVCodecContext* av_codec_ctx_;
    };
}

