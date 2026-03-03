/**
 * SPACESIM!
 * Skysphere loading and rendering module for raylib
 *
 * @author The glitched engineers
 */

// #include "rlgl.h"

#include "skysphere.h"

#include "rlgl.h"
#include "raymath.h" // Required for: MatrixPerspective(), MatrixLookAt()

#define GLSL_VERSION 330

static TextureCubemap GenTextureCubemap(Shader shader, Texture2D panorama, int size, int format)
{
    TextureCubemap cubemap = {0};

    rlDisableBackfaceCulling(); // Disable backface culling to render inside the cube

    // STEP 1: Setup framebuffer
    //------------------------------------------------------------------------------------------
    unsigned int rbo = rlLoadTextureDepth(size, size, true);
    cubemap.id = rlLoadTextureCubemap(0, size, format, 1);

    unsigned int fbo = rlLoadFramebuffer();
    rlFramebufferAttach(fbo, rbo, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
    rlFramebufferAttach(fbo, cubemap.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_CUBEMAP_POSITIVE_X, 0);

    // Check if framebuffer is complete with attachments (valid)
    if (rlFramebufferComplete(fbo))
        TraceLog(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", fbo);
    //------------------------------------------------------------------------------------------

    // STEP 2: Draw to framebuffer
    //------------------------------------------------------------------------------------------
    // NOTE: Shader is used to convert HDR equirectangular environment map to cubemap equivalent (6 faces)
    rlEnableShader(shader.id);

    // Define projection matrix and send it to shader
    Matrix matFboProjection = MatrixPerspective(90.0 * DEG2RAD, 1.0, rlGetCullDistanceNear(), rlGetCullDistanceFar());
    rlSetUniformMatrix(shader.locs[SHADER_LOC_MATRIX_PROJECTION], matFboProjection);

    // Define view matrix for every side of the cubemap
    Matrix fboViews[6] = {
        MatrixLookAt(Vector3{0.0f, 0.0f, 0.0f}, Vector3{1.0f, 0.0f, 0.0f}, Vector3{0.0f, -1.0f, 0.0f}),
        MatrixLookAt(Vector3{0.0f, 0.0f, 0.0f}, Vector3{-1.0f, 0.0f, 0.0f}, Vector3{0.0f, -1.0f, 0.0f}),
        MatrixLookAt(Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 1.0f, 0.0f}, Vector3{0.0f, 0.0f, 1.0f}),
        MatrixLookAt(Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, -1.0f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f}),
        MatrixLookAt(Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 1.0f}, Vector3{0.0f, -1.0f, 0.0f}),
        MatrixLookAt(Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f}, Vector3{0.0f, -1.0f, 0.0f})};

    rlViewport(0, 0, size, size); // Set viewport to current fbo dimensions

    // Activate and enable texture for drawing to cubemap faces
    rlActiveTextureSlot(0);
    rlEnableTexture(panorama.id);

    for (int i = 0; i < 6; i++)
    {
        // Set the view matrix for the current cube face
        rlSetUniformMatrix(shader.locs[SHADER_LOC_MATRIX_VIEW], fboViews[i]);

        // Select the current cubemap face attachment for the fbo
        // WARNING: This function by default enables->attach->disables fbo!!!
        rlFramebufferAttach(fbo, cubemap.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_CUBEMAP_POSITIVE_X + i, 0);
        rlEnableFramebuffer(fbo);

        // Load and draw a cube, it uses the current enabled texture
        rlClearScreenBuffers();
        rlLoadDrawCube();
    }
    //------------------------------------------------------------------------------------------

    // STEP 3: Unload framebuffer and reset state
    //------------------------------------------------------------------------------------------
    rlDisableShader();        // Unbind shader
    rlDisableTexture();       // Unbind texture
    rlDisableFramebuffer();   // Unbind framebuffer
    rlUnloadFramebuffer(fbo); // Unload framebuffer (and automatically attached depth texture/renderbuffer)

    // Reset viewport dimensions to default
    rlViewport(0, 0, rlGetFramebufferWidth(), rlGetFramebufferHeight());
    rlEnableBackfaceCulling();
    //------------------------------------------------------------------------------------------

    cubemap.width = size;
    cubemap.height = size;
    cubemap.mipmaps = 1;
    cubemap.format = format;

    return cubemap;
}

Model LoadSkysphere(const char *texture_path)
{
    // Load skybox model
    Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model skybox = LoadModelFromMesh(cube);

    // Load skybox shader and set required locations
    skybox.materials[0].shader = LoadShader(TextFormat("resources/shaders/glsl%i/skybox.vs", GLSL_VERSION),
                                            TextFormat("resources/shaders/glsl%i/skybox.fs", GLSL_VERSION));

    int environmentMap[] = {MATERIAL_MAP_CUBEMAP};
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "environmentMap"), environmentMap, SHADER_UNIFORM_INT);
    int doGamma[] = {0};
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "doGamma"), doGamma, SHADER_UNIFORM_INT);
    int vflipped[] = {0};
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "vflipped"), vflipped, SHADER_UNIFORM_INT);

    // Load cubemap shader and setup required shader locations
    Shader shdrCubemap = LoadShader(TextFormat("resources/shaders/glsl%i/cubemap.vs", GLSL_VERSION),
                                    TextFormat("resources/shaders/glsl%i/cubemap.fs", GLSL_VERSION));

    int equirectangularMap[] = {0};
    SetShaderValue(shdrCubemap, GetShaderLocation(shdrCubemap, "equirectangularMap"), equirectangularMap, SHADER_UNIFORM_INT);

    // Load skysphere texture and assign to material
    Texture2D texture = LoadTexture(texture_path);
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = GenTextureCubemap(shdrCubemap, texture, 1024, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    UnloadTexture(texture);

    return skybox;
}

void UnloadSkysphere(Model skybox)
{
    UnloadShader(skybox.materials[0].shader);
    UnloadTexture(skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture);

    UnloadModel(skybox);
}

void DrawSkysphere(const Model *skysphere)
{
    // We are inside the cube, we need to disable backface culling!
    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(*skysphere, Vector3{0, 0, 0}, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();
}
