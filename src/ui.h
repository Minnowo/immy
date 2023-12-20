


#ifndef DOKO_UI_H
#define DOKO_UI_H

#include <raylib.h>
#include <stddef.h>

#include "doko.h"


/**
 * Initializes and creates the window/UI components. Should always be called first.
 */
void ui_init();

/**
 * Deinitializes and destroys all the window/UI components. Should always be called last.
 */
void ui_deinit();

/**
 * Renders the background tile.
 */
void ui_renderBackground();

/**
 * Creates a background tile texture.
 */
Texture2D ui_loadBackgroundTile(size_t w, size_t h, Color a, Color b);

/**
 * Renders the image.
 */
void ui_renderImage(doko_image_t* image);

/**
 * Renders the info bar.
 */
void ui_renderInfoBar(doko_image_t* image);

/**
 * Renders the pixel grid.
 */
void ui_renderPixelGrid(doko_image_t* image);

#endif