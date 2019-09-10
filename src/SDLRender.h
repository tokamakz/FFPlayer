#pragma once

#include "SDL2/SDL.h"
#include "libavutil/frame.h"

namespace simple_player {
    class SDLRender  {
    public:
        SDLRender();
        ~SDLRender();
        bool open();
        void close();
        long render(AVFrame* frame);

    private:
        SDL_Window *sdl_window_;
        SDL_Renderer *sdl_renderer_;
        SDL_Texture *sdl_texture_;
    };
}

