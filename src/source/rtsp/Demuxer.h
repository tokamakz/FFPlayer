#pragma once

#include "RTPPacket.h"
extern "C"{
#include "libavcodec/avcodec.h"
}

namespace simple_player {
    class Demuxer {
    public:
        Demuxer();
        ~Demuxer();
        bool demux(RTPPacket *rtp_pkt, AVPacket *pkt);

    private:
        bool rtp_parse_packet(RTPPacket *rtp_pkt);
        bool h264_handle_aggregated_packet(RTPPacket *rtp_pkt, AVPacket *pkt);
        bool h264_handle_packet_fu_a(RTPPacket *rtp_pkt, AVPacket *pkt);
        unsigned char start_sequence_[4] = {0x00, 0x00, 0x00, 0x01};
        const unsigned int start_sequence_size_ = 4;
        const unsigned int frame_data_len_max_ = 1024000;
    };
}
