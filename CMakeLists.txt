cmake_minimum_required(VERSION 3.10)
project(FFPlayer CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

#glog
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/glog/include)
link_libraries(${PROJECT_SOURCE_DIR}/3rdparty/glog/lib/libglog.a)

##gtest
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/gtest/include)
#link_libraries(${PROJECT_SOURCE_DIR}/3rdparty/gtest/lib/libgtest.a)

#FFmpeg
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/ffmpeg/include)
link_libraries(${PROJECT_SOURCE_DIR}/3rdparty/ffmpeg/lib/libavformat.a)
link_libraries(${PROJECT_SOURCE_DIR}/3rdparty/ffmpeg/lib/libavcodec.a)
link_libraries(${PROJECT_SOURCE_DIR}/3rdparty/ffmpeg/lib/libavutil.a)

#SDL
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/sdl/include)
link_libraries(${PROJECT_SOURCE_DIR}/3rdparty/sdl/lib/libSDL2.a)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(SRC_INFRA
        src/infra/net/BasicSocket.cpp src/infra/net/BasicSocket.h
        src/infra/net/TCPSocket.cpp src/infra/net/TCPSocket.h
        src/infra/net/UDPSocket.cpp src/infra/net/UDPSocket.h)

set(SRC_FFPLAYER
        src/FFPlayer.cpp src/FFPlayer.h
        src/source/ISource.cpp src/source/ISource.h
        src/source/rtsp/RTPPacket.h
        src/source/rtsp/Demuxer.cpp src/source/rtsp/Demuxer.h
        src/source/rtsp/RTPQueue.cpp src/source/rtsp/RTPQueue.h
        src/source/rtsp/RTSPClient.cpp src/source/rtsp/RTSPClient.h
        src/source/FFSource.cpp src/source/FFSource.h
        src/FFDecoder.cpp src/FFDecoder.h
        src/SDLRender.cpp src/SDLRender.h
        src/AVFrameQueue.cpp src/AVFrameQueue.h
        src/AVPacketQueue.cpp src/AVPacketQueue.h)

add_executable(FFPlayer src/main.cpp
        ${SRC_INFRA} ${SRC_FFPLAYER})

target_link_libraries(FFPlayer pthread)
target_link_libraries(FFPlayer dl)