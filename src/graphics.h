#ifndef __SNAKEN_GRAPHICS__
#define __SNAKEN_GRAPHICS__

#include <raylib.h>

#include "snaken.h"
#include "utils.h"

/// @brief 
/// @param window_width 
/// @param window_height 
/// @return 
snaken_error_code_t snaken_graphics_begin(
    int window_width,
    int window_height
);

/// @brief 
/// @param snaken 
/// @return 
snaken_error_code_t snaken_graphics_draw(
    snaken2d_t* snaken
);

/// @brief 
/// @return 
snaken_error_code_t snaken_graphics_end();

#endif