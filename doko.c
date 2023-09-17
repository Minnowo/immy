
#include <X11/X.h>
#include <X11/Xlib.h>

#include <Imlib2.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doko.h"

// https://x.org/releases/current/doc/libX11/libX11/libX11.html#Opening_the_Display
//
win_t main_window;


void on_keypress() {

}

void render() {

}


void run(win_t* win) {
    
    win_env_t* e;
    XEvent ev;
    
    bool running;
    
    e = &win->env;
    running = true;
    
    while(running) {

      do {
        XNextEvent(e->dsp, &ev);

        switch (ev.type) {

               case Expose:
                   break;

        case ClientMessage:

          if ((Atom) ev.xclient.data.l[0] == atoms[ATOM_WM_DELETE_WINDOW])

            running = false;

          break;
        }

      } while (XPending(e->dsp));
    }

}

int main() {

    win_init(&main_window);

    win_create(&main_window);

    run(&main_window);

    win_close(&main_window);

    return 0;
}