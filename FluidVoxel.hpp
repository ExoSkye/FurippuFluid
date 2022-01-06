#pragma once

#include <glm/glm.hpp>
#include "MDArray.hpp"

template<typename T, unsigned x, unsigned y>
class SwapPair {
  public:
    MDArray<T, x, y>* current = &mArray1;
    MDArray<T, x, y>* previous = &mArray2;

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

    MDArray<T, x, y>& get_current() {
        return *current;
    }

    MDArray<T, x, y>& get_previous() {
        return *previous;
    }

  private:
    bool mSwapped = false;

    MDArray<T, x, y> mArray1{};
    MDArray<T, x, y> mArray2{};
};

template<typename T, unsigned x, unsigned y>
class FluidSim {
  public:
    SwapPair<T, x, y> Vx;
    SwapPair<T, x, y> Vy;

    SwapPair<T, x, y> Density;

    void solve(MDArray<T, x, y>& values, MDArray<T, x, y>& prev_values, T k, T c, int iter) {
        ZoneScopedN("Running solve")
        T cRecip = 1.0 / c;

        for (int cur_iter = 0; cur_iter < iter; cur_iter++) {
            ZoneScopedN("Running GaussSeidel iteration")
            for (int j = 1; j < m_Size.y - 1; j++) {
                //ZoneScopedN("Running Y iteration")
                for (int i = 1; i < m_Size.x - 1; i++) {
                    //ZoneScopedN("Running X iteration")
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

    void diffuse(SwapPair<T, x, y>& values, T diff, T dt, int iter) {
        ZoneScopedN("Running diffuse")
        float a = dt * diff * (m_Size.x - 2) * (m_Size.y - 2);
        solve(values.get_current(), values.get_previous(), a, 1 + 4 * a, iter);
    }

    void set_density(T value, unsigned int x_coord, unsigned int y_coord) {
        ZoneScopedN("Setting density at point")
        Density.get_current()[x_coord][y_coord] = value;
    }

  private:
    static constexpr glm::vec<2, unsigned int, glm::defaultp> m_Size = {x, y};
};