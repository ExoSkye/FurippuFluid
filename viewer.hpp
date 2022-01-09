#pragma once

#include <SDL.h>
#include <algorithm>
#include "MDArray.hpp"
#include <Tracy.hpp>

template <typename T, unsigned int x, unsigned int y>
class Viewer {
  public:
    enum ViewType {
        Density,
        Velocity,
        Combined,
        Add,
        Subtract
    };

    Viewer(int scale) {
        SDL_Init(SDL_INIT_VIDEO);
        m_Scale = scale;
        m_Window = SDL_CreateWindow("Output", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x*m_Scale, y*m_Scale,
                                    SDL_WINDOW_VULKAN);
    }

    bool update(MDArray<T, x, y>& density, MDArray<T, x, y>& velo_x, MDArray<T, x, y>& velo_y, bool* wantToPause,
                bool* paused) {
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
                    if (m_Dragging) {
                        *wantToPause = true;
                        while (!(*paused)) {}
                        m_EndPos = {e.motion.x, e.motion.y};
                        for (int i = m_StartPos.x - m_Size / 2; i < m_StartPos.x + m_Size / 2; i++) {
                            for (int j = m_StartPos.y - m_Size / 2; j < m_StartPos.y + m_Size / 2; j++) {
                                if (m_ControlType == Add) {
                                    velo_x[i][j] += (m_EndPos.x - m_StartPos.x);
                                    velo_y[i][j] += (m_EndPos.y - m_StartPos.y);
                                }
                                else {
                                    velo_x[i][j] -= (m_EndPos.x - m_StartPos.x);
                                    velo_y[i][j] -= (m_EndPos.y - m_StartPos.y);
                                }
                            }
                        }
                        *wantToPause = false;
                    }
                    break;

                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_v:
                            m_ViewType = Velocity;
                            break;

                        case SDLK_d:
                            m_ViewType = Density;
                            break;

                        case SDLK_c:
                            m_ViewType = Combined;
                            break;

                        case SDLK_KP_PLUS:
                            m_ControlType = Add;
                            break;

                        case SDLK_KP_MINUS:
                            m_ControlType = Subtract;
                            break;
                    }
            }
        }

        SDL_Surface* surface = SDL_GetWindowSurface(m_Window);

        T max_value_density = 0;
        T max_value_velocity = 0;


        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                if (m_ViewType == Density || m_ViewType == Combined) {
                    max_value_density = std::max(density[i][j], max_value_density);
                }
                if (m_ViewType == Velocity || m_ViewType == Combined) {
                    max_value_velocity = std::max(glm::abs(velo_x[i][j]), max_value_velocity);
                    max_value_velocity = std::max(glm::abs(velo_y[i][j]), max_value_velocity);
                }

            }
        }

        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                uint8_t dens;
                uint8_t veloc_x;
                uint8_t veloc_y;
                if (m_ViewType == Density || m_ViewType == Combined) {
                    dens = density[i][j] / max_value_density * 255;
                }

                if (m_ViewType == Velocity || m_ViewType == Combined) {
                    veloc_x = glm::abs(velo_x[i][j] / max_value_velocity) * 255;
                    veloc_y = glm::abs(velo_y[i][j] / max_value_velocity) * 255;
                }

                switch (m_ViewType) {
                    case Density:
                        ((uint32_t*) surface->pixels)[j * (surface->pitch / sizeof(uint32_t)) + i] =
                            SDL_MapRGBA(surface->format, dens, dens, dens, 255);
                        break;

                    case Velocity:
                        ((uint32_t*) surface->pixels)[j * (surface->pitch / sizeof(uint32_t)) + i] =
                            SDL_MapRGBA(surface->format, 0, veloc_x, veloc_y, 255);
                        break;

                    case Combined:
                        ((uint32_t*) surface->pixels)[j * (surface->pitch / sizeof(uint32_t)) + i] =
                            SDL_MapRGBA(surface->format, dens, veloc_x, veloc_y, 255);
                        break;
                }
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
    ViewType m_ViewType = ViewType::Density;
    ViewType m_ControlType = ViewType::Add;
    int m_Scale;
    SDL_Window* m_Window;
    bool m_Dragging = false;
    glm::ivec2 m_StartPos{};
    glm::ivec2 m_EndPos;
    unsigned int m_Size = 10;
};