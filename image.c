
#include <X11/X.h>
#include <time.h>
#include <Imlib2.h>

#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "doko.h"
#include "config.h"


void img_init(img_t *img, const win_t *win) {

    imlib_set_cache_size(OPTIONS->image_cache_size);
    imlib_set_font_cache_size(OPTIONS->font_cache_size);
    imlib_set_color_usage(OPTIONS->max_color_usage);
    imlib_context_set_dither(OPTIONS->dither_context);

	imlib_context_set_display(win->env.dsp);
	imlib_context_set_visual(win->env.vis);
	imlib_context_set_colormap(win->env.cmap);

	img->im = NULL;
	img->win = win;
	img->aa = OPTIONS->anti_alias;
    img->blend = OPTIONS->blend;
    img->dither =  OPTIONS->dither_context;
    img->dirty = false;
    img->gamma = 1.0;
    img->zoom = 1.0;
    img->cmod = imlib_create_color_modifier();

}


Imlib_Image img_open(const file_t* file) {
    
    Imlib_Image img = NULL;

    if (access(file->path, R_OK) != 0) {

        error(0, errno, "The file %s does not exist or is missing read permission", file->path);

        return img;
    }

    img = imlib_load_image(file->path);
    
    if(img == NULL) 
        error(0, errno, "Could not load image %s", file->path);
    
    return img;
}

bool img_load(const file_t* file, img_t* img) {
    
    Imlib_Image i = img_open(file);
    
    if(!i) {
        return false;
    }
    
    img->im = i;

    imlib_context_set_image(i);
    
    img->w = imlib_image_get_width();

    img->h = imlib_image_get_height();
    
    img->dirty = true;
    
    return true;
}

void img_close(img_t* img, bool decache) {
    
    if(img->im == NULL)
        return;

    imlib_context_set_image(img->im);

    if (decache) {

        imlib_free_image_and_decache();
    }
    else {

        imlib_free_image();
    }

    img->im = NULL;
}


void img_draw(img_t* img, win_t* win) {
    
    if(!img->dirty || !img->im)
        return;

    win_clear(win);
    
    imlib_context_set_image(img->im);
	imlib_context_set_anti_alias(img->aa);
    imlib_context_set_dither(img->dither);
    imlib_context_set_blend(img->blend);
	imlib_context_set_drawable(win->buf.pm);
    
    double zoom = img->zoom;
    int ix = img->x;
    int iy = img->y;
    int wx = 0;
    int wy = 0;
    
    if(ix < 0) {
        wx = -ix ;
        ix = 0;
    } else {
        ix *= zoom;
    }

    if(iy < 0) {
        wy = -iy ;
        iy = 0;
    } else {
        iy *= zoom;
    }

    imlib_render_image_part_on_drawable_at_size(
        ix, iy, img->win->w * zoom,
        img->win->h * zoom, wx, wy,
        img->win->w, img->win->h);
}


void img_apply_gamma(img_t* img) {

    if(!img || !img->im) 
        return;
    
    if(!img->cmod) 
        img->cmod = imlib_create_color_modifier();
    
    if(img->gamma > GAMMA_RANGE_MAX)
        img->gamma = GAMMA_RANGE_MAX;
    
    if(img->gamma < GAMMA_RANGE_MIN) 
        img->gamma = GAMMA_RANGE_MIN;
    
    imlib_context_set_image(img->im);
    imlib_context_set_color_modifier(img->cmod);

    imlib_reset_color_modifier();
    imlib_modify_color_modifier_gamma(img->gamma);
    
    img->dirty = true;
}


void img_invert(img_t* img) {
    
    if(!img || !img->im) 
        return;
    
    uint8_t red_table[256];
    uint8_t green_table[256];
    uint8_t blue_table[256];
    uint8_t alpha_table[256];

    for (int i = 0; i < 256; i++) {
        red_table[i] = 255 - i;
        green_table[i] = 255 - i;
        blue_table[i] = 255 - i;
        alpha_table[i] = i;
    }

    imlib_context_set_image(img->im);

    Imlib_Color_Modifier cmod = imlib_create_color_modifier();
    imlib_context_set_color_modifier(cmod);
    imlib_set_color_modifier_tables(red_table, green_table, blue_table, alpha_table);
    imlib_apply_color_modifier();
    imlib_free_color_modifier();

    imlib_context_set_color_modifier(img->cmod);

    img->dirty = true;
}


void img_center(img_t* img) {
    
    double visW = img->w / img->zoom;
    double visH = img->h / img->zoom;
    int winW = img->win->w;
    int winH = img->win->h;

    if(visW > winW) {
        img->x = (visW / 2.0) - (winW /2.0);
    } else {
        img->x = -(winW / 2.0) + (visW / 2.0);
    }

    if(visH > winH) {
        img->y = (visH / 2.0) - (winH /2.0);
    } else {
        img->y = -(winH / 2.0) + (visH / 2.0);
    }
}


void img_move(img_t* img, direction_t dir, move_type_t mmode) {

    int mx;
    int my;
    int ox = img->x;
    int oy = img->y;

    switch (mmode) {

    default:
    case MOVE_SCREEN:
        mx = (img->win->w / IMG_MOVE_FRAC);
        my = (img->win->h / IMG_MOVE_FRAC);
        break;

    case MOVE_VISIBLE_IMG:
        mx = (img->w / img->zoom / IMG_MOVE_FRAC) - IMG_MOVE_DELTA;
        my = (img->h / img->zoom / IMG_MOVE_FRAC) - IMG_MOVE_DELTA;
        break;
    }

    switch (dir) {
        
        case DIR_UP:
            img->y -= my;
            break;
        case DIR_DOWN:
            img->y += my;
            break;
        case DIR_LEFT:
            img->x -= mx;
            break;
        case DIR_RIGHT:
            img->x += mx;
            break;
    }

    if (-img->x > img->win->w) {

        img->x = 1 - (int)img->win->w;

    } else if (img->x > img->w / img->zoom) {

        img->x = img->w / img->zoom - 1;

    }

    if (-img->y > img->win->h) {

        img->y = 1 - (int)img->win->h;

    } else if (img->y > img->h / img->zoom) {

        img->y = img->h / img->zoom - 1;

    }
    
    img->dirty = img->x != ox || img->y != oy;
}