#include "RTSPClient.h"

#include <cstring>
#include <cstdlib>

#include <iostream>
#include <thread>
#include <chrono>
#include <regex>

#include "common.h"
#include "infra/net/TCPSocket.h"

extern "C" {
#include "libavutil/md5.h"
}

const unsigned int simple_player::RTSPClient::RTSP_PORT_MIN = 49152;
const unsigned int simple_player::RTSPClient::RTSP_PORT_MAX = 65535;
const std::string  simple_player::RTSPClient::RTSP_VERSION = "RTSP/1.0";
const std::string  simple_player::RTSPClient::USER_AGENT = "sms/1.0.0 (rtspclient)";

namespace simple_player {
    RTSPClient::RTSPClient() {
        cseq_ = 1;
        rtp_port_ = RTSP_PORT_MIN;
        rtcp_port_ = RTSP_PORT_MIN + 1;
        rtsp_socket_ = new Infra::Net::TCPSocket();
        demuxer_ = new Demuxer();
        receive_status_ = false;
        //rtsp_sdp_ = new RTSPSdp();
    }

    RTSPClient::~RTSPClient() {
        safe_deletep(rtsp_socket_);
        //safe_deletep(rtsp_sdp_);
    }

    bool RTSPClient::open(const std::string &url) {
        std::regex regex_rtsp(R"(rtsp://(\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3}))");
        std::smatch smatch_rtsp;

        if (!std::regex_match(url, smatch_rtsp, regex_rtsp)) {
            LOG(ERROR) << "url vaild! " << url;
            return false;
        }

        url_ = url;
        user_name_ = "admin";
        password_ = "q1495359275";

        int ret = rtsp_socket_->open();
        if (ret < 0) {
            LOG(ERROR) << "rtsp_socket_->open failed! error: " <<  strerror(errno) << " errno:" << errno;
            return false;
        }

        ret = rtsp_socket_->connect(smatch_rtsp.str(1).c_str(), 554);
        if (ret < 0) {
            LOG(ERROR) << "rtsp_socket_->connect failed! error: " <<  strerror(errno) << " errno:" << errno;
            return false;
        }

        rtp_queue_.init(10000);


        options();
        describe();
        setup();
        play();


        return true;
    }

    bool RTSPClient::options() {
        std::string request = "OPTIONS " + url_ + " " + RTSP_VERSION + "\r\n";
        set_request_header(request, "CSeq", std::to_string(++cseq_));
        set_request_header(request, "User-Agent", USER_AGENT);
        request.append("\r\n");

        std::string response;
        sendRequest(request, response);
        if(response.empty()) {
            LOG(ERROR) << "options failed! error: response empty!";
            return false;
        }

        bool ret = false;
        std::string code;
        std::string message;
        std::tie(ret, code, message) = get_response_message(response);
        if(!ret) {
            LOG(ERROR) << "options failed! error:get_response_message error!";
            return false;
        }

        LOG(INFO) << code;
        LOG(INFO) << message;
        LOG(INFO) << response;

        return true;
    }


    bool RTSPClient::describe() {
        std::string request = "DESCRIBE " + url_ + " " + RTSP_VERSION + "\r\n";
        set_request_header(request, "CSeq", std::to_string(++cseq_));
        set_request_header(request, "User-Agent", USER_AGENT);

        set_request_header(request, "Accept", "application/sdp");
        request.append("\r\n");

        std::string response;
        sendRequest(request, response);
        if(response.empty()) {
            LOG(ERROR) << "describe FAILED! message: response null";
            return false;
        }

        bool ret = false;
        std::string code;
        std::string message;
        std::tie(ret, code, message) = get_response_message(response);
        if(!ret) {
            LOG(ERROR) << "options failed! error:get_response_message error!";
            return false;
        }

        if(code == "401") {
            std::string www_authenticate;
            get_response_header(response, "WWW-Authenticate", www_authenticate);
            if(www_authenticate.empty()) {
                LOG(ERROR) << "describe FAILED! message: WWW-Authenticate not found!";
                return false;
            }

            auto get_attribute = [&] (std::string &&attribute) {
                auto attribute_start = www_authenticate.find(attribute) + attribute.size();
                auto attribute_end = www_authenticate.find('\"', attribute_start);
                return www_authenticate.substr(attribute_start, attribute_end - attribute_start);
            };

            std::string realm = get_attribute("realm=\"");
            std::string nonce = get_attribute("nonce=\"");
            std::string stale = get_attribute("stale=\"");

            std::string temp1 = user_name_ + ':' + realm + ':' + password_;
            uint8_t md5val1[16] = {0};
            char A1hash[33] = {0};
            ::av_md5_sum(md5val1, (const uint8_t *)temp1.c_str(), temp1.size());
            data_to_hex(A1hash, md5val1, 16, 1);

            authorization_prefix_.append(A1hash);
            authorization_prefix_.append(":");
            authorization_prefix_.append(nonce);
            authorization_prefix_.append(":");
            std::string cmd_fmt = R"(Digest username="%s", realm="%s", nonce="%s", uri="%s", response=)";
            char authorization[10240] = {0};
            ::snprintf(authorization, 10240, cmd_fmt.c_str(),
                       user_name_.c_str(), realm.c_str(), nonce.c_str(), url_.c_str());
            authorization_.assign(authorization);
            std::string authorization_test = set_authorization("DESCRIBE");

            std::string request_new = "DESCRIBE " + url_ + " " + RTSP_VERSION + "\r\n";
            std::stringstream cseq_new;
            cseq_new << ++cseq_;
            set_request_header(request_new, "CSeq", cseq_new.str());
            set_request_header(request_new, "User-Agent", USER_AGENT);
            set_request_header(request_new, "Authorization", authorization_test);
            set_request_header(request_new, "Accept", "application/sdp");
            request_new.append("\r\n");

            sendRequest(request_new, response);

            LOG(INFO) << response.substr(response.find("\r\n\r\n")+4);
        }

        return false;
    }

