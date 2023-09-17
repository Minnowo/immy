
#include <X11/X.h>
#include <X11/Xlib.h>

#include <Imlib2.h>

#include <X11/Xutil.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void on_keypress(XKeyEvent* key) {
    
    bool dirty = image.dirty;
	KeySym ksym;
	char kc;

	XLookupString(key, &kc, 1, &ksym, NULL);
    
    if(IsModifierKey(ksym))
        return;

    switch (ksym) {

    case XK_Q:
    case XK_q:
        is_running = false;
        break;

    case XK_H:
    case XK_h:
        image.x -= 50;
        dirty = true;
        break;
    case XK_J:
    case XK_j:
        image.y += 50;
        dirty = true;
        break;
    case XK_K:
    case XK_k:
        image.y -= 50;
        dirty = true;
        break;
    case XK_L:
    case XK_l:
        image.x += 50;
        dirty = true;
        break;
    }

    if(dirty && is_running) {
        redraw();
    }
}

void run(win_t *win) {

    win_env_t *e;
    XEvent ev;

    e = &win->env;

    while (is_running) {

        do {
          XNextEvent(e->dsp, &ev);

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
            }
          }

        } while (XPending(e->dsp));
        

    }
}

int main(int argc, char *argv[]) {

    win_init(&main_window);

    win_create(&main_window);
   
    img_init(&image, &main_window);
    
    file_t file;
    file.name = "./test_images/bg.jpg";
    file.path = realpath(file.name, NULL);
    
    printf("File: %s\n", file.path);

    img_load(&file, &image);
   
    run(&main_window);
    
    img_close(&image, true);

    win_close(&main_window);

    return 0;
}