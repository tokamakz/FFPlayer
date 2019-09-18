#include "FFPlayer.h"
#include "common.h"
#include "source/rtsp/RTSPClient.h"
extern "C" {
#include "libavutil/time.h"
}

namespace simple_player {
    FFPlayer::FFPlayer() {
        render_interval_ = 0;
        play_status_ = false;
        receive_stream_thread_ = nullptr;
        video_decode_thread_ = nullptr;
        image_render_thread_ = nullptr;
        render_ = new SDLRender();
        decoder_ = new FFDecoder();
        source_ = new RTSPClient();
    }

    FFPlayer::~FFPlayer() {
        safe_deletep(render_);
        safe_deletep(decoder_);
        safe_deletep(source_);
    }

    bool FFPlayer::open(const std::string &url) {
        bool bRet = render_->open();
        if(!bRet) {
            LOG(ERROR) << "render_->init fail!";
            return false;
        }

        bRet = source_->open(url);
        if (!bRet) {
            LOG(ERROR) << "source_->open fail!";
            return false;
        }

        render_interval_ = 1000000 / source_->getFrameRate();
        LOG(INFO) << "render_interval_ = " << render_interval_;

        bRet = decoder_->open(source_->getAVCodecID());
        if (!bRet) {
            LOG(ERROR) << "decoder_->open fail!";
            return false;
        }

        bRet = frame_queue_.init(1000);
        if(!bRet) {
            LOG(ERROR) << "frame_queue_.init fail!";
            return false;
        }

        bRet = pkt_queue_.init(1000);
        if(!bRet) {
            LOG(ERROR) << "pkt_queue_.init fail!";
            return false;
        }

        play_status_ = true;
        receive_stream_thread_ = new std::thread(&FFPlayer::receive_stream_thread_body, this);
        video_decode_thread_ = new std::thread(&FFPlayer::video_decode_thread_body, this);
        //image_render_thread_ = new std::thread(&FFPlayer::image_render_thread_body, this);

        return true;
    }

    bool FFPlayer::close() {
        play_status_ = false;
        receive_stream_thread_->join();
        safe_deletep(receive_stream_thread_);
        video_decode_thread_->join();
        safe_deletep(video_decode_thread_);
        image_render_thread_->join();
        safe_deletep(image_render_thread_);
        source_->close();
        decoder_->close();
        render_->close();
        frame_queue_.de_init();
        pkt_queue_.de_init();
        return true;
    }

    void FFPlayer::receive_stream_thread_body() {
        while(play_status_) {
            AVPacket* pkt = av_packet_alloc();
            if (pkt == nullptr) {
                LOG(ERROR) << "av_packet_alloc fail!";
                break;
            }

            int iRet = av_new_packet(pkt, 900000);
            if(iRet != 0) {
                LOG(ERROR) << "av_new_packet fail!";
                break;
            }

            pkt->pos = 0;

            bool bRet = source_->read_frame(pkt);
            if(!bRet) {
                LOG(ERROR) << "source_->read_frame fail!";
                av_packet_free(&pkt);
                break;
            }
            LOG(ERROR) << "pkt->pos " << pkt->pos;
            pkt_queue_.push(pkt);
        }
    }

    void FFPlayer::video_decode_thread_body() {
        while(play_status_) {
            AVPacket* pkt = pkt_queue_.pop();
            if(pkt == nullptr) {
                continue;
            }
            AVFrame* frame = frame_queue_.get();
            if(frame == nullptr) {
                continue;
            }
            bool bRet = decoder_->decode(pkt, frame);
            ::av_packet_unref(pkt);
            if(!bRet) {
                LOG(ERROR) << "decoder_->decode fail!";
                frame_queue_.put(frame);
                continue;
            }
            frame_queue_.push(frame);
        }
    }

    void FFPlayer::image_render_thread_body() {
        while(play_status_) {
            AVFrame* frame = frame_queue_.pop();
            long sleep_time = render_interval_ - render_->render(frame);
            LOG(INFO) << "sleep_time " << sleep_time;
            ::av_usleep(sleep_time);
            frame_queue_.put(frame);
        }
    }
}