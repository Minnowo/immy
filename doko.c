
#include <X11/X.h>
#include <X11/Xlib.h>

#include <Imlib2.h>

#include <X11/Xutil.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "doko.h"
#include "config.h"

// https://x.org/releases/current/doc/libX11/libX11/libX11.html#Opening_the_Display
//
win_t main_window;

img_t image;

static bool is_running = true;

void redraw(){

    img_draw(&image, &main_window);

    win_draw(&main_window);
}

void update_gamma_by(double amt) {

    image.gamma += amt;

    if(image.gamma <= 0) 
        image.gamma = 0.01;
    
    if(image.gamma > 10)
        image.gamma = 10;

    img_apply_gamma(&image);
}

void on_keypress(XKeyEvent* key) {
    
    bool dirty = image.dirty;
	KeySym ksym;
	char kc;

	XLookupString(key, &kc, 1, &ksym, NULL);
    
    if(IsModifierKey(ksym))
        return;

    switch (ksym) {

    case XK_equal:
        image.zoom += 0.1;
        dirty = true;
        break;
    case XK_minus:
        image.zoom -= 0.1;
        dirty = true;
        break;
    case XK_Q:
    case XK_q:
        is_running = false;
        break;
    case XK_F:
    case XK_f:
        win_toggle_fullscreen(&main_window);
        break;

    case XK_I:
    case XK_i:
        img_invert(&image);
        break;

    case XK_6:
        update_gamma_by(GAMMA_DELTA);
        break;

    case XK_5:
        update_gamma_by(-GAMMA_DELTA);
        break;

    case XK_H:
    case XK_h:
        image.x += IMG_MOVE_DELTA;
        dirty = true;
        break;
    case XK_J:
    case XK_j:
        image.y -= IMG_MOVE_DELTA;
        dirty = true;
        break;
    case XK_K:
    case XK_k:
        image.y += IMG_MOVE_DELTA;
        dirty = true;
        break;
    case XK_L:
    case XK_l:
        image.x -= IMG_MOVE_DELTA;
        dirty = true;
        break;
    }

    if(dirty && is_running) {
        redraw();
    }
}

void run(win_t *win) {

    win_env_t *e;
    XEvent ev, nextev;
    bool discard;

    e = &win->env;

    while (is_running) {

        do {
          discard = false;

          XNextEvent(win->env.dsp, &ev);

          if (XEventsQueued(win->env.dsp, QueuedAlready) <= 0)
            break;

          XPeekEvent(win->env.dsp, &nextev);

          switch (ev.type) {
          case ConfigureNotify:
          case MotionNotify:
            discard = ev.type == nextev.type;
            break;
          case KeyPress:
            discard = (nextev.type == KeyPress || nextev.type == KeyRelease) &&
                      ev.xkey.keycode == nextev.xkey.keycode;
            break;
          }
        } while (discard);

        switch (ev.type) {

        case Expose:
          redraw();
          break;

        case KeyPress:
          on_keypress(&ev.xkey);
          break;

        case ClientMessage:

          if ((Atom)ev.xclient.data.l[0] == atoms[ATOM_WM_DELETE_WINDOW])

            is_running = false;

          break;

        case ConfigureNotify:
          if (win_configure(win, &ev.xconfigure)) {
            printf("Win buff: %d x %d\n", win->buf.w, win->buf.h);
            printf("Win size: %d x %d\n", win->w, win->h);
          }
          break;
        }
    }
}

int main(int argc, char *argv[]) {
    
    parse_start_arguments(argc, argv);
    
    printf("File: %s\n", OPTIONS->image.path);

    if (access(OPTIONS->image.path, R_OK) != 0) { 
        
        fprintf(stderr, "Could not find or read the file %s\n", OPTIONS->image.path);
        return 1;
    }

    win_init(&main_window);

    win_create(&main_window);
   
    img_init(&image, &main_window);

    img_load(&OPTIONS->image, &image);
   
    run(&main_window);
    
    img_close(&image, true);

    win_close(&main_window);

    return 0;
}