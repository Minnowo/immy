
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <Imlib2.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "doko.h"
#include "config.h"

// https://x.org/releases/current/doc/libX11/libX11/libX11.html#Opening_the_Display
//
win_t main_window;

img_t image;

file_t* files;
size_t files_len = 0;
size_t files_size = 0;
size_t file_idx = 0;

static bool is_running = true;


void check_add_file(const char* filename) {
    
    char* path = realpath(filename, NULL);
    
    if (path == NULL || access(path, R_OK) != 0) {
        
        error(0, errno, "The file %s does not exist or is missing read permission\n", filename);

        return;
    }
    
    if(files_size + 1 >= files_len) {

        files_len *= 2;
        files = erealloc(files, sizeof(*files) * files_len);
        memset(files + files_len / 2, 0, sizeof(*files) * files_len / 2);
    }
    
    files[files_size].name = estrdup(filename);
    files[files_size].path = path;
    
    files_size++;
}

void remove_n_file(int n) {
    
    if(n < 0 || n > files_size) 
        return;

    file_t* file = &files[n];
    
    if(file->name != file->path)
        free((void*)file->path);
    
    free((void*)file->name);
    
    files_size--;

    if(n == files_size) 
        return;
    
    memmove(files + n, files + n + 1, (files_size - n ) * sizeof(file_t));
}

void load_n_img(int n) {

    if(n < 0 || n >= files_size) 
        return;
    
    bool prev = n < file_idx;
    
    img_close(&image, false);

    while(!img_load(&files[n], &image)) {
        
        error(0, 0, "Could not load image: %d\n", files[n].path);

        remove_n_file(n);
        
        if(n == files_size)
            n--;

        if(n < 0)
            error(EXIT_FAILURE, 0, "No more images to read.");
    }
    
    file_idx = n;
}

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
    double before;

	XLookupString(key, &kc, 1, &ksym, NULL);
    
    if(IsModifierKey(ksym))
        return;

    printf("Image x y : %f %f\n", image.x, image.y);

    switch (ksym) {

        case XK_Left:
            load_n_img(file_idx - 1 % files_size);
        break;
        case XK_Right:
            load_n_img(file_idx + 1 % files_size);
        break;
        case XK_A:
        case XK_a:
            image.aa = !image.aa;
            dirty = true;
            break;
        case XK_D:
        case XK_d:
            image.dither = !image.dither;
            dirty = true;
            break;
        case XK_B:
        case XK_b:
            image.blend = !image.blend;
            dirty = true;
            break;
        case XK_V:
        case XK_v:
    image.x = 0;
    image.y = 0;
    dirty=true;
        break;
    case XK_C:
    case XK_c:
        img_center(&image);
        dirty =true;
        break;
    case XK_equal:
        before = image.zoom;
        image.zoom -= 0.1;
        dirty = true;
        break;
    case XK_minus:
        before = image.zoom;
        image.zoom += 0.1;
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
        img_move(&image, DIR_RIGHT, OPTIONS->image_move_mode);
        break;
    case XK_J:
    case XK_j:
        img_move(&image, DIR_UP, OPTIONS->image_move_mode);
        break;
    case XK_K:
    case XK_k:
        img_move(&image, DIR_DOWN, OPTIONS->image_move_mode);
        break;
    case XK_L:
    case XK_l:
        img_move(&image, DIR_LEFT, OPTIONS->image_move_mode);
        break;
    }
    
    dirty = dirty || image.dirty;

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

void init_files_array(size_t s){
    
    files_len = s;

    files = emalloc(sizeof(*files) * files_len);

    memset(files, 0, sizeof(*files) * files_len);
}

void free_files_array(){

    file_t* file;

    for(int i = 0; i < files_size; i++) {
        
        file = &files[i];
        
        if(file->name != file->path)
            free((void*)file->path);
        
        free((void*)file->name);
    }
    
    free(files);
}

int main(int argc, char **argv) {
    
    char* filename;
    size_t n;
    ssize_t len;
    
    parse_start_arguments(argc, argv);
    
    init_files_array(OPTIONS->file_count);
    
    
    if(OPTIONS->read_stdin) {
        printf("Reading form stdin\n");
        
        n = 0;
        filename = NULL;
        
        while ((len = getline(&filename, &n, stdin)) > 0) {

            if (filename[len-1] == '\n')
                filename[len-1] = '\0';

            check_add_file(filename);
        }

        free(filename);

    } else {
        
        
        for(int i = 0; i < OPTIONS->file_count; i++) {
            
            filename = OPTIONS->filenames[i];
            
            check_add_file(filename);
            
        }
    }
    
    if(files_size == 0)
        error(EXIT_FAILURE, 0, "No files to read... aborting.");

    win_init(&main_window);

    win_create(&main_window);
   
    img_init(&image, &main_window);

    load_n_img(0);
   
    run(&main_window);
    
    img_close(&image, true);

    win_close(&main_window);
    
    free_files_array();

    return 0;
}