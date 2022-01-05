#pragma once

#include <glm/glm.hpp>
#include "MDArray.hpp"

template<typename T, unsigned x, unsigned y>
class FluidSim {
  public:
    MDArray<T, x, y> Vx{};
    MDArray<T, x, y> Vy{};

    MDArray<T, x, y> Prev_Vx{};
    MDArray<T, x, y> Prev_Vy{};

    MDArray<T, x, y> Density{};
    MDArray<T, x, y> Prev_Density{};

    void solve(MDArray<T, x, y>& values, MDArray<T, x, y>& prev_values, T k, T c, int iter) {
        ZoneScopedN("Running solve")
        T cRecip = 1.0 / c;

        for (int cur_iter = 0; cur_iter < iter; cur_iter++) {
            ZoneScopedN("Running GaussSeidel iteration")
            for (int j = 1; j < m_Size.y - 1; j++) {
                ZoneScopedN("Running Y iteration")
                for (int i = 1; i < m_Size.x - 1; i++) {
                    ZoneScopedN("Running X iteration")
                    values[i][j] =
                        (prev_values[i][j]
                            + k * (
                                    values[i + 1][j]
                                +   values[i - 1][j]
                                +   values[i][j + 1]
                                +   values[i][j - 1]
                            )
                        ) * cRecip;
                }
            }
        }
    }

    void diffuse(MDArray<T, x, y>& values, MDArray<T, x, y>& prev_values, T diff, T dt, int iter) {
        ZoneScopedN("Running diffuse")
        float a = dt * diff * (m_Size.x + 2) * (m_Size.y + 2);
        solve(values, prev_values, a, 1 + 6 * a, iter);
    }

    void set_density(T value, unsigned int x_coord, unsigned int y_coord) {
        ZoneScopedN("Setting density at point")
        Density[x_coord][y_coord] = value;
    }

  private:
    static constexpr glm::vec<2, unsigned int, glm::defaultp> m_Size = {x - 1, y - 1};
};