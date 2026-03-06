/**
 * SPACESIM!
 * Interactive orbital mechanics simulator
 * Main module
 * 
 * @details Real-time visualization of orbital mechanics using ephemeris data, with an interactive HUD overlay
 * @author The glitched engineers
 */

#include <algorithm>
#include <iostream>

#include "physics/solver.h"
#include "physics/mission.h"
#include "renderer/renderer.h"

// Simulation parameters
#define START_EPHEMERIDES "resources/ephemerides/2028-12-31_00-00-00.csv"

/**
 * @brief Update function called each frame to advance the simulation
 *
 * @param sim Pointer to the OrbitalSim structure representing the current state of the simulation
 * @param dt Time elapsed since the last frame (in seconds)
 */
static void OnScreenFrame(OrbitalSim *sim, float frame_time)
{
    // Update simulation state
    float t = 0;
    while (t < frame_time)
    {
        float mission_time_step = PerformMissionStep(sim);

        float time_step = std::min(mission_time_step, frame_time - t);
        SimulateOrbitalSimStep(sim, time_step);

        t += time_step;
    }
}

/**
 * @brief Main entry point of the application
 */
int main(int argc, char *argv[])
{
    // Initialize mission state
    OrbitalSim *sim = CreateOrbitalSimFromFile(START_EPHEMERIDES);
    if (!sim)
    {
        std::cerr << "error: Failed to load solar system ephemeris data file: " << START_EPHEMERIDES << std::endl;

        exit(1);
    }

    LoadMission(sim);

    // Start rendering loop with the simulation state and frame update callback
    RenderOrbitalSim(sim, OnScreenFrame);

    // Clean up simulation resources after mission completion
    UnloadMission(sim);

    DestroyOrbitalSim(sim);

    return 0;
}
