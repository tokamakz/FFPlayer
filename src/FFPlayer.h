#pragma once

#include <string>
#include <list>

#include "FFSource.h"
#include "FFDecoder.h"
#include "SDLDisplay.h"

namespace simple_player {
    class FFPlayer {
    public:
        FFPlayer();
        ~FFPlayer();
        bool open(const std::string &url);
        bool play();
        bool close();
        bool send_packet(AVPacket*);

    private:
        SDLDisplay *display_;
        FFDecoder *decoder_;
        FFSource *source_;
        std::list<AVPacket*> pkt_queue_;
    };
}
