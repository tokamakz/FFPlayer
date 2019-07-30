#include "FFSource.h"

namespace simple_player {
    FFSource::FFSource() {
        format_ctx_ = nullptr;
        stream_type_ = -1;
    }

    FFSource::~FFSource() = default;

    bool FFSource::open(const std::string &url) {
        AVDictionary* options = nullptr;
        av_dict_set(&options, "stimeout", "3000000", 0);
        av_dict_set(&options, "rtsp_transport", "tcp", 0);

        int err_code = avformat_open_input(&format_ctx_, url.c_str(), nullptr, &options);
        if (err_code < 0) {
            char error_buf[10240];
            av_make_error_string(error_buf, 10240, err_code);
            fprintf(stderr, "[video_player] Error: avformat_open_input failed! err_code = %d error_buf = %s\n", err_code, error_buf);
            return false;
        }

        err_code = avformat_find_stream_info(format_ctx_, nullptr);
        if (err_code < 0) {
            fprintf(stderr, "[video_player] Error: avformat_find_stream_info failed! err_code = %d\n", err_code);
            return false;
        }

        int stream_type = -1;
        for (int i = 0; i < format_ctx_->nb_streams; i++) {
            if (format_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                stream_type = i;
                break;
            }
        }

        if (stream_type == -1) {
            fprintf(stderr, "[video_player] Error: NOT find a video stream. ERROR! \n");
            return false;
        }

        stream_type_ = stream_type;

        return true;
    }

    bool FFSource::close() {
        avformat_close_input(&format_ctx_);
        return true;
    }

     bool FFSource::read_frame(AVPacket* pkt) {
        while (av_read_frame(format_ctx_, pkt) >= 0) {
            if (pkt->stream_index != stream_type_) {
                continue;
            }
            return true;
        }
        return false;
    }
}
