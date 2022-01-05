#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <algorithm>
#include "MDArray.hpp"
#include <Tracy.hpp>

template <typename T, unsigned int x, unsigned int y>
class Viewer {
  public:
    Viewer(int scale) {
        SDL_Init(SDL_INIT_VIDEO);
        m_Scale = scale;
        m_Window = SDL_CreateWindow("Output", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x*m_Scale, y*m_Scale,
                                    SDL_WINDOW_VULKAN);
    }

    bool update(MDArray<T, x, y>& toDraw) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    return false;
            }
        }

        T max_value = 0;
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                max_value = std::max(toDraw[i][j], max_value);
            }
        }

        SDL_Surface* surface = SDL_GetWindowSurface(m_Window);

        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                double t = toDraw[i][j] / max_value;
                uint8_t darkness = t * 255;
                ((uint32_t*)surface->pixels)[j * (surface->pitch / sizeof(uint32_t)) + i] =
                    SDL_MapRGBA(surface->format, darkness, darkness, darkness, 255);
            }
        }

        SDL_UpdateWindowSurface(m_Window);
        return true;
    }

    ~Viewer() {
        SDL_DestroyWindow(m_Window);
    }
  private:
    int m_Scale;
    SDL_Window* m_Window;
};