/**
 * SPACESIM!
 * Mission management for space simulation
 * 
 * @author The glitched engineers
 */

#ifndef MISSION_H_
#define MISSION_H_

#include "solver.h"

/**
 * @brief Initializes the mission state for the given orbital simulation.
 * 
 * @param sim Pointer to the OrbitalSim instance for which to initialize the mission.
 */
void LoadMission(OrbitalSim *sim);

/** 
 * @brief Unloads any resources associated with the mission state for the given orbital simulation.
 * 
 * @param sim Pointer to the OrbitalSim instance for which to unload the mission.
 */
void UnloadMission(OrbitalSim *sim);

/**
 * Performs a single step of the mission, updating the state of the orbital simulation accordingly.
 * 
 * @param sim Pointer to the OrbitalSim instance for which to perform the mission step.
 * 
 * @result The time step for the next simulation update, in seconds. This value can be used to control the simulation's precision.
 */
float PerformMissionStep(OrbitalSim *sim);

#endif // MISSION_H_
