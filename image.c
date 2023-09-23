
#include <Imlib2.h>

#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "doko.h"
#include "config.h"


void img_init(img_t *img, win_t *win) {

    imlib_set_cache_size(OPTIONS->image_cache_size);
    imlib_set_font_cache_size(OPTIONS->font_cache_size);
    imlib_set_color_usage(OPTIONS->max_color_usage);
    imlib_context_set_dither(OPTIONS->dither_context);

	imlib_context_set_display(win->env.dsp);
	imlib_context_set_visual(win->env.vis);
	imlib_context_set_colormap(win->env.cmap);

	img->im = NULL;
	img->win = win;
	img->aa = ANTI_ALIAS;
    img->dirty = false;
    img->gamma = 1.0;
    img->cmod = imlib_create_color_modifier();

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

    win_clear(win);
    
    imlib_context_set_image(img->im);
	imlib_context_set_anti_alias(img->aa);
	imlib_context_set_drawable(win->buf.pm);

    imlib_context_set_blend(0);
    imlib_render_image_on_drawable(img->x, img->y);
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
    
    imlib_context_set_image(img->im);

    int width = imlib_image_get_width();
    int height = imlib_image_get_height();
    int len = width * height;
    
    Imlib_Color color;
    for (int i = 0; i < len; i++) {

        int x = i % width;
        int y = i / width;

        imlib_image_query_pixel(x, y, &color);
        imlib_context_set_color(255 - color.red, 255 - color.green,
                                255 - color.blue, color.alpha);
        imlib_image_draw_pixel(x, y, 0);
    }

    /*
     * https://docs.enlightenment.org/api/imlib2/html/imlib2_8c.html#17817446139a645cc017e9f79124e5a2
     * probably faster, but will invalidate the cache
     
    DATA32 *im_data = imlib_image_get_data ();
    rgba_t *c = im_data;
    
    for (int y = 0; y < len; y++) {

        c->rgba.r = 255 - c->rgba.r;
        c->rgba.g = 255 - c->rgba.g;
        c->rgba.b = 255 - c->rgba.b;
        
        c++;
    }
    */

    img->dirty = true;
    
}