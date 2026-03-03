/**
 * SPACESIM!
 * Orbital body rendering module
 * 
 * @author The glitched engineers
 */

#ifndef ORBITAL_BODY_H_
#define ORBITAL_BODY_H_

#include "../physics/solver.h"

#include "raylib.h"

/**
 * @brief Loads orbital body rendering resources (sphere mesh + textures).
 */
void ConstructOrbitalBodyRenderer();

/**
 * @brief Unloads orbital body rendering resources.
 */
void DestroyOrbitalBodyRenderer();

/**
 * @brief Draws a single orbital body using raylib.
 * 
 * @param camera The camera to use for rendering.
 * @param sim Pointer to the orbital simulation state.
 * @param body_index Index of the body to draw.
 */
void DrawOrbitalBody(Camera3D *camera, const OrbitalSim *sim, uint32_t body_index);

#endif // ORBITAL_BODY_H_
