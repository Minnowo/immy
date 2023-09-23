
#ifndef DOKO_CONF_H
#define DOKO_CONF_H 

#include <limits.h>
#include "doko.h"

#define ANTI_ALIAS 1

#define CACHE_SIZE 1024 * 1024 * 50
#define FONT_CACHE_SIZE 1024 * 1024 * 1
#define MAX_COLOR_USAGE 255
#define DITHER_CONTEXT 1 

#define IMG_MOVE_DELTA 50
#define IMG_MOVE_FRAC 5.0
#define IMAGE_MOVE_MODE MOVE_SCREEN 

#define WINDOW_W 512
#define WINDOW_H 512
#define WINDOW_X 0
#define WINDOW_Y 0

/**
 * The amount in which gamma is increase / decreased by hotkeys
*/
#define GAMMA_DELTA 0.1

/**
 * The highest and lowset possible gamma value
*/
#define GAMMA_RANGE_MAX 32.0
#define GAMMA_RANGE_MIN 0.01

#endif