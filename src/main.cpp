#include <csignal>

#include <thread>

#include "glog/logging.h"
#include "FFPlayer.h"

static void force_exit(int arg) {
    LOG(INFO) << "FFPlayer stop!!!";
    exit(0);
}

int main() {
    ::signal(SIGINT, force_exit);
    using namespace simple_player;
    auto player = new FFPlayer();

    //rtsp://admin:q1495359275@192.168.3.48:554
    ///home/user1/test.mp4
    bool bRet = player->open("rtsp://192.168.3.48");
    if(!bRet) {
        LOG(ERROR) << "player->open fail!";
        return 0;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1000));
    return 0;
}
