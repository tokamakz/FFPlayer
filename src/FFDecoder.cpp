#include "FFDecoder.h"

#include "glog/logging.h"

namespace simple_player {
    FFDecoder::FFDecoder() {
        av_codec_ctx_ = nullptr;
    }

    FFDecoder::~FFDecoder() = default;

    bool FFDecoder::open(enum AVCodecID codec_id, const AVCodecParameters *par) {
        AVCodec* codec = ::avcodec_find_decoder(codec_id);
        if (codec == nullptr) {
            LOG(ERROR) << "Unsupported codec";
            return false;
        }

        av_codec_ctx_ = ::avcodec_alloc_context3(codec);
        if (av_codec_ctx_ == nullptr) {
            LOG(ERROR) << "avcodec_alloc_context3 ERROR";
            return false;
        }

        int err_code = ::avcodec_parameters_to_context(av_codec_ctx_, par);
        if (err_code != 0) {
            LOG(ERROR) << "avcodec_parameters_to_context ERROR";
            return false;
        }

        err_code = ::avcodec_open2(av_codec_ctx_, codec, nullptr);
        if (err_code != 0) {
            LOG(ERROR) << "avcodec_open2 ERROR";
            return false;
        }

        return true;
    }

    bool FFDecoder::decode(AVPacket *pkt, AVFrame* frame) {
        if(pkt == nullptr || frame == nullptr) {
            return false;
        }

        int ret = ::avcodec_send_packet(av_codec_ctx_, pkt);
        if (0 != ret) {
            LOG(ERROR) << "avcodec_send_packet failed! ret = " << ret;
            return false;
        }

        ret = ::avcodec_receive_frame(av_codec_ctx_, frame);
        if (0 != ret) {
            LOG(ERROR) << "avcodec_receive_frame failed!";
            return false;
        }

        return true;
    }

    void FFDecoder::close() {
        ::avcodec_free_context(&av_codec_ctx_);
    }
}