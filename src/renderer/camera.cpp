/**
 * SPACESIM!
 * Orbital camera module
 *
 * @author The glitched engineers
 */

// For M_PI constant in MSVC++
#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "camera.h"

// Sensitivity constants for movement
#define MOVEMENT_SMOOTHNESS_CONSTANT 0.2f // Time constant for smoothing movement (seconds)

// Sensitivity constants for rotation
#define ROTATION_KEY_SENSITIVITY 30.0f   // Degrees per second for arrow key rotation
#define ROTATION_MOUSE_SENSITIVITY 2.0f  // Degrees per pixel for mouse rotation
#define ROTATION_SMOOTHING_CONSTANT 0.2f // Time constant for smoothing rotation (seconds)

/**
 * @brief Structure to hold the target position and rotation for the orbital camera, which will be smoothly interpolated towards.
 */
typedef struct
{
    Vector3 position;
    Vector2 rotation;
} OrbitalCamera;

static OrbitalCamera orbital_camera;

/**
 * @brief Computes the pitch and yaw angles for the camera to look at a target position from a given camera position.
 * 
 * @param camera_position The current position of the camera in world space.
 * @param camera_target The target position that the camera should look at in world space.
 * 
 * @return A Vector2 where x is the pitch angle (in degrees) and y is the yaw angle (in degrees) for the camera to look at the target.
 */
static Vector2 GetOrbitalRotation(Vector3 camera_position, Vector3 camera_target)
{
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera_target, camera_position));

    float pitch = -asinf(forward.y) * RAD2DEG;
    float yaw = atan2f(forward.x, forward.z) * RAD2DEG;

    return Vector2{pitch, yaw};
}

void SetOrbitalCameraPositionAndTarget(Vector3 camera_position, Vector3 camera_target)
{
    orbital_camera.position = camera_position;
    orbital_camera.rotation = GetOrbitalRotation(camera_position, camera_target);
}

Vector3 GetAbsoluteVelocity(Camera3D *camera, Vector3 relative_velocity)
{
    // Compute the camera's local axes
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera->up));
    Vector3 up = Vector3Normalize(Vector3CrossProduct(right, forward));

    // Compute the absolute velocity in world space
    Vector3 absolute_velocity = {0};

    absolute_velocity = Vector3Add(absolute_velocity, Vector3Scale(forward, relative_velocity.z));
    absolute_velocity = Vector3Add(absolute_velocity, Vector3Scale(right, relative_velocity.x));
    absolute_velocity = Vector3Add(absolute_velocity, Vector3Scale(up, relative_velocity.y));

    return absolute_velocity;
}

Quaternion GetCameraRotation(Camera3D *camera)
{
    Vector2 rotation = GetOrbitalRotation(camera->position, camera->target);
    
    return QuaternionFromEuler(rotation.x * DEG2RAD, rotation.y * DEG2RAD, 0.0f);
}

void SetCameraRotation(Camera3D *camera, Quaternion rotation)
{
    // Compute the forward vector from the rotation
    Vector3 direction = Vector3RotateByQuaternion(Vector3{0.0f, 0.0f, 1.0f}, rotation);

    camera->target = Vector3Add(camera->position, direction);
}

/**
 * @brief Computes the alpha value for an exponential smoothing filter based on the elapsed time and a time constant.
 *
 * @param delta_time Time elapsed since the last frame (in seconds).
 *
 * @param tau The time constant for the smoothing filter (in seconds). A smaller tau results in faster smoothing, while a larger tau results in slower smoothing.
 */
static float GetFilterAlpha(float delta_time, float tau)
{
    float alpha = 1.0f - expf(-delta_time / tau);

    return fmin(alpha, 1.0f);
}

void UpdateOrbitalCamera(Camera3D *camera, float delta_time, Vector3 position_delta, Vector2 rotation_delta)
{
    // Get current camera position and rotation
    Vector3 position = camera->position;
    Quaternion rotation = GetCameraRotation(camera);

    // Update target position and rotation based on input
    orbital_camera.position = Vector3Add(orbital_camera.position, position_delta);
    orbital_camera.rotation = Vector2Add(orbital_camera.rotation, rotation_delta);
    if (orbital_camera.rotation.x < -89.9f)
        orbital_camera.rotation.x = -89.9f;
    else if (orbital_camera.rotation.x > 89.9f)
        orbital_camera.rotation.x = 89.9f;

    // Smooth position
    float movement_alpha = GetFilterAlpha(delta_time, MOVEMENT_SMOOTHNESS_CONSTANT);
    camera->position = Vector3Add(
        Vector3Scale(position, 1.0f - movement_alpha),
        Vector3Scale(orbital_camera.position, movement_alpha));

    // Smooth rotation
    float rotation_alpha = GetFilterAlpha(delta_time, ROTATION_SMOOTHING_CONSTANT);
    rotation = QuaternionSlerp(rotation,
                               QuaternionFromEuler(orbital_camera.rotation.x * DEG2RAD, orbital_camera.rotation.y * DEG2RAD, 0.0f),
                               rotation_alpha);
    SetCameraRotation(camera, rotation);
}
