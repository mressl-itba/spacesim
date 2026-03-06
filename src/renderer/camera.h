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
 * @brief Sets the camera's position and target.
 * 
 * @param camera_position The desired position of the camera in world space.
 * @param camera_target The desired target point that the camera should look at in world space.
 */
void SetOrbitalCameraPositionAndTarget(Vector3 camera_position, Vector3 camera_target);

/**
 * @brief Converts a relative velocity vector in the camera's local space to an absolute velocity vector in world space.
 * 
 * @param camera Pointer to the Camera3D structure representing the camera.
 * @param relative_velocity A Vector3 representing the velocity in the camera's local space, where x is right, y is up, and z is forward.

 * @return A Vector3 representing the absolute velocity in world space.
 */
Vector3 GetAbsoluteVelocity(Camera3D *camera, Vector3 relative_velocity);

/**
 * @brief Updates the orbital camera's position and orientation based on user input and elapsed time.
 *
 * @param camera Pointer to the Camera3D structure representing the camera to update.
 * @param delta_time Time elapsed since the last frame (in seconds), used for smooth movement and rotation.
 * @param position_delta The desired change in position for the camera, which will be smoothed over time.
 * @param rotation_delta The desired change in rotation (pitch and yaw) for the camera, which will be smoothed over time.
 */
void UpdateOrbitalCamera(Camera3D *camera, float delta_time, Vector3 position_delta, Vector2 rotation_delta);

#endif // ORBITAL_CAMERA_H_
