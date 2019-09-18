#pragma once

#include <string>

extern "C" {
#include "libavformat/avformat.h"
}

namespace simple_player {
    class ISource {
    public:
        ISource(){};

        ~ISource() {};

        virtual bool open(const std::string &url) = 0;

        virtual bool close() = 0;

        virtual bool read_frame(AVPacket *pkt) = 0;

        virtual enum AVCodecID getAVCodecID() = 0;

        virtual double getFrameRate() = 0;
    };
}
