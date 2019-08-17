#include <csignal>

#include <thread>

#include "glog/logging.h"
#include "FFPlayer.h"

static void force_exit(int arg) {
    LOG(INFO) << "FFPlayer stop!!!";
    exit(0);
}

static void init_log() {
    google::InitGoogleLogging("FFPlayer");
    google::SetLogDestination(google::GLOG_ERROR, "/home/user1/projects/log/video_player/ERROR_");
    google::SetStderrLogging(google::GLOG_ERROR);
    google::SetLogFilenameExtension("log_");
    FLAGS_colorlogtostderr = true;
}

int main() {
    ::signal(SIGINT, force_exit);
    using namespace simple_player;
    auto player = new FFPlayer();
    bool bRet = player->open("rtsp://192.168.3.43:8554/test.264");
    if(!bRet) {
        LOG(ERROR) << "player->open fail!";
        return 0;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1000));
    return 0;
}
