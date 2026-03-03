/**
 * SPACESIM!
 * Orbital grid rendering module
 *
 * @author The glitched engineers
 */

#include "grid.h"

#include "raylib.h"

void DrawOrbitalGrid(uint32_t n, float spacing)
{
    Color color = Fade(WHITE, 0.2f);

    // Draw central axes
    DrawLine3D(Vector3{-(n * spacing), 0.0f, 0.0f}, Vector3{n * spacing, 0.0f, 0.0f}, color);
    DrawLine3D(Vector3{0.0f, 0.0f, -(n * spacing)}, Vector3{0.0f, 0.0f, n * spacing}, color);

    // Draw concentric circles on the XZ plane
    for (uint32_t i = 1; i <= n; i++)
    {
        float radius = i * spacing;
        DrawCircle3D(Vector3{0.0f, 0.0f, 0.0f}, radius, Vector3{1.0f, 0.0f, 0.0f}, 90.0f, color);
    }
}
