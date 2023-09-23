
#ifndef DOKO_CONF_H
#define DOKO_CONF_H 


#define ANTI_ALIAS 1

#define CACHE_SIZE 1024 * 1024 * 50
#define FONT_CACHE_SIZE 1024 * 1024 * 1
#define MAX_COLOR_USAGE 65510 
#define DITHER_CONTEXT 1 

#define IMG_MOVE_DELTA 50

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