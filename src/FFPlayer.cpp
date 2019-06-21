#include "FFPlayer.h"

#include <thread>

namespace simple_player {
    FFPlayer::FFPlayer() {
        display_ = new SDLDisplay();
        decoder_ = new FFDecoder();
        source_ = new FFSource();
    }

    FFPlayer::~FFPlayer() {
    }

    bool FFPlayer::open(const std::string &url) {
        bool bRet = source_->open(url);
        if (!bRet) {
            fprintf(stderr, "[video_player] source_->open alloc fail!\n");
            return false;
        }

        bRet = decoder_->open(source_->getAVCodecID(), source_->getAVCodecParameters());
        if (!bRet) {
            fprintf(stderr, "[video_player] decoder_->open alloc fail!\n");
            return false;
        }

        display_->init();

        return false;
    }

    bool FFPlayer::close() {
        return false;
    }

    bool FFPlayer::play() {

        AVPacket* pkt = av_packet_alloc();
        if (pkt == nullptr) {
            fprintf(stderr, "[video_player] Error: raw_packet alloc fail!\n");
            return false;
        }

        while(source_->read_frame(pkt)) {
            send_packet(pkt);
        }

        return false;
    }
}