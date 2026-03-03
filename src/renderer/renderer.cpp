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

// Camera parameters
#define INITIAL_CAMERA_POSITION (Vector3{0.0f, GRID_SPACING * RENDER_SCALE, GRID_SPACING * RENDER_SCALE})
#define INITIAL_CAMERA_TARGET (Vector3{0.0f, 0.0f, 0.0f})

#define CAMERA_VELOCITY_INITIAL 1e11f
#define CAMERA_VELOCITY_MIN 1e6f
#define CAMERA_VELOCITY_MAX 1e12f
#define CAMERA_VELOCITY_WHEEL_SENSITIVITY 0.25f

typedef struct
{
    OrbitalSim *sim; // Pointer to the underlying simulation data

    uint32_t time_speed_index; // Index for controlling simulation speed
    bool paused;               // Whether the simulation is currently paused

    float camera_velocity; // Velocity for smooth camera movement

    OrbitalCamera *orbital_camera; // The orbital camera for navigating the 3D scene
    Model skysphere;               // Skysphere
} OrbitalVis;

static OrbitalVis *CreateOrbitalVis(OrbitalSim *sim)
{
    // Initialize window and OpenGL context
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(TARGET_FPS);

    // Hide cursor for better camera control
    DisableCursor();

    // Initialize orbital visualization state
    OrbitalVis *vis = new OrbitalVis();

    vis->sim = sim;

    vis->time_speed_index = 0;
    vis->paused = false;

    vis->camera_velocity = CAMERA_VELOCITY_INITIAL;

    vis->orbital_camera = CreateOrbitalCamera(INITIAL_CAMERA_POSITION, INITIAL_CAMERA_TARGET);
    vis->skysphere = LoadSkysphere(SKYSPHERE_TEXTURE_PATH);
    ConstructOrbitalBodyRenderer();

    return vis;
}

static void DestroyOrbitalVis(OrbitalVis *vis)
{
    DestroyOrbitalBodyRenderer();
    UnloadSkysphere(vis->skysphere);

    CloseWindow();

    DestroyOrbitalCamera(vis->orbital_camera);

    delete vis;
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

static void DrawOrbitalVis(OrbitalVis *vis)
{
    float render_velocity = vis->camera_velocity * RENDER_SCALE;
    
    UpdateOrbitalCamera(vis->orbital_camera, GetFrameTime(), render_velocity);

    BeginDrawing();

    ClearBackground(BLACK);

    // 3D scene
    BeginMode3D(vis->orbital_camera->camera);

    DrawSkysphere(&vis->skysphere);

    DrawOrbitalGrid(50, GRID_SPACING * RENDER_SCALE);

    OrbitalSim *sim = vis->sim;
    Vector3 camera_position = vis->orbital_camera->camera.position;
    for (uint32_t i = 0; i < sim->num_bodies; i++)
        DrawOrbitalBody(&vis->orbital_camera->camera, sim, i);

    EndMode3D();

    // 2D HUD
    DrawOrbitalHUD(vis->paused,
                   vis->sim->time,
                   time_speed_labels[vis->time_speed_index],
                   TextFormat("%.2e m/s", vis->camera_velocity));

    EndDrawing();
}

static void UpdateOrbitalVis(OrbitalVis *vis, OnFrame on_frame)
{
    // Toggle fullscreen with F11 key
    if (IsKeyPressed(KEY_F11))
        ToggleFullscreen();

    // Pause/resume with P key
    if (IsKeyPressed(KEY_SPACE))
        vis->paused = !vis->paused;

    // Time speed control with J/K keys
    if (IsKeyPressed(KEY_K) && vis->time_speed_index < (TIME_SPEED_INDICES_COUNT - 1))
        vis->time_speed_index++;
    else if (IsKeyPressed(KEY_J) && vis->time_speed_index > 0)
        vis->time_speed_index--;

    // Single step with RETURN / ENTER when paused
    bool do_step = !vis->paused || IsKeyPressed(KEY_ENTER);
    if (do_step)
    {
        float dt = GetFrameTime() * time_speed_indices[vis->time_speed_index];

        on_frame(vis->sim, dt);
    }

    // Camera velocity control with mouse wheel
    float wheel = GetMouseWheelMove();
    float factor = powf(10.0f, wheel * CAMERA_VELOCITY_WHEEL_SENSITIVITY);
    vis->camera_velocity *= factor;
    if (vis->camera_velocity < CAMERA_VELOCITY_MIN)
        vis->camera_velocity = CAMERA_VELOCITY_MIN;
    else if (vis->camera_velocity > CAMERA_VELOCITY_MAX)
        vis->camera_velocity = CAMERA_VELOCITY_MAX;
}

void RenderOrbitalSim(OrbitalSim *sim, OnFrame on_frame)
{
    OrbitalVis *vis = CreateOrbitalVis(sim);

    while (!WindowShouldClose())
    {
        DrawOrbitalVis(vis);

        UpdateOrbitalVis(vis, on_frame);
    }

    DestroyOrbitalVis(vis);
}
