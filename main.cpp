#include <thread>
#include <cstdio>
#include <Tracy.hpp>

#include "FluidVoxel.hpp"
#include "viewer.hpp"

#define TYPE double
#define X 1000
#define Y 1000

bool toClose = false;
bool wantToPause = false;
bool paused = false;

void run_viewer(MDArray<TYPE, X, Y>* density, MDArray<TYPE, X, Y>* velo_x, MDArray<TYPE, X, Y>* velo_y) {
    tracy::SetThreadName("Viewer");
    Viewer<TYPE, X, Y> viewer{1};

    while (true) {
        ZoneScopedN("Run viewer")
        if (!viewer.update(*density, *velo_x, *velo_y, &wantToPause, &paused)) {
            toClose = true;
            break;
        }
    }
}

int main(int, char**) {
    tracy::SetThreadName("Simulator Thread");

    FluidSim<TYPE, X, Y> sim{};
    {
        ZoneScopedN("Initializing Simulation")
        for (int i = 0; i < X; i++) {
            for (int j = 0; j < Y; j++) {
                sim.set_density(0.0, i, j);
            }
        }
        for (int i = X / 10 * 4; i < X / 10 * 6; i++) {
            for (int j = Y / 10 * 4; j < Y / 10 * 6; j++) {
                sim.set_density(10.0, i, j);
            }
        }
    }

    {
        ZoneScopedN("Main loop")

        std::thread viewer_thread(run_viewer, &sim.density.get_current(),
                                  &sim.velocity_x.get_current(), &sim.velocity_y.get_current());

        TYPE dt = 0.01;
        //scanf("%f\n", &dt);
        int iter = 5;
        TYPE diff = 0.1;

        while (!toClose) {
            if (wantToPause) {
                paused = true;
                while (wantToPause);
                paused = false;
            }

            ZoneScopedN("Running main loop")
            sim.diffuse_velocity(diff, dt, iter);
            sim.advect_velocity(dt);

            sim.diffuse_density(diff, dt, iter);
            sim.advect_density(dt);

            FrameMark
        }
        {
            ZoneScopedN("Waiting for viewer to exit")
            viewer_thread.join();
        }
    }
}
