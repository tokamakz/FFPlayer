#include "Demuxer.h"

#include <cstring>

extern "C" {
#include "libavutil/intreadwrite.h"
}

namespace simple_player {

    Demuxer::Demuxer() {
    }

    Demuxer::~Demuxer() {

    }

    bool Demuxer::demux(RTPPacket *rtp_pkt, AVPacket *pkt) {
        bool bRet = rtp_parse_packet(rtp_pkt);
        if(!bRet) {
            LOG(ERROR) << "rtp_parse_packet failed!";
        }

        unsigned char nal = rtp_pkt->buf[0];
        unsigned char type = nal & 0x1f;

        bool result = false;

        switch (type) {
            case 0: // undefined, but pass them through
            case 1:
                if(pkt->pos + start_sequence_size_ < frame_data_len_max_) {
                    std::memcpy(pkt->data + pkt->pos, start_sequence_, start_sequence_size_);
                    pkt->pos += start_sequence_size_;
                } else {
                    LOG(ERROR) << "memory will out, clear all media data";
                    result = false;
                    break;
                }

                if(pkt->pos + rtp_pkt->len < frame_data_len_max_) {
                    std::memcpy(pkt->data + pkt->pos, rtp_pkt->buf, rtp_pkt->len);
                    pkt->pos += rtp_pkt->len;
                } else {
                    LOG(ERROR) << "memory will out, clear all media data";
                    result = false;
                    break;
                }
                result = true;
                break;
            case 6: // SEI
            case 7: // SPS
            case 8: // PPS
                if(pkt->pos + start_sequence_size_ < frame_data_len_max_) {
                    std::memcpy(pkt->data + pkt->pos, start_sequence_, start_sequence_size_);
                    pkt->pos += start_sequence_size_;
                } else {
                    LOG(ERROR) << "memory will out, clear all media data";
                    result = false;
                    break;
                }

                if(pkt->pos + rtp_pkt->len < frame_data_len_max_) {
                    std::memcpy(pkt->data + pkt->pos, rtp_pkt->buf, rtp_pkt->len);
                    pkt->pos += rtp_pkt->len;
                } else {
                    LOG(ERROR) << "memory will out, clear all media data";
                    result = false;
                    break;
                }
                result = false;
                break;
            case 24: // STAP-A (one packet, multiple nals)
                rtp_pkt->buf++;
                rtp_pkt->len--;
                h264_handle_aggregated_packet(rtp_pkt, pkt);
                result = false;
                break;
            case 25: // STAP-B
            case 26: // MTAP-16
            case 27: // MTAP-24
            case 29: // FU-B
                LOG(ERROR) << "RTP H.264 NAL unit type " << type;
                result = false;
                break;

            case 28: // FU-A (fragmented nal)
                result = h264_handle_packet_fu_a(rtp_pkt, pkt);
                break;

            case 30: // undefined
            case 31: // undefined
            default:
                LOG(ERROR) << "Undefined type " << type;
                result = false;
                break;
        }

        return result;
    }

    bool Demuxer::rtp_parse_packet(RTPPacket *rtp_pkt) {
        if (rtp_pkt->len < 12) {
            LOG(ERROR) << "rtp_pkt->len < 12";
            return false;
        }

        if ((rtp_pkt->buf[0] & 0xc0) != (RTPPacket::RTP_VERSION << 6)) {
            LOG(ERROR) << "RTP_VERSION ERROR";
            return false;
        }

        unsigned int ssrc = 0;
        int payload_type = 0;
        int ext = 0;
        uint32_t timestamp = 0;
        int csrc = 0;
        int seq = 0;
        int mark = 0;

        csrc         = rtp_pkt->buf[0] & 0x0f;
        ext          = rtp_pkt->buf[0] & 0x10;
        payload_type = rtp_pkt->buf[1] & 0x7f;
        mark         = rtp_pkt->buf[1] & 0x80;
        seq          = AV_RB16(rtp_pkt->buf + 2);
        timestamp    = AV_RB32(rtp_pkt->buf + 4);
        ssrc         = AV_RB32(rtp_pkt->buf + 8);

        if (rtp_pkt->buf[0] & 0x20) {
            int padding = rtp_pkt->buf[rtp_pkt->len - 1];
            if (rtp_pkt->len >= 12 + padding) {
                rtp_pkt->len -= padding;
            }
        }

        //seq
        rtp_pkt->len -= 12;
        rtp_pkt->buf += 12;

        rtp_pkt->len -= 4 * csrc;
        rtp_pkt->buf += 4 * csrc;

        /* RFC 3550 Section 5.3.1 RTP Header Extension handling */
        if (ext) {
            if (rtp_pkt->len < 4) {
                return false;
            }

            /* calculate the header extension length (stored as number
             * of 32-bit words) */
            ext = (AV_RB16(rtp_pkt->buf + 2) + 1) << 2;

            if (rtp_pkt->len < ext) {
                return false;
            }

            // skip past RTP header extension
            rtp_pkt->len -= ext;
            rtp_pkt->buf += ext;
        }

        return true;
    }

