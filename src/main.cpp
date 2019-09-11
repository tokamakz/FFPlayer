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

    bool bRet = player->open("/home/user1/test.mp4");
    if(!bRet) {
        LOG(ERROR) << "player->open fail!";
        return 0;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1000));
    return 0;
}
