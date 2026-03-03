/**
 * SPACESIM!
 * Orbital body rendering module
 *
 * @author The glitched engineers
 */

#include "body.h"
#include "renderer.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include <cmath>
#include <cstdlib>

#define SPHERE_RINGS 64
#define SPHERE_SLICES 64

#define MINIMUM_PIXEL_RADIUS 2.0f

#define MASSIVE_BODIES_COUNT 10

#define TRAIL_LENGTH 500 // Number of trail points per body

static const char *massive_bodies_textures[MASSIVE_BODIES_COUNT] = {
    "resources/textures/2k_sun.png",
    "resources/textures/2k_mercury.png",
    "resources/textures/2k_venus.png",
    "resources/textures/2k_earth.png",
    "resources/textures/2k_mars.png",
    "resources/textures/2k_jupiter.png",
    "resources/textures/2k_saturn.png",
    "resources/textures/2k_uranus.png",
    "resources/textures/2k_neptune.png",
    "resources/textures/2k_moon.png",
};

#if defined(MATERIAL_MAP_ALBEDO)
#define ORBITAL_BODY_TEXTURE_MAP MATERIAL_MAP_ALBEDO
#else
#define ORBITAL_BODY_TEXTURE_MAP MATERIAL_MAP_DIFFUSE
#endif

typedef struct
{
    Model unit_sphere_model;
    Texture2D massive_body_textures[MASSIVE_BODIES_COUNT];
    Texture2D point_sprite;

    // Trail system
    Vector3 trails[MASSIVE_BODIES_COUNT][TRAIL_LENGTH];
    int trail_index[MASSIVE_BODIES_COUNT];
    int trail_count[MASSIVE_BODIES_COUNT];
} OrbitalBodyRenderCache;

static OrbitalBodyRenderCache body_render_cache = {0};

static Mesh GenMeshSphereEquirectangular(float radius, int rings, int slices)
{
    Mesh mesh = {0};

    int vertexCount = (rings + 1) * (slices + 1);
    int triangleCount = rings * slices * 2;

    mesh.vertexCount = vertexCount;
    mesh.triangleCount = triangleCount;

    mesh.vertices = (float *)RL_MALLOC(vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float *)RL_MALLOC(vertexCount * 2 * sizeof(float));
    mesh.normals = (float *)RL_MALLOC(vertexCount * 3 * sizeof(float));
    mesh.indices = (unsigned short *)RL_MALLOC(triangleCount * 3 * sizeof(unsigned short));

    int vertexIndex = 0;
    int texcoordIndex = 0;
    int normalIndex = 0;

    // Generate vertices, normals, and texcoords
    for (int ring = 0; ring <= rings; ring++)
    {
        float v = (float)ring / (float)rings; // 0 to 1, bottom to top
        float phi = v * PI;                   // 0 to PI, south pole to north pole

        for (int slice = 0; slice <= slices; slice++)
        {
            float u = (float)slice / (float)slices; // 0 to 1, around the sphere
            float theta = u * 2.0f * PI;            // 0 to 2*PI, around the sphere

            // Spherical to Cartesian coordinates
            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            mesh.vertices[vertexIndex++] = x;
            mesh.vertices[vertexIndex++] = y;
            mesh.vertices[vertexIndex++] = z;

            // Normals (pointing outward)
            Vector3 normal = Vector3Normalize(Vector3{x, y, z});
            mesh.normals[normalIndex++] = normal.x;
            mesh.normals[normalIndex++] = normal.y;
            mesh.normals[normalIndex++] = normal.z;

            // Texture coordinates for equirectangular mapping
            mesh.texcoords[texcoordIndex++] = 1.0f - u;
            mesh.texcoords[texcoordIndex++] = v;
        }
    }

    // Generate indices (counter-clockwise from outside)
    int indexCount = 0;
    for (int ring = 0; ring < rings; ring++)
    {
        for (int slice = 0; slice < slices; slice++)
        {
            int current = ring * (slices + 1) + slice;
            int next = current + slices + 1;

            // First triangle (CCW from outside)
            mesh.indices[indexCount++] = current;
            mesh.indices[indexCount++] = current + 1;
            mesh.indices[indexCount++] = next;

            // Second triangle (CCW from outside)
            mesh.indices[indexCount++] = current + 1;
            mesh.indices[indexCount++] = next + 1;
            mesh.indices[indexCount++] = next;
        }
    }

    UploadMesh(&mesh, false);

    return mesh;
}

static Color ComputeFallbackBodyColor(float mass)
{
    float solar_mass = 1.989e30f; // Solar mass for scaling

    // Simple mass-based coloring: low mass → cyan, high mass → orange/red
    float normalized_mass = (solar_mass > 0.0f) ? mass / solar_mass : 0.5f;
    normalized_mass = Clamp(normalized_mass, 0.0f, 1.0f);

    unsigned char red = (unsigned char)(255 * normalized_mass);
    unsigned char green = (unsigned char)(180 * (1.0f - normalized_mass * 0.4f));
    unsigned char blue = (unsigned char)(220 * (1.0f - normalized_mass));

    return Color{red, green, blue, 255};
}

