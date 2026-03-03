/**
 * SPACESIM!
 * Orbital grid rendering module
 * 
 * @author The glitched engineers
 */

#ifndef ORBITAL_GRID_H_
#define ORBITAL_GRID_H_

#include <cstdint>

/**
 * @brief Draws a grid representing the orbital plane using raylib.
 * 
 * @param n Number of grid lines to draw in each direction from the center.
 * @param spacing Distance between adjacent grid lines.
 */
void DrawOrbitalGrid(uint32_t n, float spacing);

#endif // ORBITAL_GRID_H_
