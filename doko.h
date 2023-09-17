

#ifndef DOKO_H
#define DOKO_H

#include <X11/X.h>
#include <X11/Xlib.h>

#include <Imlib2.h>
#include <stdint.h>



#define ATOM_COUNT 1

enum {
    ATOM_WM_DELETE_WINDOW = 0
};

extern Atom atoms[ATOM_COUNT];

typedef union {
  struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  } rgba;

  uint32_t decimal;
} rgba_t;

typedef struct {
    Display* dsp;
	int scr;
	int scrw, scrh;
	Visual *vis;
	Colormap cmap;
	int depth;
} win_env_t;


struct win {
    
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
};


typedef struct win win_t;




void win_init(win_t* win);
void win_create(win_t* win);
void win_close(win_t* win);

#endif