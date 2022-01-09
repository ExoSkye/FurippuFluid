#pragma once

#include <glm/glm.hpp>
#include "MDArray.hpp"

template<typename T, unsigned ... Dims>
class SwapPair {
  public:
    MDArray<T, Dims...>* current = &mArray1;
    MDArray<T, Dims...>* previous = &mArray2;

    void swap() {
        if (mSwapped) {
            current = &mArray1;
            previous = &mArray2;
        }
        else {
            current = &mArray2;
            previous = &mArray1;
        }
        mSwapped = !mSwapped;
    }

    MDArray<T, Dims...>& get_current() {
        return *current;
    }

    MDArray<T, Dims...>& get_previous() {
        return *previous;
    }

  private:
    bool mSwapped = false;

    MDArray<T, Dims...> mArray1{};
    MDArray<T, Dims...> mArray2{};
};

template<typename T, unsigned x, unsigned y>
class FluidSim {
  public:
    typedef glm::vec<2, T, glm::defaultp> vectorT;
    SwapPair<T, x, y> velocity_x;
    SwapPair<T, x, y> velocity_y;
    SwapPair<T, x, y> density;

    void solve(MDArray<T, x, y>& values, MDArray<T, x, y>& prev_values, T diff, T dt, int iter) {
        ZoneScopedN("Running solve")
        T k = dt * diff * (m_Size.x - 2) * (m_Size.y - 2);
        T c = 1 + 4 * k;
        T cRecip = 1.0 / c;

        for (int cur_iter = 0; cur_iter < iter; cur_iter++) {
            ZoneScopedN("Running GaussSeidel iteration")
            for (int j = 1; j < m_Size.y - 1; j++) {
                for (int i = 1; i < m_Size.x - 1; i++) {
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

    void diffuse_density(T diff, T dt, int iter) {
        ZoneScopedN("Running diffuse on density")
        solve(density.get_current(), density.get_previous(), diff, dt, iter);
        density.swap();
    }

    void diffuse_velocity(T diff, T dt, int iter) {
        ZoneScopedN("Running diffuse on velocity")
        solve(velocity_x.get_current(), velocity_x.get_previous(), diff, dt, iter);
        solve(velocity_y.get_current(), velocity_y.get_previous(), diff, dt, iter);
        velocity_x.swap();
        velocity_y.swap();
    }

    void advect_density(T dt) {
        ZoneScopedN("Running advection on density")
        advect(density.get_current(), density.get_previous(),
               velocity_x.get_current(), velocity_y.get_current(), dt
        );
    }

    void advect_velocity(T dt) {
        ZoneScopedN("Running advection on velocity")
        advect(
            velocity_x.get_current(), velocity_x.get_previous(),
            velocity_x.get_previous(), velocity_y.get_previous(), dt
        );
        advect(
            velocity_y.get_current(), velocity_y.get_previous(),
            velocity_x.get_previous(), velocity_y.get_previous(), dt
        );
    }

    void advect(MDArray<T, x, y>& values, MDArray<T, x, y>& prev_values,
                MDArray<T, x, y>& velo_x, MDArray<T, x, y>& velo_y, T dt) {

        ZoneScopedN("Running advection")

        glm::vec2 dt_vec = {dt * (m_Size.x - 2), dt * (m_Size.y - 2)};

        for (int j = 1; j < m_Size.x - 1; j++) {
            for (int i = 1; i < m_Size.y - 1; i++) {
                glm::vec2 coord = {
                    (T)i - (dt_vec.x * velo_x[i][j]),
                    (T)j - (dt_vec.y * velo_y[i][j])
                };

                if (coord.x < 0.5) coord.x = 0.5;
                if (coord.x > m_Size.x + 0.5) coord.x = m_Size.x + 0.5;

                if (coord.y < 0.5) coord.y = 0.5;
                if (coord.y > m_Size.y + 0.5) coord.y = m_Size.y + 0.5;

                vectorT coord0 = {
                    (int)floor((double)coord.x),
                    (int)floor((double)coord.y),
                };

                vectorT coord1 = coord0 + vectorT{1, 1};

                vectorT coord_s1 = {
                    coord.x - coord0.x,
                    coord.y - coord0.y
                };

                vectorT coord_s0 = vectorT{1, 1} - coord_s1;

                if (coord1.x >= x) {
                    coord1.x = x - 1;
                }

                if (coord1.y >= y) {
                    coord1.y = y - 1;
                }

                if (coord0.x >= x) {
                    coord0.x = x - 1;
                }

                if (coord0.y >= y) {
                    coord0.y = y - 1;
                }


                values[i][j] =
                    coord_s0.x * (
                        (coord_s0.y * prev_values[coord0.x][coord0.y]) +
                        (coord_s1.y * prev_values[coord0.x][coord1.y])
                    ) +
                    coord_s1.x * (
                        (coord_s0.y * prev_values[coord1.x][coord0.y]) +
                        (coord_s1.y * prev_values[coord1.x][coord1.y])
                    );
            }
        }
    }

    void set_density(T value, unsigned int x_coord, unsigned int y_coord) {
        ZoneScopedN("Setting density at point")
        density.get_current()[x_coord][y_coord] = value;
    }

    void set_velocity(vectorT value, unsigned int x_coord, unsigned int y_coord) {
        ZoneScopedN("Setting density at point")
        velocity_x.get_current()[x_coord][y_coord] = value.x;
        velocity_y.get_current()[x_coord][y_coord] = value.y;
    }

  private:
    static constexpr glm::vec<2, unsigned int, glm::defaultp> m_Size = {x, y};
};