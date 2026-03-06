/**
 * SPACESIM!
 * Orbital rendering module
 *
 * @author The glitched engineers
 */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "body.h"
#include "camera.h"
#include "grid.h"
#include "hud.h"
#include "renderer.h"
#include "skysphere.h"

// Target frames per second for rendering
#define TARGET_FPS 60

// Render parameters
#define SKYSPHERE_TEXTURE_PATH "resources/textures/2k_stars_milky_way.png"

// Initial camera parameters
#define INITIAL_CAMERA_POSITION (Vector3{0.0f, GRID_SPACING * RENDER_SCALE, GRID_SPACING * RENDER_SCALE})
#define INITIAL_CAMERA_TARGET (Vector3{0.0f, 0.0f, 0.0f})

// Camera velocity parameters
#define CAMERA_VELOCITY_INITIAL 1e11f
#define CAMERA_VELOCITY_MIN 1e6f
#define CAMERA_VELOCITY_MAX 1e12f
#define CAMERA_SPEED_BOOST 3.0f
#define CAMERA_VELOCITY_WHEEL_SENSITIVITY 0.25f

// Sensitivity constants for rotation
#define ROTATION_KEY_SENSITIVITY 30.0f  // Degrees per second for arrow key rotation
#define ROTATION_MOUSE_SENSITIVITY 2.0f // Degrees per pixel for mouse rotation

typedef struct
{
    OrbitalSim *sim; // Pointer to the underlying simulation data

    uint32_t time_speed_index; // Index for controlling simulation speed
    bool paused;               // Whether the simulation is currently paused

    bool following_ship;            // Whether the camera is currently following the ship (first body in the simulation)
    Vector3 previous_ship_position; // Last known position of the ship for smooth following

    Camera3D camera;                 // The camera used for rendering the scene
    float camera_velocity_magnitude; // The base magnitude of the velocity for camera movement
} OrbitalRenderer;

static OrbitalRenderer orbital_renderer;

/**
 * @brief Initializes the orbital renderer, including setting up the window, camera, and loading resources.
 *
 * @param sim Pointer to the OrbitalSim structure containing the simulation data to be rendered, it is the caller's responsibility to manage the memory of the simulation and ensure it remains valid for the duration of the rendering.
 */
void LoadOrbitalRenderer(OrbitalSim *sim)
{
    // Initialize window and OpenGL context
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(TARGET_FPS);

    // Hide cursor for better camera control
    DisableCursor();

    // Initialize orbital visualization state
    orbital_renderer.sim = sim;

    orbital_renderer.time_speed_index = 0;
    orbital_renderer.paused = false;

    orbital_renderer.following_ship = false;
    orbital_renderer.previous_ship_position = GetOrbitalBodyPosition(sim, 0);

    orbital_renderer.camera = Camera3D{
        INITIAL_CAMERA_POSITION,
        INITIAL_CAMERA_TARGET,
        Vector3{0.0f, 1.0f, 0.0f},
        60.0f,
        CAMERA_PERSPECTIVE};
    orbital_renderer.camera_velocity_magnitude = CAMERA_VELOCITY_INITIAL;
    SetOrbitalCameraPositionAndTarget(orbital_renderer.camera.position, orbital_renderer.camera.target);

    LoadSkysphere(SKYSPHERE_TEXTURE_PATH);

    LoadOrbitalBodyRenderer();
}

/**
 * @brief Updates the orbital renderer's state based on user input and elapsed time, including camera movement, rotation, and simulation speed control.
 */
static void UnloadOrbitalRenderer()
{
    UnloadOrbitalBodyRenderer();
    UnloadSkysphere();

    CloseWindow();
}

#define SECOND 1
#define MINUTE (60 * SECOND)
#define HOUR (60 * MINUTE)
#define DAY (24 * HOUR)

