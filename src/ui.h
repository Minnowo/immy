


#ifndef DOKO_UI_H
#define DOKO_UI_H

#include <raylib.h>
#include <stddef.h>

#include "doko.h"

#define COLOR_INVERT(c)                                                        \
    ((Color){.r = 255 - (c).r, .g = 255 - (c).g, .b = 255 - (c).b})

/**
 * Initializes and creates the window/UI components. Should always be called first.
 */
void ui_init();

/**
 * Deinitializes and destroys all the window/UI components. Should always be called last.
 */
void ui_deinit();

/**
 * Unload and reload Unifont with the codepoints stored
 */
void ui_loadUnifont();

/*
 * Sets the initial codepoints 
 */
void ui_setInitialCodePoints(const char* text);

/*
 *  Adds codepoints from the filelist
 */
void ui_loadCodepointsFromFileList(doko_control_t* ctrl);

/*
 *  Add codepoints from the given string
 */
void ui_loadCodepoints(const char* text, bool reloadFont);

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
 * Renders the info bar with this text.
 */
void ui_renderTextOnInfoBar(const char* text);

/**
 * Renders the info bar.
 */
void ui_renderInfoBar(doko_image_t* image);

/**
 * Renders the pixel grid.
 */
void ui_renderPixelGrid(doko_image_t* image);

/**
 * Renders the a file list.
 */
void ui_renderFileList(doko_control_t* ctrl);

/**
 * Renders the a file list.
 */
void ui_renderKeybinds(doko_control_t* ctrl);

#endif