void ConstructOrbitalBodyRenderer()
{
    body_render_cache.unit_sphere_model = LoadModelFromMesh(GenMeshSphereEquirectangular(1.0f, SPHERE_RINGS, SPHERE_SLICES));

    // Load and assign textures to model material
    for (uint32_t i = 0; i < MASSIVE_BODIES_COUNT; i++)
        body_render_cache.massive_body_textures[i] = LoadTexture(massive_bodies_textures[i]);

    // Create 1x1 white pixel sprite for small bodies
    Image sprite_img = GenImageColor(1, 1, WHITE);
    body_render_cache.point_sprite = LoadTextureFromImage(sprite_img);
    UnloadImage(sprite_img);

    // Initialize trail buffers
    for (uint32_t i = 0; i < MASSIVE_BODIES_COUNT; i++)
    {
        body_render_cache.trail_index[i] = 0;
        body_render_cache.trail_count[i] = 0;
    }
}

void DestroyOrbitalBodyRenderer()
{
    for (uint32_t i = 0; i < MASSIVE_BODIES_COUNT; i++)
        UnloadTexture(body_render_cache.massive_body_textures[i]);

    UnloadTexture(body_render_cache.point_sprite);
    UnloadModel(body_render_cache.unit_sphere_model);
}

void DrawOrbitalBody(Camera3D *camera, const OrbitalSim *sim, uint32_t body_index)
{
    const Body *body = &sim->bodies[body_index];

    // Convert to render coordinates
    Vector3 render_pos = {
        body->position[0] * RENDER_SCALE,
        body->position[2] * RENDER_SCALE,
        body->position[1] * RENDER_SCALE};

    if (body_index < MASSIVE_BODIES_COUNT)
    {
        // Update trail with current position
        int trail_idx = body_render_cache.trail_index[body_index];
        body_render_cache.trails[body_index][trail_idx] = render_pos;
        body_render_cache.trail_index[body_index] = (trail_idx + 1) % TRAIL_LENGTH;
        if (body_render_cache.trail_count[body_index] < TRAIL_LENGTH)
            body_render_cache.trail_count[body_index]++;

        // Draw trail
        int trail_count = body_render_cache.trail_count[body_index];
        if (trail_count > 1)
        {
            for (int i = 0; i < trail_count - 1; i++)
            {
                int idx1 = (body_render_cache.trail_index[body_index] - trail_count + i + TRAIL_LENGTH) % TRAIL_LENGTH;
                int idx2 = (idx1 + 1) % TRAIL_LENGTH;

                // Fade trail from transparent to more opaque
                float alpha = (float)(i + 1) / (float)trail_count;
                Color trail_color = {150, 150, 200, (unsigned char)(alpha * 100)};

                DrawLine3D(
                    body_render_cache.trails[body_index][idx1],
                    body_render_cache.trails[body_index][idx2],
                    trail_color);
            }
        }

        float render_radius = body->mean_radius * RENDER_SCALE;

        // Ensure minimum visible size for distant bodies
        float cam_distance = Vector3Distance(camera->position, render_pos);
        if (cam_distance > 0.0f)
        {
            float pixel_factor = (0.5f * WINDOW_WIDTH / cam_distance);
            float pixel_radius = render_radius * pixel_factor;
            if (pixel_radius < MINIMUM_PIXEL_RADIUS)
                render_radius = MINIMUM_PIXEL_RADIUS / pixel_factor;
        }

        // Assign texture for this body
        body_render_cache.unit_sphere_model.materials[0].maps[ORBITAL_BODY_TEXTURE_MAP].texture =
            body_render_cache.massive_body_textures[body_index];

        // Render sphere with one-sided culling
        DrawModelEx(
            body_render_cache.unit_sphere_model,
            render_pos,
            Vector3{0.0f, 1.0f, 0.0f},
            0.0f,
            Vector3{render_radius, render_radius, render_radius},
            WHITE);

        return;
    }

    // Draw small bodies as 1x1 pixel billboard sprite with alpha blending
    // Calculate world-space size to ensure minimum screen-space size
    float cam_distance = Vector3Distance(camera->position, render_pos);
    float pixel_factor = (0.25f * WINDOW_WIDTH / cam_distance);
    float billboard_size = (cam_distance > 0.0f)
                               ? MINIMUM_PIXEL_RADIUS / pixel_factor
                               : 1.0f;

    // WHITE at 50% opacity
    Color sprite_color = {255, 255, 255, 127};
    DrawBillboard(*camera, body_render_cache.point_sprite, render_pos, billboard_size, sprite_color);
}
