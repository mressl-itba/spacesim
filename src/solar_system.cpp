/**
 * SPACESIM!
 * 3D visualization
 * Main module
 *
 * @author The glitched engineers
 */

#include <algorithm>
#include <iostream>

#include "physics/solver.h"
#include "renderer/renderer.h"

#define EPHEMERIS_DATA_FILE "resources/ephemerides/2028-12-31_00-00-00.csv"

// Simulation parameters
#define MIN_STEP_DT 60.0f // Minimum time step for physics updates to maintain stability

/**
 * @brief Update function called each frame to advance the simulation
 *
 * @param sim Pointer to the OrbitalSim structure representing the current state of the simulation
 * @param dt Time elapsed since the last frame (in seconds)
 */
static void OnScreenFrame(OrbitalSim *sim, float dt)
{
    // Update simulation state
    for (float t = 0.0f; t < dt; t += MIN_STEP_DT)
    {
        float current_dt = std::min(MIN_STEP_DT, dt - t);
        SimulateOrbitalSimStep(sim, current_dt);
    }
}

/**
 * @brief Main entry point of the application
 */
int main(int argc, char *argv[])
{
    OrbitalSim *sim = CreateOrbitalSimFromFile(EPHEMERIS_DATA_FILE);

    if (!sim)
    {
        std::cerr << "error: Failed to load solar system ephemeris data file: " << EPHEMERIS_DATA_FILE << "\n";
        return 1;
    }

    RenderOrbitalSim(sim, OnScreenFrame);

    DestroyOrbitalSim(sim);

    return 0;
}
