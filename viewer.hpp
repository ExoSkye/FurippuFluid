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

    bool update(MDArray<T, x, y>& density, MDArray<T, x, y>& velo_x, MDArray<T, x, y>& velo_y) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    return false;

                case SDL_MOUSEBUTTONDOWN:
                    m_StartPos = {e.motion.x, e.motion.y};
                    m_Dragging = true;
                    break;

                case SDL_MOUSEWHEEL:
                    if (m_Dragging) {
                        if (e.wheel.y > 0) {
                            m_Size += 10;
                        }
                        else if (e.wheel.y > 0 && m_Size > 10) {
                            m_Size -= 10;
                        }
                    }
                    if (m_Size < 10) m_Size = 10;
                    break;

                case SDL_MOUSEBUTTONUP:
                    glm::ivec2 end_pos = {e.motion.x, e.motion.y};
                    for (int i = m_StartPos.x - m_Size / 2; i < m_StartPos.x + m_Size / 2; i++) {
                        for (int j = m_StartPos.y - m_Size / 2; j < m_StartPos.y + m_Size / 2; j++) {
                            velo_x[i][j] = (end_pos.x - m_StartPos.x) / 10;
                            velo_y[i][j] = (end_pos.y - m_StartPos.y) / 10;
                        }
                    }
                    break;
            }
        }

        T max_value = 0;
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                max_value = std::max(density[i][j], max_value);
            }
        }

        SDL_Surface* surface = SDL_GetWindowSurface(m_Window);

        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                double t = density[i][j] / max_value;
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
        exit(0);
    }
  private:
    int m_Scale;
    SDL_Window* m_Window;
    bool m_Dragging = false;
    glm::ivec2 m_StartPos;
    unsigned int m_Size = 10;
};