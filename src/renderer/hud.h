/**
 * SPACESIM!
 * Orbital HUD rendering module
 *
 * @author The glitched engineers
 */

#ifndef ORBITAL_OVERLAY_H
#define ORBITAL_OVERLAY_H

#include "renderer.h"

/**
 * @brief Draws an overlay on top of the orbital simulation visualization, displaying information such as the current simulation time, speed, and pause status.
 *
 * @param paused Boolean indicating whether the simulation is currently paused, to display on the overlay.
 * @param following_ship Boolean indicating whether the camera is currently following the ship, to display on the overlay.
 * @param time Current simulation time to display on the overlay.
 * @param speed_label Current simulation speed label to display on the overlay.
 * @param camera_velocity_magnitude Current camera velocity magnitude to display on the overlay.
 */
void DrawOrbitalHUD(bool paused, bool following_ship, double time, const char *speed_label, float camera_velocity_magnitude);

#endif // ORBITAL_OVERLAY_H
