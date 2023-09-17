
#include <Imlib2.h>

#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "doko.h"
#include "config.h"


void img_init(img_t *img, win_t *win) {

    imlib_set_cache_size(CACHE_SIZE);
    imlib_set_font_cache_size(FONT_CACHE_SIZE);
    imlib_set_color_usage(MAX_COLOR_USAGE);
    imlib_context_set_dither(DITHER_CONTEXT);

	imlib_context_set_display(win->env.dsp);
	imlib_context_set_visual(win->env.vis);
	imlib_context_set_colormap(win->env.cmap);

	img->im = NULL;
	img->win = win;
	img->aa = ANTI_ALIAS;
    img->dirty = false;
}


Imlib_Image img_open(file_t* file) {
    
    Imlib_Image img;

    if (access(file->path, R_OK) != 0) {

        fprintf(stderr, "The file %s does not exist or is missing read permission\n", file->path);

        return img;
    }

    img = imlib_load_image(file->path);
    
    if(!img) {
        fprintf(stderr, "Could not load image %s\n", file->path);
    } 
    
    return img;
}

bool img_load(file_t* file, img_t* img) {
    
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
    
    if(img->im) {

        imlib_context_set_image(img->im);

		if (decache) {

			imlib_free_image_and_decache();
        }
		else {

			imlib_free_image();
        }

		img->im = NULL;
    }
}


void img_draw(img_t* img, win_t* win) {
    
    if(!img->dirty || !img->im)
        return;

    // win_clear(win);
    
    imlib_context_set_image(img->im);
	imlib_context_set_anti_alias(img->aa);
	imlib_context_set_drawable(win->buf.pm);

    imlib_context_set_blend(0);
    imlib_render_image_on_drawable(img->x, img->y);
}