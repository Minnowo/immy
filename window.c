


#include <X11/X.h>
#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doko.h"

Atom atoms[ATOM_COUNT];

static GC gc;

void win_init(win_t* win) {
    
    win_env_t* e;

    memset(win, 0, sizeof(win_t));
    
    e = &win->env;

    e->dsp = XOpenDisplay(NULL);
    
    if(!e->dsp) {

        fprintf(stderr, "Could not connect to X\n");
        exit(1);
    }

    e->scr = DefaultScreen(e->dsp);
    e->scrw = DisplayWidth(e->dsp, e->scr);
    e->scrh = DisplayHeight(e->dsp, e->scr);
    e->vis = DefaultVisual(e->dsp, e->scr);
    e->cmap = DefaultColormap(e->dsp, e->scr);
    e->depth = DefaultDepth(e->dsp, e->scr);
    
    atoms[ATOM_WM_DELETE_WINDOW] = XInternAtom(e->dsp, "WM_DELETE_WINDOW", False);
}

void win_create(win_t *win) {

    win_env_t *e;
    long parent;

    e = &win->env;

    parent = RootWindow(e->dsp, e->scr);
    
    win->w = 200;
    win->h = 200;
    win->x = e->scrw / 2;
    win->y = e->scrh / 2;
	win->buf.w = e->scrw;
	win->buf.h = e->scrh;

    win->xwin = XCreateWindow(e->dsp, parent, win->x, win->y, win->w, win->h, 0,
                              e->depth, InputOutput, e->vis, 0, NULL);
    

    if(!win->xwin) {
        fprintf(stderr, "Could not create xwindow\n");
        exit(1);
    }

	win->buf.pm = XCreatePixmap(e->dsp, win->xwin,
	                            win->buf.w, win->buf.h, e->depth);

    XSelectInput(e->dsp, win->xwin,
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                     ExposureMask);

    XSetWMProtocols(e->dsp, win->xwin, &atoms[ATOM_WM_DELETE_WINDOW], 1);


	gc = XCreateGC(e->dsp, win->xwin, 0, None);
    
	XSetForeground(e->dsp, gc, win->bg.decimal);
	XFillRectangle(e->dsp, win->buf.pm, gc, 0, 0, win->buf.w, win->buf.h);
	XSetWindowBackgroundPixmap(e->dsp, win->xwin, win->buf.pm);

	XMapWindow(e->dsp, win->xwin);
	XFlush(e->dsp);
}


void win_close(win_t *win)
{
    XFreeGC(win->env.dsp, gc);
	XDestroyWindow(win->env.dsp, win->xwin);
	XCloseDisplay(win->env.dsp);
}