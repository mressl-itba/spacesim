/**
 * SPACESIM!
 * Skysphere module for raylib
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
 */
void LoadSkysphere(const char *texture_path);

/**
 * @brief Unloads the resources associated with a skysphere model.
 */
void UnloadSkysphere();

/**
 * @brief Draws the skysphere centered on the camera position.
 */
void DrawSkysphere(Vector3 camera_position);

#endif // SKYSPHERE_H_