static const uint32_t time_speed_indices[] = {
    1 * SECOND, 2 * SECOND, 5 * SECOND, 10 * SECOND, 30 * SECOND,
    1 * MINUTE, 2 * MINUTE, 5 * MINUTE, 10 * MINUTE, 30 * MINUTE,
    1 * HOUR, 3 * HOUR, 6 * HOUR, 12 * HOUR,
    DAY, 2 * DAY, 5 * DAY, 10 * DAY, 30 * DAY};

static const char *time_speed_labels[] = {
    "1 s/s", "2 s/s", "5 s/s", "10 s/s", "30 s/s",
    "1 min/s", "2 min/s", "5 min/s", "10 min/s", "30 min/s",
    "1 h/s", "3 h/s", "6 h/s", "12 h/s",
    "1 d/s", "2 d/s", "5 d/s", "10 d/s", "30 d/s"};

#define TIME_SPEED_INDICES_COUNT (sizeof(time_speed_indices) / sizeof(time_speed_indices[0]))

/**
 * @brief Updates the orbital renderer's state based on user input and elapsed time, including camera movement, rotation, and simulation speed control.
 *
 * @param on_frame A callback function that takes a pointer to the OrbitalSim and a delta time in seconds, it is called every frame to update the simulation state, it is the caller's responsibility to implement the function and update the simulation accordingly.
 */
static void UpdateOrbitalRenderer(OnFrame on_frame)
{
    // Update parameters
    float delta_time = GetFrameTime();
    Vector3 camera_velocity = {0.0f, 0.0f, 0.0f};
    Vector2 camera_angular_velocity = {0.0f, 0.0f};

    // Velocity magnitude control with mouse wheel/IM keys
    float velocity_magnitude_control = GetMouseWheelMove();
    if (IsKeyPressed(KEY_I))
        velocity_magnitude_control += 1.0f;
    if (IsKeyPressed(KEY_M))
        velocity_magnitude_control -= 1.0f;
    orbital_renderer.camera_velocity_magnitude *= powf(10.0f, velocity_magnitude_control * CAMERA_VELOCITY_WHEEL_SENSITIVITY);
    if (orbital_renderer.camera_velocity_magnitude < CAMERA_VELOCITY_MIN)
        orbital_renderer.camera_velocity_magnitude = CAMERA_VELOCITY_MIN;
    else if (orbital_renderer.camera_velocity_magnitude > CAMERA_VELOCITY_MAX)
        orbital_renderer.camera_velocity_magnitude = CAMERA_VELOCITY_MAX;

    // Velocity control with WASD and QE keys, Shift for speed boost
    float key_velocity_magnitude = orbital_renderer.camera_velocity_magnitude * RENDER_SCALE;
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
        key_velocity_magnitude *= CAMERA_SPEED_BOOST; // Speed boost with Shift key
    if (IsKeyDown(KEY_W))
        camera_velocity.z += key_velocity_magnitude;
    if (IsKeyDown(KEY_S))
        camera_velocity.z -= key_velocity_magnitude;
    if (IsKeyDown(KEY_D))
        camera_velocity.x += key_velocity_magnitude;
    if (IsKeyDown(KEY_A))
        camera_velocity.x -= key_velocity_magnitude;
    if (IsKeyDown(KEY_E))
        camera_velocity.y += key_velocity_magnitude;
    if (IsKeyDown(KEY_Q))
        camera_velocity.y -= key_velocity_magnitude;

    // Rotation with arrow keys/mouse movement
    if (IsKeyDown(KEY_UP))
        camera_angular_velocity.x -= ROTATION_KEY_SENSITIVITY;
    if (IsKeyDown(KEY_DOWN))
        camera_angular_velocity.x += ROTATION_KEY_SENSITIVITY;
    if (IsKeyDown(KEY_RIGHT))
        camera_angular_velocity.y -= ROTATION_KEY_SENSITIVITY;
    if (IsKeyDown(KEY_LEFT))
        camera_angular_velocity.y += ROTATION_KEY_SENSITIVITY;
    Vector2 mouse_delta = GetMouseDelta();
    camera_angular_velocity.x += mouse_delta.y * ROTATION_MOUSE_SENSITIVITY;
    camera_angular_velocity.y += -mouse_delta.x * ROTATION_MOUSE_SENSITIVITY;

    // Time speed control with J/K keys
    if (IsKeyPressed(KEY_J))
        if (orbital_renderer.time_speed_index > 0)
            orbital_renderer.time_speed_index--;
    if (IsKeyPressed(KEY_K))
        if (orbital_renderer.time_speed_index < (TIME_SPEED_INDICES_COUNT - 1))
            orbital_renderer.time_speed_index++;

    // Follow ship toggle with F key
    if (IsKeyPressed(KEY_F))
    {
        orbital_renderer.following_ship = !orbital_renderer.following_ship;

        if (orbital_renderer.following_ship)
        {
            Vector3 camera_target = GetOrbitalBodyPosition(orbital_renderer.sim, 0);
            Vector3 camera_position = Vector3Add(camera_target,
                                                 Vector3{0.0f, 2e7 * RENDER_SCALE, 2e7 * RENDER_SCALE});
            SetOrbitalCameraPositionAndTarget(camera_position, camera_target);
        }
    }

    // Toggle fullscreen with F11 key
    if (IsKeyPressed(KEY_F11))
        ToggleFullscreen();

    // Pause/resume with SPACE key
    if (IsKeyPressed(KEY_SPACE))
        orbital_renderer.paused = !orbital_renderer.paused;

    // Update simulation state if not paused or if SPACE was just pressed to unpause
    if (!orbital_renderer.paused || IsKeyPressed(KEY_ENTER))
    {
        float dt = delta_time * time_speed_indices[orbital_renderer.time_speed_index];

        on_frame(orbital_renderer.sim, dt);
    }

    // Update camera position
    Vector3 velocity = GetAbsoluteVelocity(&orbital_renderer.camera, camera_velocity);
    Vector3 position_delta = Vector3Scale(velocity, delta_time);

    Vector2 rotation_delta = Vector2Scale(camera_angular_velocity, delta_time);

    Vector3 ship_position = GetOrbitalBodyPosition(orbital_renderer.sim, 0);
    Vector3 ship_position_delta = Vector3Subtract(ship_position, orbital_renderer.previous_ship_position);
    orbital_renderer.previous_ship_position = ship_position;
    if (orbital_renderer.following_ship)
        position_delta = Vector3Add(position_delta, ship_position_delta);

    UpdateOrbitalCamera(&orbital_renderer.camera, GetFrameTime(), position_delta, rotation_delta);
}

