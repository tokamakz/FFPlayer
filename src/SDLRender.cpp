#include "SDLRender.h"

#include "glog/logging.h"

namespace simple_player {
    SDLRender::SDLRender() {
    }

    SDLRender::~SDLRender() {
    }

    bool SDLRender::open() {
        if (SDL_Init(SDL_INIT_VIDEO)) {
            LOG(ERROR) << "SDL_Init ERROR" << SDL_GetError();
            return false;
        }
        SDL_Window *screen = SDL_CreateWindow("FFPlayer", 0, 0, 192, 108, SDL_WINDOW_OPENGL);
        if (!screen) {
            LOG(ERROR) << "SDL_CreateWindow ERROR" << SDL_GetError();
            return false;
        }

        sdl_renderer_ = SDL_CreateRenderer(screen, -1, 0);
        sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_IYUV,
                SDL_TEXTUREACCESS_STREAMING, 1920, 1080);

        return true;
    }

    void SDLRender::close() {
        SDL_DestroyTexture(sdl_texture_);
        SDL_DestroyRenderer(sdl_renderer_);
        SDL_DestroyWindow(sdl_window_);
    }

    void SDLRender::render(AVFrame* frame) {
        SDL_UpdateYUVTexture(sdl_texture_, nullptr,
                             frame->data[0], frame->linesize[0],
                             frame->data[1], frame->linesize[1],
                             frame->data[2], frame->linesize[2]);
        SDL_RenderClear(sdl_renderer_);
        SDL_RenderCopy(sdl_renderer_, sdl_texture_, nullptr, nullptr);
        SDL_RenderPresent(sdl_renderer_);
    }
}