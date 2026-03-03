/**
 * SPACESIM!
 * Orbital camera module
 *
 * @author The glitched engineers
 */

#ifndef ORBITAL_CAMERA_H_
#define ORBITAL_CAMERA_H_

#include <cstdint>

#include "raylib.h"

/**
 * @brief Structure representing an orbital camera, which allows for smooth movement and zooming around a target point in 3D space.
 */
struct OrbitalCamera
{
    Camera3D camera; // The underlying Camera3D structure from raylib

    Vector3 movement; // Movement velocity along forward, right, and up axes for smooth movement

    Vector3 rotation; // Rotation angles around right (pitch), up (yaw), and forward (roll) axes for smooth rotation
};

/**
 * @brief Creates and initializes a Camera3D structure for orbital visualization.
 * 
 * @param position The initial position of the camera in 3D space.
 * @param target The initial target point that the camera looks at in 3D space.
 * 
 * @return Pointer to the newly created OrbitalCamera structure.
 */
OrbitalCamera *CreateOrbitalCamera(Vector3 position, Vector3 target);

/**
 * @brief Destroys and cleans up any resources associated with the orbital camera.
 * 
 * @param camera Pointer to the OrbitalCamera structure to destroy.
 */
void DestroyOrbitalCamera(OrbitalCamera *camera);

/**
 * @brief Updates the orbital camera's position and orientation based on user input and elapsed time.
 *
 * @param camera Pointer to the OrbitalCamera structure to update.
 * @param delta_time Time elapsed since the last frame (in seconds), used for smooth movement and rotation.
 * @param velocity The base velocity for camera movement, which can be modified by user input (e.g., holding Shift for speed boost).
 */
void UpdateOrbitalCamera(OrbitalCamera *camera, float delta_time, float velocity);

#endif // ORBITAL_CAMERA_H_
