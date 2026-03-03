/**
 * SPACESIM!
 * Orbital rendering module
 *
 * @author The glitched engineers
 */

#ifndef ORBITAL_RENDERER_H_
#define ORBITAL_RENDERER_H_

#include "../physics/solver.h"

// Screen parameters
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE "SPACESIM! - Solar System Simulator"

// Render parameters
#define RENDER_SCALE 1e-9f  // Scale factor for world units to render units
#define GRID_SPACING 100e9f // 100 million km per grid line

/**
 * @brief Type definition for the callback function that will be called each frame to update the simulation state.
 */
typedef void (*OnFrame)(OrbitalSim *, float);

/**
 * @brief Initializes the rendering window and starts the main render loop for the orbital simulation.
 * 
 * @param sim Pointer to the OrbitalSim structure containing the simulation data to visualize.
 * @param on_frame Callback function that will be called each frame to update the simulation state.
 */
void RenderOrbitalSim(OrbitalSim *sim, OnFrame on_frame);

#endif // ORBITAL_RENDERER_H_
