/**
 * SPACESIM!
 * Headless orbital mechanics simulator
 * Main module
 *
 * @details Command-line simulation of orbital mechanics without visualization
 * @author The glitched engineers
 */

#include <iostream>

#include "physics/solver.h"
#include "physics/mission.h"

// Mission parameters
#define START_EPHEMERIDES "resources/ephemerides/2028-12-31_00-00-00.csv"
#define MISSION_DURATION (9 * 30 * 86400.0f)

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

    // Update simulation state
    float t = 0;
    while (t < MISSION_DURATION)
    {
        float time_step = PerformMissionStep(sim);

        SimulateOrbitalSimStep(sim, time_step);

        t += time_step;
    }

    // Clean up simulation resources after mission completion
    UnloadMission(sim);

    DestroyOrbitalSim(sim);

    return 0;
}
