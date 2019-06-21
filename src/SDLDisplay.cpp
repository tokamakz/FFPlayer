#include <thread>

#include "SDLDisplay.h"

namespace simple_player {
    SDLDisplay::SDLDisplay() {
    }

    SDLDisplay::~SDLDisplay() {
    }

    bool SDLDisplay::init() {
        if (SDL_Init(SDL_INIT_VIDEO)) {
            fprintf(stderr, "[video_player] Error: Could not initialize. %s\n", SDL_GetError());
            return -1;
        }
        SDL_Window *screen = SDL_CreateWindow("video_player", 0, 0, 950, 540, SDL_WINDOW_OPENGL);
        if (!screen) {
            fprintf(stderr, "[video_player] Error: could not create window. %s\n", SDL_GetError());
            return -1;
        }

        sdl_renderer_ = SDL_CreateRenderer(screen, -1, 0);
        sdl_texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_IYUV,
                SDL_TEXTUREACCESS_STREAMING, 1920, 1080);

        return false;
    }

    void SDLDisplay::de_init() {
    }

    void SDLDisplay::start() {

        std::thread th([&]() {
            AVFrame* frame = nullptr;
            {
                std::lock_guard<std::mutex> lock(list_mutex_);
                if (!frame_list_.empty()) {
                    frame = frame_list_.front();
                    frame_list_.pop_front();
                }
            }

            if (frame != nullptr) {
                display(frame);
            }
        });
        th.detach();
    }

    void SDLDisplay::display(AVFrame* frame) {
        SDL_UpdateYUVTexture(sdl_texture_, nullptr,
                             frame->data[0], frame->linesize[0],
                             frame->data[1], frame->linesize[1],
                             frame->data[2], frame->linesize[2]);
        SDL_RenderClear(sdl_renderer_);
        SDL_RenderCopy(sdl_renderer_, sdl_texture_, nullptr, nullptr);
        SDL_RenderPresent(sdl_renderer_);
    }

    void SDLDisplay::onData(AVFrame* frame) {
        std::lock_guard<std::mutex> lock(list_mutex_);
        frame_list_.push_back(frame);
    }
}