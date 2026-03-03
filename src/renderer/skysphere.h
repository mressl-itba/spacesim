/**
 * SPACESIM!
 * Skysphere loading and rendering module for raylib
 * 
 * @author The glitched engineers
 */

#ifndef SKYSPHERE_H_
#define SKYSPHERE_H_

#include "raylib.h"

/**
 * @brief Loads a skysphere model with the specified texture.
 * 
 * @param texture_path Path to the texture image to use for the skysphere.
 * 
 * @return A Model struct representing the loaded skysphere.
 */
Model LoadSkysphere(const char *texture_path);

/**
 * @brief Unloads the resources associated with a skysphere model.
 * 
 * @param skybox The Model struct representing the skysphere to unload.
 */
void UnloadSkysphere(Model skybox);

/**
 * @brief Draws the skysphere centered on the camera position.
 * 
 * @param skysphere Pointer to the Model struct representing the skysphere to draw.
 * @param camera_position The position of the camera in world space.
 */
void DrawSkysphere(const Model *skysphere);

#endif // SKYSPHERE_H_
