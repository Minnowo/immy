


#include <X11/X.h>
#include <X11/Xlib.h>

#include <stdbool.h>
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
    
    printf("Screen dims %d x %d\n", e->scrw, e->scrh);
    printf("Primary screen dims %d x %d\n", DisplayWidth(e->dsp, e->scr), DisplayHeight(e->dsp, e->scr));

    atoms[ATOM_WM_DELETE_WINDOW] = XInternAtom(e->dsp, "WM_DELETE_WINDOW", False);
    atoms[_NET_ATOM_WM_STATE] = XInternAtom(e->dsp, "_NET_WM_STATE", False);
    atoms[_NET_WM_STATE_FULLSCREEN] = XInternAtom(e->dsp, "_NET_WM_STATE_FULLSCREEN", False);
}

void win_create(win_t *win) {

    win_env_t *e;
    long parent;

    e = &win->env;

    parent = RootWindow(e->dsp, e->scr);

    win->w = OPTIONS->win_w;
    win->h = OPTIONS->win_h;
    win->x = OPTIONS->win_x;
    win->y = OPTIONS->win_y;
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

    XSelectInput(e->dsp, win->xwin, KeyPressMask| ExposureMask | StructureNotifyMask);

    XSetWMProtocols(e->dsp, win->xwin, &atoms[ATOM_WM_DELETE_WINDOW], 1);


	gc = XCreateGC(e->dsp, win->xwin, 0, None);
    
	XSetForeground(e->dsp, gc, win->bg.decimal);
	XFillRectangle(e->dsp, win->buf.pm, gc, 0, 0, win->buf.w, win->buf.h);
	XSetWindowBackgroundPixmap(e->dsp, win->xwin, win->buf.pm);

	XMapWindow(e->dsp, win->xwin);
	XFlush(e->dsp);
    
    if(OPTIONS->fullscreen) {
        win_toggle_fullscreen(win);
    }
}


void win_close(win_t *win)
{
    XFreeGC(win->env.dsp, gc);
	XDestroyWindow(win->env.dsp, win->xwin);
	XCloseDisplay(win->env.dsp);
}


bool win_configure(win_t *win, XConfigureEvent *c)
{
	bool changed;

	changed = win->w != c->width || win->h != c->height;

	win->x = c->x;
	win->y = c->y;
	win->w = c->width;
	win->h = c->height;

	return changed;
}

void win_clear(win_t *win)
{
	win_env_t *e;

	e = &win->env;

	if (win->w > win->buf.w || win->h > win->buf.h) {

		XFreePixmap(e->dsp, win->buf.pm);

		win->buf.w = MAX(win->buf.w, win->w);

		win->buf.h = MAX(win->buf.h, win->h);

		win->buf.pm = XCreatePixmap(e->dsp, win->xwin,
		                            win->buf.w, win->buf.h, e->depth);
	}

	XSetForeground(e->dsp, gc, win->bg.decimal);
	XFillRectangle(e->dsp, win->buf.pm, gc, 0, 0, win->buf.w, win->buf.h);
}


void win_draw(win_t *win)
{

	XSetWindowBackgroundPixmap(win->env.dsp, win->xwin, win->buf.pm);
	XClearWindow(win->env.dsp, win->xwin);
	XFlush(win->env.dsp);
}


void win_toggle_fullscreen(win_t *win)
{
    printf("Toggle fullscreen\n");

	XEvent ev;
	XClientMessageEvent *cm;

	memset(&ev, 0, sizeof(ev));
	ev.type = ClientMessage;

	cm = &ev.xclient;
	cm->window = win->xwin;
	cm->message_type = atoms[_NET_ATOM_WM_STATE]; 
	cm->format = 32;
	cm->data.l[0] = 2; // toggle
	cm->data.l[1] = atoms[_NET_WM_STATE_FULLSCREEN];

	XSendEvent(win->env.dsp, DefaultRootWindow(win->env.dsp), False,
	           SubstructureNotifyMask | SubstructureRedirectMask, &ev);
}