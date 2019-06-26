#include <signal.h>

#include <iostream>

#include <thread>

#include "glog/logging.h"
#include "FFPlayer.h"

static void force_exit(int arg) {
    LOG(INFO) << "video_player stop!!!";
    exit(0);
}

static void init_log() {
    google::InitGoogleLogging("FFPlayer");
//    google::SetLogDestination(google::GLOG_INFO, "/home/user1/projects/log/video_player/INFO_");
    google::SetStderrLogging(google::GLOG_INFO);
//    google::SetLogFilenameExtension("log_");
    FLAGS_colorlogtostderr = true;
}

int main() {
    ::signal(SIGINT, force_exit);

//    using namespace simple_player;
//    auto player = new FFPlayer();
//    player->open("rtsp://admin:seemmo123@10.10.19.130");

//    {
//        std::thread th([]{
//            while(true) {
//                std::this_thread::sleep_for(std::chrono::seconds(1));
//                std::cout << "hello" << std::endl;
//            }
//        });
//        th.detach();
//    }

    std::this_thread::sleep_for(std::chrono::seconds(1000));

    return 0;
}
