#pragma once

#include <string>
#include "FrameQueue.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

namespace simple_player {
    class FFSource {
    public:
        FFSource();
        ~FFSource();
        bool open(const std::string &url);
        bool close();
        bool read_frame(AVPacket* pkt);

        enum AVCodecID getAVCodecID() const {
            return format_ctx_->streams[stream_type_]->codecpar->codec_id;
        }

        AVCodecParameters *getAVCodecParameters() const {
            return format_ctx_->streams[stream_type_]->codecpar;
        }

    private:
        AVFormatContext* format_ctx_;
        AVPacket* pkt_;
        int stream_type_;
    };
}