    bool RTSPClient::setup() {
        std::string request = "SETUP " + url_+"/trackID=1"+ + " " + RTSP_VERSION + "\r\n";
        set_request_header(request, "CSeq", std::to_string(++cseq_));
        set_request_header(request, "User-Agent", USER_AGENT);
        set_request_header(request, "Authorization", set_authorization("SETUP"));
        set_request_header(request, "Accept", "application/sdp");

        get_udp_transport();
        std::stringstream transport;
        transport << "RTP/AVP;unicast;client_port=" << rtp_port_ << "-" << rtcp_port_;
        set_request_header(request, "Transport", transport.str());
        request.append("\r\n");

        std::string response;
        sendRequest(request, response);
        if(response.empty()) {
            LOG(ERROR) << "describe FAILED! message: response null";
            return false;
        }

        std::string session;
        get_response_header(response, "Session", session);

        std::regex regex_session(R"(([0-9]{8,20});)");
        std::smatch smatch_session;
        bool ret = std::regex_search(session, smatch_session, regex_session);
        if (!ret) {
            LOG(ERROR) << "describe FAILED! message: session not found!";
            return false;
        }

        session_ = smatch_session.str(1);

        return true;
    }

    void RTSPClient::receive_rtp_packet_thread_body() {
        while (receive_status_) {
            auto *pkt = new RTPPacket();
            int recv_len = rtp_socket_->recvfrom(pkt->buf, RTPPacket::RTSP_MTU);
            if (recv_len <= 0) {
                safe_deletep(pkt);
                break;
            }
            pkt->len = recv_len;
            rtp_queue_.push(pkt);
        }
    }

    bool RTSPClient::play() {
        std::string request = "PLAY " + url_ + + " " + RTSP_VERSION + "\r\n";
        set_request_header(request, "CSeq", std::to_string(++cseq_));
        set_request_header(request, "User-Agent", USER_AGENT);
        set_request_header(request, "Authorization", set_authorization("SETUP"));
        set_request_header(request, "Session", session_);
        set_request_header(request, "Range", "npt=0.000-");
        request.append("\r\n");

        std::string response;
        sendRequest(request, response);
        if(response.empty()) {
            LOG(ERROR) << "describe FAILED! message: response null";
            return false;
        }

        LOG(INFO) << response;

        bool ret = false;
        std::string code;
        std::string message;
        std::tie(ret, code, message) = get_response_message(response);
        if(!ret) {
            LOG(ERROR) << "play failed! error:get_response_message error!";
            return false;
        }

        if (code != "200") {
            LOG(ERROR) << "play failed! error:get_response_message code!";
            return false;
        }

        rtp_socket_ = new Infra::Net::UDPSocket();
        rtcp_socket_ = new Infra::Net::UDPSocket();

        rtp_socket_->open();
        rtcp_socket_->open();

        rtp_socket_->bind(rtp_port_);
        rtcp_socket_->bind(rtcp_port_);

        receive_status_ = true;
        receive_rtp_packet_thread_ = new std::thread(&RTSPClient::receive_rtp_packet_thread_body, this);

        return true;
    }

    bool RTSPClient::teardown() {
        return false;
    }

    bool RTSPClient::set_parameter() {
        return false;
    }

    bool RTSPClient::get_parameter() {
        return false;
    }

