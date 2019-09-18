#pragma once

#include <string>

extern "C" {
#include "libavformat/avformat.h"
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

        double getFrameRate() const {
            return av_q2d(format_ctx_->streams[stream_type_]->r_frame_rate);
        }

    private:
        AVFormatContext* format_ctx_;
        int stream_type_;
    };
}

