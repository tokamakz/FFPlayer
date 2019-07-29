#include "FFPlayer.h"

#include <thread>
#include <functional>

#include "glog/logging.h"

namespace simple_player {
    FFPlayer::FFPlayer() {
        play_status_ = 1;
        render_ = new SDLRender();
        decoder_ = new FFDecoder();
        source_ = new FFSource();
    }

    FFPlayer::~FFPlayer() {
    }

    bool FFPlayer::open(const std::string &url) {
        bool bRet = render_->init();
        if(!bRet) {
            LOG(ERROR) << "render_->init fail!";
            return false;
        }

        bRet = source_->open(url);
        if (!bRet) {
            LOG(ERROR) << "source_->open fail!";
            return false;
        }

        bRet = decoder_->open(source_->getAVCodecID(), source_->getAVCodecParameters());
        if (!bRet) {
            LOG(ERROR) << "decoder_->open fail!";
            return false;
        }

        frame_queue_.init(1000);
        pkt_queue_.init(1000);

        return true;
    }

    bool FFPlayer::close() {
        return false;
    }

    bool FFPlayer::play() {
        std::thread th_render(&FFPlayer::image_render_thread, this);
        th_render.detach();
        std::thread th_decode(&FFPlayer::video_decode_thread, this);
        th_decode.detach();
        std::thread th_receive(&FFPlayer::receive_stream_thread, this);
        th_receive.detach();
    }

    void FFPlayer::receive_stream_thread() {
        while(play_status_ == 1) {
            AVPacket* pkt = av_packet_alloc();
            if (pkt == nullptr) {
                LOG(ERROR) << "av_packet_alloc fail!";
                break;
            }

            bool bRet = source_->read_frame(pkt);
            if(!bRet) {
                LOG(ERROR) << "source_->read_frame fail!";
                av_packet_free(&pkt);
                break;
            }

            pkt_queue_.push(pkt);
        }
    }

    void FFPlayer::video_decode_thread() {
        while(play_status_ == 1) {
            AVPacket* pkt = pkt_queue_.pop();
            AVFrame* frame = ::av_frame_alloc();
            if (frame == nullptr) {
                LOG(ERROR) << "av_frame_alloc fail!";
                break;
            }
            decoder_->decode(pkt, frame);
            ::av_packet_unref(pkt);
            frame_queue_.push(frame);
        }
    }

    void FFPlayer::image_render_thread() {
        while(play_status_ == 1) {
            AVFrame* frame = frame_queue_.pop();
            render_->render(frame);
            ::av_frame_free(&frame);
        }
    }
}