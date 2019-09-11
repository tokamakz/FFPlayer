Introduction
============

FFPlayer is a audio and video player,use a variety of ways to receive media source data,decode and display.


Features
============

1.0.0

- using FFmpeg avformat to recevie and parse video stream data.
- using FFmpeg avcodec to decode video stream data as YUV Data.
- using SDL to render and display YUV data. 

2.0.0
- not using FFmpeg avformat


Build
============

yasm-1.3.0

from: http://yasm.tortall.net/

./configure --prefix=~/yasm

make && make install

add yasm/bin to your PATH



ffmpeg 4.2

from:https://github.com/FFmpeg/FFmpeg

./configure --prefix=~/ffmpeg --disable-small  --disable-autodetect --disable-debug

make && make install



SDL2-2.0.10

from:http://www.libsdl.org/

./configure --prefix=~/sdl

make && make install