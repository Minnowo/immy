

#ifndef DOKO_H
#define DOKO_H

#include <X11/X.h>
#include <X11/Xlib.h>

#include <Imlib2.h>

#include <stdint.h>
#include <stdbool.h>

#define MAX(a, b) a > b ? a : b

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



#pragma region DOKO_WINDOW_H

#define ATOM_COUNT 1

enum {
    ATOM_WM_DELETE_WINDOW = 0
};

extern Atom atoms[ATOM_COUNT];

void win_init(win_t* win);
void win_create(win_t* win);
void win_close(win_t* win);
bool win_configure(win_t *win, XConfigureEvent *c);
void win_clear(win_t *win);
void win_draw(win_t *win);

#pragma endregion


#pragma region DOKO_IMAGE_H

typedef struct img {
  
  Imlib_Image im;
  int w;
  int h;

  win_t* win;
  float x;
  float y;
  
  bool aa;
  bool dirty;

} img_t;


void img_close(img_t* img, bool decache);
bool img_load(file_t* file, img_t* img);
Imlib_Image img_open(file_t* file);
void img_init(img_t *img, win_t *win);
void img_draw(img_t *img, win_t *win);

#pragma endregion




#endif