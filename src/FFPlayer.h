#pragma once

#include <string>
#include <list>

#include "FFSource.h"
#include "FFDecoder.h"

#include "PacketQueue.h"

namespace simple_player {
    class FFPlayer {
    public:
        FFPlayer();
        ~FFPlayer();
        bool open(const std::string &url);
        bool play();
        bool close();

    private:

        FFDecoder *decoder_;
        FFSource *source_;
        PacketQueue<AVPacket*> pkt_queue_;
    };
}
