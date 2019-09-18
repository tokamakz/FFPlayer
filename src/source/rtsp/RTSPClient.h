#pragma once

#include <string>
#include <tuple>
#include <functional>
#include <thread>
#include <atomic>

#include "infra/net/TCPSocket.h"
#include "infra/net/UDPSocket.h"
#include "source/ISource.h"
#include "RTPQueue.h"
#include "Demuxer.h"

namespace simple_player {
    class RTSPClient : public ISource {
    public:
        RTSPClient();
        ~RTSPClient();
        bool open(const std::string &url) override;

        bool options();
        bool describe();
        bool setup();
        bool play();
        bool teardown();
        bool set_parameter();
        bool get_parameter();

        bool read_frame(AVPacket *pkt) override;
        enum AVCodecID getAVCodecID() override;
        double getFrameRate() override;
        bool close() override;

    private:
        void receive_rtp_packet_thread_body();
        std::thread *receive_rtp_packet_thread_;
        static void set_request_header(std::string &request, const std::string &name, const std::string &value);
        static void get_response_header(const std::string &response, const std::string &name, std::string &value);
        static std::tuple<bool, std::string, std::string> get_response_message(const std::string &response);
        static char * data_to_hex(char *buff, const uint8_t *src, int s, int lowercase);
        bool get_udp_transport();
        std::string set_authorization(const std::string &cmd);
        bool sendRequest(const std::string &request, std::string &response);

        unsigned int cseq_;
        std::string url_;
        std::string user_name_;
        std::string password_;
        std::string session_;

        std::string authorization_prefix_;
        std::string authorization_;
        Infra::Net::TCPSocket *rtsp_socket_;
        //RTSPSdp* rtsp_sdp_;

        Demuxer *demuxer_;

        unsigned int rtp_port_;
        unsigned int rtcp_port_;
        RTPQueue rtp_queue_;
        std::atomic_bool receive_status_;

        Infra::Net::UDPSocket *rtp_socket_;
        Infra::Net::UDPSocket *rtcp_socket_;

        static const unsigned int RTSP_PORT_MIN;
        static const unsigned int RTSP_PORT_MAX;
        static const std::string  RTSP_VERSION;
        static const std::string  USER_AGENT;
    };
}


