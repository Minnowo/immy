#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "doko.h"


options_t _options;

const options_t* OPTIONS = &_options;
const char ARG_PREFIX = '-';


bool parse_start_arguments(int argc, char* argv[]){
    
    _options.anti_alias = ANTI_ALIAS;
    _options.dither_context = DITHER_CONTEXT;
    _options.font_cache_size = FONT_CACHE_SIZE;
    _options.image_cache_size = CACHE_SIZE;
    _options.max_color_usage = MAX_COLOR_USAGE;
    _options.win_w = WINDOW_W;
    _options.win_h = WINDOW_H;
    _options.win_x = WINDOW_X;
    _options.win_y = WINDOW_Y;
    
    for(int i = 1; i < argc; i++) {
        
        const char* arg_str = argv[i];
        const int arg_str_len = strlen(arg_str);

        if (arg_str_len < 2) {
          fprintf(stderr, "Argument %d has invalid length\n", i);
          continue;
        }
        
        const int is_short_arg = arg_str[0] == ARG_PREFIX && arg_str[1] != ARG_PREFIX;

        if(is_short_arg) {

           for(int c = 1; c < arg_str_len; c++) {
               
               switch (arg_str[c]) {
                   
                   case 'a': _options.anti_alias = 0; break;
                   case 'd': _options.dither_context = 0; break;
                   case 'f': _options.fullscreen = 1; break;
               }
           } 
        }
        
        _options.image.name = arg_str;
        printf("%d : %s\n", i, argv[i]);
    }

    _options.image.path = realpath(_options.image.name, NULL);

    return true;
}