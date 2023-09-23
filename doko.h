

#ifndef DOKO_H
#define DOKO_H

#include <X11/X.h>
#include <X11/Xlib.h>

#include <Imlib2.h>

#include <stdint.h>
#include <stdbool.h>

#define MAX(a, b) a > b ? a : b

typedef enum {
  DIR_LEFT = 0,
  DIR_RIGHT = 1,
  DIR_UP = 2,
  DIR_DOWN = 3
} direction_t;

typedef enum {
  MOVE_SCREEN = 0,
  MOVE_VISIBLE_IMG = 1,
} move_type_t;

typedef union {
  struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  } rgba;

  uint32_t decimal;
} rgba_t;

typedef struct win_env{
    Display* dsp;
	int scr;
	int scrw, scrh;
	Visual *vis;
	Colormap cmap;
	int depth;
} win_env_t;


typedef struct {
    
    Window xwin;

    win_env_t env;
    
    rgba_t bg;
    rgba_t fg;

    int x;
    int y;

    unsigned int w;
    unsigned int h;

    struct {
        int w;
        int h;
        Pixmap pm;
    } buf;
} win_t;


typedef struct {
	const char *name; /* as given by user */
	const char *path; /* always absolute */
} file_t;


typedef struct {
  file_t image;

  int win_x;
  int win_y;
  int win_w;
  int win_h;
  
  move_type_t image_move_mode;
  
  int max_color_usage;
  uint64_t image_cache_size;
  uint64_t font_cache_size;

  bool dither_context;
  bool anti_alias;
  bool blend;
  bool fullscreen;

} options_t;

extern const options_t* OPTIONS;

#pragma region DOKO_WINDOW_H

#define ATOM_COUNT 3

enum {
    ATOM_WM_DELETE_WINDOW = 0,

  // https://specifications.freedesktop.org/wm-spec/1.3/ar01s05.html
    _NET_ATOM_WM_STATE = 1,
    _NET_WM_STATE_FULLSCREEN = 2,
};

extern Atom atoms[ATOM_COUNT];

void win_init(win_t* win);
void win_create(win_t* win);
void win_close(win_t* win);
bool win_configure(win_t *win, XConfigureEvent *c);
void win_clear(win_t *win);
void win_draw(win_t *win);
void win_toggle_fullscreen(win_t *win);

#pragma endregion


#pragma region DOKO_IMAGE_H

typedef struct img {
  
  Imlib_Image im;
  int w;
  int h;

  win_t* win;
  float x;
  float y;
  double zoom;
  
  double gamma;
	Imlib_Color_Modifier cmod;

  bool aa;
  bool dither;
  bool blend;
  bool dirty;

} img_t;


void img_close(img_t* img, bool decache);
bool img_load(file_t* file, img_t* img);
Imlib_Image img_open(file_t* file);
void img_init(img_t *img, win_t *win);
void img_draw(img_t *img, win_t *win);
void img_invert(img_t* img);
void img_apply_gamma(img_t* img);
void img_center(img_t* img);
void img_move(img_t* img, direction_t dir, move_type_t mmode);

#pragma endregion



#pragma region DOKO_OPTIONS_H


bool parse_start_arguments(int argc, char* argv[]);

#pragma endregion



#endif