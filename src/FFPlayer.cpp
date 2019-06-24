#include "FFPlayer.h"

#include <thread>
#include <functional>

namespace simple_player {
    FFPlayer::FFPlayer() {
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

        pkt_queue_.start(std::bind(&FFDecoder::receive_packet, decoder_, std::placeholders::_1));

        return false;
    }

    bool FFPlayer::close() {
        return false;
    }

    bool FFPlayer::play() {
        while(true) {
            AVPacket* pkt = av_packet_alloc();
            if (pkt == nullptr) {
                fprintf(stderr, "[video_player] Error: raw_packet alloc fail!\n");
                return false;
            }

            bool bRet = source_->read_frame(pkt);
            if(!bRet) {
                av_packet_free(&pkt);
                return false;
            }

            pkt_queue_.send_packet(pkt);
        }
    }
}