#include "FFPlayer.h"

#include <thread>

#include "glog/logging.h"
extern "C" {
#include "libavutil/time.h"
}

namespace simple_player {
    FFPlayer::FFPlayer() {
        render_interval_ = 0;
        play_status_ = false;
        receive_stream_thread_cancel_flag_ = false;
        video_decode_thread_cancel_flag_ = false;
        image_render_thread_cancel_flag_ = false;
        render_ = new SDLRender();
        decoder_ = new FFDecoder();
        source_ = new FFSource();
    }

    FFPlayer::~FFPlayer() {
        if(render_ != nullptr) {
            delete render_;
            render_ = nullptr;
        }

        if(decoder_ != nullptr) {
            delete decoder_;
            decoder_ = nullptr;
        }

        if(source_ != nullptr) {
            delete source_;
            source_ = nullptr;
        }
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

        bRet = decoder_->open(source_->getAVCodecID(), source_->getAVCodecParameters());
        if (!bRet) {
            LOG(ERROR) << "decoder_->open fail!";
            return false;
        }

        bRet = frame_queue_.init(500);
        if(!bRet) {
            LOG(ERROR) << "frame_queue_.init fail!";
            return false;
        }

        bRet = pkt_queue_.init(500);
        if(!bRet) {
            LOG(ERROR) << "pkt_queue_.init fail!";
            return false;
        }

        play_status_ = true;
        receive_stream_thread_cancel_flag_ = false;
        video_decode_thread_cancel_flag_ = false;
        image_render_thread_cancel_flag_ = false;

        std::thread th_receive(&FFPlayer::receive_stream_thread, this);
        th_receive.detach();
        std::thread th_decode(&FFPlayer::video_decode_thread, this);
        th_decode.detach();
        std::thread th_render(&FFPlayer::image_render_thread, this);
        th_render.detach();

        return true;
    }

    bool FFPlayer::close() {
        play_status_ = false;

        {
            std::unique_lock<std::mutex> locker(receive_stream_mutex_);
            image_render_cond_.wait(locker, [this]{return image_render_thread_cancel_flag_;});
        }

        {
            std::unique_lock<std::mutex> locker(video_decode_mutex_);
            video_decode_cond_.wait(locker, [this]{return video_decode_thread_cancel_flag_;});
        }

        {
            std::unique_lock<std::mutex> locker(receive_stream_mutex_);
            receive_stream_cond_.wait(locker, [this]{return receive_stream_thread_cancel_flag_;});
        }

        source_->close();
        decoder_->close();
        render_->close();
        frame_queue_.de_init();
        pkt_queue_.de_init();
        return true;
    }

    void FFPlayer::receive_stream_thread() {
        while(play_status_) {
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

        {
            std::lock_guard<std::mutex> locker(receive_stream_mutex_);
            LOG(ERROR) << "receive_stream_thread in cancel";
            receive_stream_thread_cancel_flag_ = true;
            receive_stream_cond_.notify_one();
        }
    }

    void FFPlayer::video_decode_thread() {
        while(play_status_) {
            AVPacket* pkt = pkt_queue_.pop();
            AVFrame* frame = frame_queue_.get();
            bool bRet = decoder_->decode(pkt, frame);
            ::av_packet_unref(pkt);
            if(!bRet) {
                LOG(ERROR) << "decoder_->decode fail!";
                frame_queue_.put(frame);
                continue;
            }
            frame_queue_.push(frame);
        }

        {
            std::lock_guard<std::mutex> locker(video_decode_mutex_);
            video_decode_thread_cancel_flag_ = true;
            video_decode_cond_.notify_one();
        }
    }

    void FFPlayer::image_render_thread() {
        while(play_status_) {
            AVFrame* frame = frame_queue_.pop();
            render_->render(frame);
            frame_queue_.put(frame);
            ::av_usleep(render_interval_);
        }

        {
            std::lock_guard<std::mutex> locker(image_render_mutex_);
            image_render_thread_cancel_flag_ = true;
            image_render_cond_.notify_one();
        }
    }
}