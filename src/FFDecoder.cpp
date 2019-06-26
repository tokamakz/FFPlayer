#include "FFDecoder.h"

#include <cstring>
#include <thread>
#include <iostream>
#include <iomanip>

#include "glog/logging.h"

namespace simple_player {
    FFDecoder::FFDecoder() {
        display_ = new SDLDisplay();
    }

    FFDecoder::~FFDecoder() {
    }

    bool FFDecoder::open(enum AVCodecID codec_id, const AVCodecParameters *par) {
        display_->init();
        codec_id_ = codec_id;
        AVCodec* codec = avcodec_find_decoder(codec_id_);
        if (codec == nullptr) {
            LOG(ERROR) << "Unsupported codec";
            return false;
        }

        av_codec_ctx_ = avcodec_alloc_context3(codec);
        if (av_codec_ctx_ == nullptr) {
            LOG(ERROR) << "avcodec_alloc_context3 ERROR";
            return false;
        }

        int err_code = avcodec_parameters_to_context(av_codec_ctx_, par);
        if (err_code != 0) {
            LOG(ERROR) << "avcodec_parameters_to_context ERROR";
            return false;
        }

        err_code = avcodec_open2(av_codec_ctx_, codec, nullptr);
        if (err_code != 0) {
            LOG(ERROR) << "avcodec_open2 ERROR";
            return false;
        }

        return true;
    }

    void FFDecoder::start() {
    }

    void FFDecoder::receive_packet(AVPacket *pkt) {
        if(pkt == nullptr) {
            return ;
        }

        int ret = avcodec_send_packet(av_codec_ctx_, pkt);
        if (0 != ret) {
            LOG(ERROR) << "avcodec_send_packet failed! ret = " << ret;
            return ;
        }

        av_packet_free(&pkt);

        AVFrame *frame = av_frame_alloc();
        if (frame == nullptr) {
            LOG(ERROR) << "av_frame_alloc failed!";
            return ;
        }

        ret = avcodec_receive_frame(av_codec_ctx_, frame);
        if (0 != ret) {
            LOG(ERROR) << "avcodec_receive_frame failed!";
            return ;
        }

        frame_queue_.send_packet(frame);
    }
}