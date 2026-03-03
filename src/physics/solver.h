/**
 * SPACESIM!
 * N-body physics solver
 *
 * @author The glitched engineers
 */

#ifndef ORBITAL_SIMULATOR_H_
#define ORBITAL_SIMULATOR_H_

#include <cstdint>

/**
 * @brief Structure representing a celestial body in the simulation
 */
struct Body
{
    float mass;        // Mass in kilograms
    float mean_radius; // Mean radius in meters (for visualization)
    float position[3]; // Position in meters (x, y, z)
    float velocity[3]; // Velocity in meters per second (vx, vy, vz)
};

/**
 * @brief Structure representing the state of the N-body simulation
 */
struct OrbitalSim
{
    uint32_t num_bodies; // Number of bodies in the simulation
    Body *bodies;        // Dynamically allocated array of bodies

    double time;  // Current simulation time in seconds
};

/**
 * @brief Initializes the OrbitalSim structure with the specified number of bodies
 *
 * @param num_bodies The number of bodies to allocate in the simulation
 * @param initial_time The initial simulation time in seconds (UNIX epoch)
 *
 * @return Pointer to the initialized OrbitalSim structure, or nullptr on failure
 */
OrbitalSim *CreateOrbitalSim(uint32_t num_bodies, double initial_time);

/**
 * @brief Initializes the OrbitalSim structure by loading body data from a CSV file
 *
 * Expected CSV format (header required):
 * name,mass[kg],mean_radius[km],position_x[km],position_y[km],position_z[km],velocity_x[km/s],velocity_y[km/s],velocity_z[km/s]
 *
 * Example:
 * Sun,1.989e30,696340.0,0.0,0.0,0.0,0.0,0.0,0.0
 * Earth,5.972e24,6371.0,149600000.0,0.0,0.0,29.78,0.0,0.0
 *
 * @param filename Path to the file containing body data
 *
 * @return Pointer to the initialized OrbitalSim structure, or nullptr on failure
 */
OrbitalSim *CreateOrbitalSimFromFile(const char *filename);

/**
 * @brief Frees the resources allocated for the OrbitalSim structure
 *
 * @param sim Pointer to the OrbitalSim structure to destroy
 */
void DestroyOrbitalSim(OrbitalSim *sim);

/**
 * @brief Simulates a single time step of the N-body physics simulation
 *
 * @param sim Pointer to the OrbitalSim structure containing the bodies and their properties
 * @param time_step The time step to simulate (in seconds)
 */
void SimulateOrbitalSimStep(OrbitalSim *sim, float time_step);

#endif // ORBITAL_SIMULATOR_H_
