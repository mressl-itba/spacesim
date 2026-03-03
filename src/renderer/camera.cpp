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

OrbitalCamera *CreateOrbitalCamera(Vector3 position, Vector3 target)
{
    OrbitalCamera *orbital_camera = new OrbitalCamera();

    orbital_camera->camera = Camera3D{
        position,
        target,
        Vector3{0.0f, 1.0f, 0.0f},
        60.0f,
        CAMERA_PERSPECTIVE};
    orbital_camera->movement = Vector3{0.0f, 0.0f, 0.0f};
    orbital_camera->rotation = Vector3{0.0f, 0.0f, 0.0f};

    return orbital_camera;
}

void DestroyOrbitalCamera(OrbitalCamera *orbital_camera)
{
    delete orbital_camera;
}

void SetOrbitalCameraPosition(OrbitalCamera *orbital_camera, Vector3 position, Vector3 target)
{
    orbital_camera->camera.position = position;
    orbital_camera->camera.target = target;
}

static void UpdateFreeCamera(Camera *camera, Vector3 movement, Vector3 rotation, float zoom)
{
    // Compute current forward direction (full 3D, not flattened)
    Vector3 forward = Vector3Normalize(
        Vector3Subtract(camera->target, camera->position));

    // Compute right vector from forward and up
    Vector3 right = Vector3Normalize(
        Vector3CrossProduct(forward, camera->up));

    // Recompute orthonormal up (in case it drifted)
    Vector3 up = Vector3Normalize(
        Vector3CrossProduct(right, forward));

    // Apply movement in full 3D view space
    Vector3 moveDelta = {0};

    moveDelta = Vector3Add(moveDelta, Vector3Scale(forward, movement.z));
    moveDelta = Vector3Add(moveDelta, Vector3Scale(right, movement.x));
    moveDelta = Vector3Add(moveDelta, Vector3Scale(up, movement.y));

    camera->position = Vector3Add(camera->position, moveDelta);
    camera->target = Vector3Add(camera->target, moveDelta);

    // Apply rotation (yaw + pitch style)
    if (rotation.x != 0 || rotation.y != 0 || rotation.z != 0)
    {
        Vector3 view = Vector3Subtract(camera->target, camera->position);

        Matrix rot = MatrixMultiply(
            MatrixRotate(up, rotation.y * DEG2RAD),
            MatrixRotate(right, rotation.x * DEG2RAD));

        view = Vector3Transform(view, rot);

        camera->target = Vector3Add(camera->position, view);
    }

    // Apply zoom (move along forward vector)
    if (zoom != 0.0f)
    {
        Vector3 zoomDelta = Vector3Scale(forward, zoom);
        camera->position = Vector3Add(camera->position, zoomDelta);
    }
}

float GetFilterAlpha(float delta_time, float tau)
{
    float alpha = 1.0f - expf(-delta_time / tau);

    return fmin(alpha, 1.0f);
}

void UpdateOrbitalCamera(OrbitalCamera *orbital_camera, float delta_time, float render_velocity)
{
    // Movement with WASD and QE
    float movement_delta = render_velocity * delta_time;
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
        movement_delta *= 2.0f; // Speed boost with Shift key
    if (IsKeyDown(KEY_W))
        orbital_camera->movement.z += movement_delta;
    else if (IsKeyDown(KEY_S))
        orbital_camera->movement.z -= movement_delta;
    else if (IsKeyDown(KEY_A))
        orbital_camera->movement.x -= movement_delta;
    else if (IsKeyDown(KEY_D))
        orbital_camera->movement.x += movement_delta;
    else if (IsKeyDown(KEY_Q))
        orbital_camera->movement.y -= movement_delta;
    else if (IsKeyDown(KEY_E))
        orbital_camera->movement.y += movement_delta;

    // Rotation with arrow keys
    float key_rotation_speed = ROTATION_KEY_SENSITIVITY * delta_time;
    if (IsKeyDown(KEY_UP))
        orbital_camera->rotation.x += key_rotation_speed;
    else if (IsKeyDown(KEY_DOWN))
        orbital_camera->rotation.x -= key_rotation_speed;
    else if (IsKeyDown(KEY_LEFT))
        orbital_camera->rotation.y += key_rotation_speed;
    else if (IsKeyDown(KEY_RIGHT))
        orbital_camera->rotation.y -= key_rotation_speed;

    // Rotation with mouse movement
    Vector2 mouse_delta = GetMouseDelta();
    float mouse_rotation_speed = ROTATION_MOUSE_SENSITIVITY * delta_time;
    orbital_camera->rotation.x += -mouse_delta.y * mouse_rotation_speed;
    orbital_camera->rotation.y += -mouse_delta.x * mouse_rotation_speed;

    // Smooth movement
    float alpha_movement = GetFilterAlpha(delta_time, MOVEMENT_SMOOTHNESS_CONSTANT);
    Vector3 smooth_movement = Vector3Scale(orbital_camera->movement, alpha_movement);
    orbital_camera->movement = Vector3Subtract(orbital_camera->movement, smooth_movement);

    // Smooth velocity
    float alpha_rotation = GetFilterAlpha(delta_time, ROTATION_SMOOTHING_CONSTANT);
    Vector3 smooth_rotation = Vector3Scale(orbital_camera->rotation, alpha_rotation);
    orbital_camera->rotation = Vector3Subtract(orbital_camera->rotation, smooth_rotation);

    // Update camera position
    UpdateFreeCamera(&orbital_camera->camera, smooth_movement, smooth_rotation, 0.0f);
}