    bool RTSPClient::get_udp_transport() {
        auto *rtp_port_temp_socket = new Infra::Net::UDPSocket();
        auto *rtcp_port_temp_socket = new Infra::Net::UDPSocket();

        int i = 0;
        for(i = RTSP_PORT_MIN; i < RTSP_PORT_MAX; i += 2) {
            rtp_port_temp_socket->open();
            rtcp_port_temp_socket->open();

            if ((rtp_port_temp_socket->bind(i) == 0) && (rtcp_port_temp_socket->bind(i + 1) == 0)) {
                rtp_port_temp_socket->close();
                rtcp_port_temp_socket->close();
                break;
            }

            rtp_port_temp_socket->close();
            rtcp_port_temp_socket->close();
        }

        if (i < RTSP_PORT_MAX) {
            rtp_port_ = i;
            rtcp_port_ = i + 1;
            return true;
        }

        return false;
    }

    std::string RTSPClient::set_authorization(const std::string &cmd) {
        std::string temp2 = cmd + ':' + url_;
        uint8_t md5val2[16] = {0};
        char A2hash[33] = {0};
        ::av_md5_sum(md5val2, (const uint8_t *)temp2.c_str(), temp2.size());
        data_to_hex(A2hash, md5val2, 16, 1);

        std::string temp3 = authorization_prefix_.append(A2hash);
        uint8_t md5val3[16] = {0};
        char A3hash[33] = {0};
        ::av_md5_sum(md5val3, (const uint8_t *)temp3.c_str(), temp3.size());
        data_to_hex(A3hash, md5val3, 16, 1);

        return authorization_.append("\"").append(A3hash).append("\"");
    }

    char * RTSPClient::data_to_hex(char *buff, const uint8_t *src, int s, int lowercase) {
        int i;
        static const char hex_table_uc[16] = { '0', '1', '2', '3',
                                               '4', '5', '6', '7',
                                               '8', '9', 'A', 'B',
                                               'C', 'D', 'E', 'F' };
        static const char hex_table_lc[16] = { '0', '1', '2', '3',
                                               '4', '5', '6', '7',
                                               '8', '9', 'a', 'b',
                                               'c', 'd', 'e', 'f' };
        const char *hex_table = lowercase ? hex_table_lc : hex_table_uc;

        for (i = 0; i < s; i++) {
            buff[i * 2]     = hex_table[src[i] >> 4];
            buff[i * 2 + 1] = hex_table[src[i] & 0xF];
        }

        return buff;
    }

    bool RTSPClient::sendRequest(const std::string &request, std::string &response) {
        static char recv_temp_buffer[10240] = {0};
        int ret = rtsp_socket_->send(request.c_str(), request.size());
        if (ret < 0) {
            LOG(ERROR) << "rtsp_socket_->send FAILED! errno: " << errno << " message:" << strerror(errno);
            return false;
        }

        ret = rtsp_socket_->recv(recv_temp_buffer, 10240);
        if (ret < 0) {
            LOG(ERROR) << "rtsp_socket_->recv FAILED! errno: " << errno << " message:" << strerror(errno);
            return false;
        }
        response.assign(recv_temp_buffer);
        std::memset(recv_temp_buffer, 0, 10240);
        return true;
    }

    void RTSPClient::set_request_header(std::string &request, const std::string &name, const std::string &value) {
        std::string line = name + ':' + value + "\r\n";
        request.append(line);
    }


    void RTSPClient::get_response_header(const std::string &response, const std::string &name, std::string &value) {
        auto pos_start = response.find(name + ':');
        if (pos_start == std::string::npos) {
            return ;
        }

        auto pos_end = response.find("\r\n", pos_start);
        if (pos_end == std::string::npos) {
            return ;
        }

        value = response.substr(pos_start + name.size() + 1, pos_end);
    }

    std::tuple<bool, std::string, std::string>
    RTSPClient::get_response_message(const std::string &response) {
        std::regex regex_header(R"(^RTSP/1.0 ([0-9]{1,3}) (\w{1,128}))");
        std::smatch smatch_header;
        bool ret = std::regex_search(response, smatch_header, regex_header);
        return std::tuple<bool, std::string, std::string>(ret, smatch_header.str(1), smatch_header.str(2));
    }

    bool RTSPClient::read_frame(AVPacket *pkt) {
        bool demux_status = false;

        while(!demux_status) {
            auto rtp_pkt = rtp_queue_.pop();
            if (rtp_pkt == nullptr) {
                return false;
            }

            demux_status = demuxer_->demux(rtp_pkt, pkt);
        }

        return true;
    }

    enum AVCodecID RTSPClient::getAVCodecID() {
        return AV_CODEC_ID_H264;
    }

    double RTSPClient::getFrameRate() {
        return 0;
    }

    bool RTSPClient::close() {
        return false;
    }
}