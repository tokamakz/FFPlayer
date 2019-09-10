#include <chrono>
#include "SDLRender.h"

#include "glog/logging.h"

namespace simple_player {
    SDLRender::SDLRender() {
        sdl_window_ = nullptr;
        sdl_renderer_ = nullptr;
        sdl_texture_ = nullptr;
    }

    SDLRender::~SDLRender() = default;

    bool SDLRender::open() {
        if (SDL_Init(SDL_INIT_VIDEO)) {
            LOG(ERROR) << "SDL_Init ERROR" << SDL_GetError();
            return false;
        }
        SDL_Window *screen = SDL_CreateWindow("FFPlayer", 0, 0, 960, 540, SDL_WINDOW_OPENGL);
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
        SDL_Quit();
    }

    long SDLRender::render(AVFrame* frame) {
        auto start_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        SDL_UpdateYUVTexture(sdl_texture_, nullptr,
                             frame->data[0], frame->linesize[0],
                             frame->data[1], frame->linesize[1],
                             frame->data[2], frame->linesize[2]);
        SDL_RenderClear(sdl_renderer_);
        SDL_RenderCopy(sdl_renderer_, sdl_texture_, nullptr, nullptr);
        SDL_RenderPresent(sdl_renderer_);
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_time;
    }
}