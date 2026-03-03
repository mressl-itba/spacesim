/**
 * SPACESIM!
 * Orbital HUD rendering module
 *
 * @author The glitched engineers
 */

#include <ctime>

#include "raylib.h"

#include "hud.h"

#define FONT_SIZE 20

void DrawOrbitalHUD(bool paused, double time, const char *speed_label, const char *velocity_label)
{
    // Get effective simulation time for display
    time_t sim_time = (time_t)time;
    struct tm *tm_info = localtime(&sim_time);
    uint32_t year = tm_info->tm_year + 1900;
    uint32_t month = tm_info->tm_mon + 1;
    uint32_t day = tm_info->tm_mday;
    uint32_t hour = tm_info->tm_hour;
    uint32_t minute = tm_info->tm_min;
    uint32_t second = tm_info->tm_sec;

    // Get activity color
    Color activity_color = paused ? RED : GREEN;

    // Metrics
    int text_top = 10;
    int text_bottom = GetScreenHeight() - 10;

    // Keyboard controls
    DrawText("WASDQE: navigate", 10, text_top + FONT_SIZE * 0, FONT_SIZE, WHITE);
    DrawText("JK: time speed", 10, text_top + FONT_SIZE * 1, FONT_SIZE, WHITE);
    DrawText("SPACE: pause/resume", 10, text_top + FONT_SIZE * 2, FONT_SIZE, WHITE);
    DrawText("ENTER: single step (when paused)", 10, text_top + FONT_SIZE * 3, FONT_SIZE, WHITE);
    DrawText("WHEEL: camera velocity", 10, text_top + FONT_SIZE * 4, FONT_SIZE, WHITE);
    DrawText("F: toggle ship follow", 10, text_top + FONT_SIZE * 5, FONT_SIZE, WHITE);
    DrawText("F11: toggle fullscreen", 10, text_top + FONT_SIZE * 6, FONT_SIZE, WHITE);

    // Status information
    DrawText(paused ? "PAUSED" : "RUNNING",
             10, text_bottom - FONT_SIZE * 5, FONT_SIZE, activity_color);
    DrawText(TextFormat("Time: %04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second),
             10, text_bottom - FONT_SIZE * 4, FONT_SIZE, activity_color);
    DrawText(TextFormat("Time speed: %s", speed_label),
             10, text_bottom - FONT_SIZE * 3, FONT_SIZE, activity_color);
    DrawText(TextFormat("Velocity: %s", velocity_label),
             10, text_bottom - FONT_SIZE * 2, FONT_SIZE, activity_color);
    DrawText(TextFormat("FPS: %d", GetFPS()),
             10, text_bottom - FONT_SIZE * 1, FONT_SIZE, activity_color);
}
