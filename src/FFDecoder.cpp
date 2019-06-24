#include "FFDecoder.h"

#include <cstring>
#include <thread>
#include <iostream>
#include <iomanip>


namespace simple_player {
    FFDecoder::FFDecoder() {
        display_ = new SDLDisplay();
    }

    FFDecoder::~FFDecoder() {
    }

    bool FFDecoder::init() {
        return false;
    }

    bool FFDecoder::open(enum AVCodecID codec_id, const AVCodecParameters *par) {
        display_->init();
        codec_id_ = codec_id;
        AVCodec* codec = avcodec_find_decoder(codec_id_);
        if (codec == nullptr) {
            fprintf(stderr, "[video_player] Error: Unsupported codec. ERROR! \n");
            return false;
        }

        av_codec_ctx_ = avcodec_alloc_context3(codec);
        if (av_codec_ctx_ == nullptr) {
            fprintf(stderr, "[video_player] Error: Unsupported codec. ERROR! \n");
            return false;
        }

        int err_code = avcodec_parameters_to_context(av_codec_ctx_, par);
        if (err_code != 0) {
            fprintf(stderr, "[video_player] Error: avcodec_parameters_to_context\n");
            return false;
        }

        err_code = avcodec_open2(av_codec_ctx_, codec, nullptr);
        if (err_code != 0) {
            fprintf(stderr, "[video_player] Error: can not open the codec! ERROR! \n");
            return false;
        }

        return true;
    }

    void FFDecoder::de_init() {
    }


    void FFDecoder::start() {

    }

    void FFDecoder::receive_packet(AVPacket *pkt) {
        if(pkt == nullptr) {
            return ;
        }

        int ret = avcodec_send_packet(av_codec_ctx_, pkt);
        if (0 != ret) {
            fprintf(stderr, "[video_player] Error: avcodec_send_packet failed! ret = %d\n", ret);
            return ;
        }

        av_packet_free(&pkt);

        AVFrame *frame = av_frame_alloc();
        if (frame == nullptr) {
            fprintf(stderr, "[video_player] Error: pFrame alloc fail! ERROR!\n");
            return ;
        }

        ret = avcodec_receive_frame(av_codec_ctx_, frame);
        if (0 != ret) {
            fprintf(stderr, "[video_player] Error: avcodec_receive_frame failed! ret = %d\n", ret);
            return ;
        }

        frame_queue_.send_packet(frame);
    }
}