#pragma once

#include <list>
#include <mutex>

#include "SDL2/SDL.h"
#include "libavutil/frame.h"

namespace simple_player {
    class SDLDisplay  {
    public:
        SDLDisplay();
        ~SDLDisplay();
        bool init();
        void de_init();
        void start();
        void display(AVFrame* frame);
        void onData(AVFrame* frame);

    private:
        SDL_Renderer *sdl_renderer_;
        SDL_Texture *sdl_texture_;
        std::mutex list_mutex_;
        std::list<AVFrame*> frame_list_;
    };
}

