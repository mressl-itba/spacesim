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
 * @brief Loads orbital body rendering resources, such as models and textures.
 */
void LoadOrbitalBodyRenderer();

/**
 * @brief Unloads orbital body rendering resources to free memory.
 */
void UnloadOrbitalBodyRenderer();

/**
 * @brief Retrieves the position of an orbital body in render coordinates.
 *
 * @param sim Pointer to the orbital simulation state.
 * @param body_index Index of the body to retrieve the position for.
 *
 * @return A Vector3 representing the position of the body in render coordinates.
 */
Vector3 GetOrbitalBodyPosition(const OrbitalSim *sim, uint32_t body_index);

/**
 * @brief Draws a single orbital body using raylib.
 *
 * @param camera The camera to use for rendering.
 * @param sim Pointer to the orbital simulation state.
 * @param body_index Index of the body to draw.
 */
void DrawOrbitalBody(Camera3D *camera, const OrbitalSim *sim, uint32_t body_index);

#endif // ORBITAL_BODY_H_