    bool Demuxer::h264_handle_aggregated_packet(RTPPacket *rtp_pkt, AVPacket *pkt) {
        int total_length = 0;
        // first we are going to figure out the total size
        for (int pass = 0; pass < 2; pass++) {
            const uint8_t *src = rtp_pkt->buf;
            int src_len = rtp_pkt->len;

            while (src_len > 2) {
                uint16_t nal_size = AV_RB16(src);
                src += 2;
                src_len -= 2;

                if (nal_size <= src_len) {
                    if (pass == 0) {
                        total_length += start_sequence_size_ + nal_size;
                    } else {
                        if (pkt->pos + start_sequence_size_ < frame_data_len_max_) {
                            std::memcpy(pkt->data + pkt->pos, start_sequence_, start_sequence_size_);
                            pkt->pos += start_sequence_size_;
                        } else {
                            LOG(ERROR) << "memory will out, clear all media data";
                            return false;
                        }

                        if (pkt->pos + nal_size < frame_data_len_max_) {
                            std::memcpy(pkt->data + pkt->pos, src, nal_size);
                            pkt->pos += nal_size;
                        } else {
                            LOG(ERROR) << "memory will out, clear all media data";
                            return false;
                        }
                    }
                } else {
                    LOG(ERROR)<< "nal size exceeds length: " << nal_size << " " << src_len;
                    return false;
                }

                src += nal_size;
                src_len -= nal_size;
            }
        }

        return true;
    }

    bool Demuxer::h264_handle_packet_fu_a(RTPPacket *rtp_pkt, AVPacket *pkt) {
        if (rtp_pkt->len < 2) {
            LOG(ERROR) << "rtp_pkt->len < 2";
            return false;
        }

        unsigned char fu_indicator = rtp_pkt->buf[0];
        unsigned char fu_header    = rtp_pkt->buf[1];

        unsigned char start_bit = fu_header >> 7;
        unsigned char end_bit = (fu_header & 0x40) == 0x40 ? 1 : 0;
        unsigned char nal_type = fu_header & 0x1f;
        unsigned char nal = (fu_indicator & 0xe0) | nal_type;

        rtp_pkt->buf += 2;
        rtp_pkt->len -= 2;

        if (start_bit) {
            if(pkt->pos + rtp_pkt->len + 1 < frame_data_len_max_) {
                std::memcpy(pkt->data + pkt->pos, start_sequence_, start_sequence_size_);
                pkt->pos  += start_sequence_size_;
                pkt->data[rtp_pkt->len] = nal;
                pkt->pos += 1;
            } else {
                LOG(ERROR) << "memory will out, clear all media data";
                return false;
            }
        }

        if (pkt->pos + rtp_pkt->len < frame_data_len_max_) {
            std::memcpy(pkt->data + pkt->pos, rtp_pkt->buf, rtp_pkt->len);
            pkt->pos += rtp_pkt->len;
        } else {
            LOG(ERROR) << "memory will out, clear all media data";
            return false;
        }

        if (end_bit) {
            return true;
        }

        return false;
    }
}
