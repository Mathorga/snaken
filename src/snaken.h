/*
*****************************************************************
cortex.h

Copyright (C) 2024 Luka Micheletti
*****************************************************************
*/

#ifndef __SNAKEN__
#define __SNAKEN__

#include <stdint.h>

#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_SNAKE_SPEED 0x7F

typedef int32_t snaken_world_size_t;
typedef uint8_t snaken_snake_speed_t;

typedef enum {
    FALSE = 0x00,
    TRUE = 0x01
} snaken_bool_t;

typedef struct {
    // ################ World properties ################

    // World size.
    snaken_world_size_t world_width;
    snaken_world_size_t world_height;

    // ################ Walls locations ################

    // Length of walls array.
    snaken_world_size_t walls_length;

    // Walls array.
    snaken_world_size_t* walls;

    // ################ Food locations ################

    // Food array length.
    snaken_world_size_t food_length;

    // Food array.
    snaken_world_size_t* food;

    // ################ Snake locations ################

    // Snake length.
    snaken_world_size_t snake_length;

    // Snake body.
    snaken_world_size_t* snake_body;

    // ################ Snake properties ################

    // Snake speed. Goes from 0x00 to 0xFF, where 0x00 means the snake does not move and 0xFF means the snake moves once every time step.
    snaken_snake_speed_t snake_speed;

    // Whether or not self intersection is enabled or not.
    snaken_bool_t self_intersection;

    // Snake view radius.
    snaken_world_size_t snake_view_radius;
} snaken2d_t;


// ########################################## Initialization functions ##########################################

/// @brief Initializes the given snaken with default values.
/// @param snaken The snaken to initialize.
/// @param width The width of the snaken world.
/// @param height The height of the snaken world.
/// @return The code for the occurred error, [ERROR_NONE] if none.
snaken_error_code_t snaken2d_init(snaken2d_t** snaken, snaken_world_size_t width, snaken_world_size_t height);


// ########################################## Execution functions ##########################################

/// @brief Performs a single run cycle in the provided snaken.
/// @param snaken The snaken to run the loop in.
/// @return The code for the occurred error, [ERROR_NONE] if none.
snaken_error_code_t snaken2d_tick(snaken2d_t* snaken);


// ########################################## Getter functions ##########################################

snaken_error_code_t snaken2d_get_snake_view();


// ########################################## Setter functions ##########################################

snaken_error_code_t snaken2d_set_snake_move();


#ifdef __cplusplus
}
#endif

#endif