/**
 * @brief Draws the orbital simulation scene, including the skysphere, orbital grid, and orbital bodies, as well as the 2D HUD.
 */
static void DrawOrbitalRenderer()
{
    // Start drawing
    BeginDrawing();

    ClearBackground(BLACK);

    // 3D scene
    BeginMode3D(orbital_renderer.camera);

    DrawSkysphere();

    DrawOrbitalGrid(50, GRID_SPACING * RENDER_SCALE);

    OrbitalSim *sim = orbital_renderer.sim;
    for (uint32_t i = 0; i < sim->num_bodies; i++)
        DrawOrbitalBody(&orbital_renderer.camera, sim, i);

    EndMode3D();

    // 2D HUD
    DrawOrbitalHUD(orbital_renderer.paused,
                   orbital_renderer.following_ship,
                   orbital_renderer.sim->time,
                   time_speed_labels[orbital_renderer.time_speed_index],
                   orbital_renderer.camera_velocity_magnitude);

    EndDrawing();
}

void RenderOrbitalSim(OrbitalSim *sim, OnFrame on_frame)
{
    LoadOrbitalRenderer(sim);

    while (!WindowShouldClose())
    {
        UpdateOrbitalRenderer(on_frame);

        DrawOrbitalRenderer();
    }

    UnloadOrbitalRenderer();
}
