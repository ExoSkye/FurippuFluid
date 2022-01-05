#include <thread>
#include <Tracy.hpp>

#include "FluidVoxel.hpp"
#include "viewer.hpp"

#define TYPE double
#define X 1001
#define Y 1001

bool toClose = false;

void run_viewer(MDArray<TYPE, X, Y>* toDraw) {
    tracy::SetThreadName("Viewer");
    Viewer<TYPE, X, Y> viewer{1};

    while (true) {
        ZoneScopedN("Run viewer")
        if (!viewer.update(*toDraw)) {
            toClose = true;
            break;
        }
    }
}

int main(int, char**) {
    tracy::SetThreadName("Main Thread");

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
                sim.set_density(1.0, i, j);
            }
        }
    }

    {
        ZoneScopedN("Main loop")

        std::thread viewer_thread(run_viewer, &sim.Density);

        while (!toClose) {
            ZoneScopedN("Running main loop")
            sim.Prev_Density = sim.Density;
            sim.diffuse(sim.Density, sim.Prev_Density, 0.1, 0.00001, 20);
        }
        {
            ZoneScopedN("Waiting for viewer to exit")
            viewer_thread.join();
        }
    }
}