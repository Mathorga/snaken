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

// Translate an id wrapping it to the provided size (pacman effect).
// WARNING: Only works with signed types and does not show errors otherwise.
// [i] is the given index.
// [n] is the size over which to wrap.
#define WRAP(i, n) ((i) >= 0 ? ((i) % (n)) : ((n) + ((i) % (n))))

// Computes the diameter of a square neighborhood given its radius.
#define NH_DIAM_2D(r) (2 * (r) + 1)

// Computes the number of neighbors in a square neighborhood given its diameter.
#define NH_COUNT_2D(d) ((d) * (d) - 1)

// Translates bidimensional indexes to a monodimensional one.
// |i| is the row index.
// |j| is the column index.
// |m| is the number of columns (length of the rows).
#define IDX2D(i, j, m) (((m) * (j)) + (i))

// Translates tridimensional indexes to a monodimensional one.
// |i| is the index in the first dimension.
// |j| is the index in the second dimension.
// |k| is the index in the third dimension.
// |m| is the size of the first dimension.
// |n| is the size of the second dimension.
#define IDX3D(i, j, k, m, n) (((m) * (n) * (k)) + ((m) * (j)) + (i))

typedef int32_t snaken_world_size_t;
typedef uint8_t snaken_snake_speed_t;

typedef enum {
    FALSE = 0x00,
    TRUE = 0x01
} snaken_bool_t;

typedef enum {
    UP = 0x00,
    LEFT = 0x01,
    DOWN = 0x02,
    RIGHT = 0x03
} snaken_dir_t;

typedef enum {
    EMPTY = 0x00,
    SNAKE_HEAD = 0x01,
    SNAKE_BODY = 0x02,
    FOOD = 0x03,
    WALL = 0x04
} snaken_cell_type_t;

#define DEFAULT_SNAKE_SPEED 0x7F
#define DEFAULT_SNAKE_VIEW_RADIUS 0x02
#define STARTING_SNAKE_LENGTH 0x05
#define STARTING_SNAKE_DIR UP

typedef struct {
    // ################ World properties ################

    // World size.
    snaken_world_size_t world_width;
    snaken_world_size_t world_height;

    snaken_cell_type_t* world;

    // ################ Walls locations ################

    // Length of walls array.
    snaken_world_size_t walls_length;

    // Walls array.
    snaken_world_size_t* walls;

    // ################ Apple locations ################

    // Apples array length.
    snaken_world_size_t apples_length;

    // Apples array.
    snaken_world_size_t* apples;

    // ################ Snake locations ################

    // Snake length.
    snaken_world_size_t snake_length;

    // Snake body.
    snaken_world_size_t* snake_body;

    // ################ Snake properties ################

    // Snake speed. Goes from 0x00 to 0xFF, where 0x00 means the snake does not move and 0xFF means the snake moves once every time step.
    snaken_snake_speed_t snake_speed;

    // The current snake direction.
    snaken_dir_t snake_direction;

    // Whether self intersection is enabled or not.
    snaken_bool_t self_intersection;

    // Tells whether the snake is currently alive or not.
    snaken_bool_t snake_alive;

    // Snake view radius.
    snaken_world_size_t snake_view_radius;
} snaken2d_t;


// ########################################## Initialization functions ##########################################

/// @brief Initializes the given snaken with default values.
/// @param snaken The snaken to initialize.
/// @param world_width The width of the snaken world.
/// @param world_height The height of the snaken world.
/// @return The code for the occurred error, [ERROR_NONE] if none.
snaken_error_code_t snaken2d_init(snaken2d_t** snaken, snaken_world_size_t world_width, snaken_world_size_t world_height);


// ########################################## Execution functions ##########################################

snaken_error_code_t snaken2d_update_content(snaken2d_t* snaken);

snaken_error_code_t snaken2d_update_workd(snaken2d_t* snaken);

/// @brief Performs a single run cycle in the provided snaken.
/// @param snaken The snaken to run the loop in.
/// @return The code for the occurred error, [ERROR_NONE] if none.
snaken_error_code_t snaken2d_tick(snaken2d_t* snaken);

/// @brief Spawns a single apple at a random location.
/// @param snaken The snaken in which to spawn the apple.
/// @return The code for the occurred error, [ERROR_NONE] if none.
snaken_error_code_t snaken2d_spawn_apple(snaken2d_t* snaken);


// ########################################## Getter functions ##########################################

/// @brief Retrieves the current snake view and stores it in [view].
/// @param view The view to populate from the given snaken.
/// @param snaken The snaken to extract the view from.
/// @return The code for the occurred error, [ERROR_NONE] if none.
snaken_error_code_t snaken2d_get_snake_view(snaken_world_size_t* view, snaken2d_t* snaken);


// ########################################## Setter functions ##########################################

/// @brief Sets the snake facing direction.
/// @param snaken The snaken to apply the snake direction to.
/// @param direction The direction to set the snake to.
/// @return The code for the occurred error, [ERROR_NONE] if none.
snaken_error_code_t snaken2d_set_snake_dir(snaken2d_t* snaken, snaken_dir_t direction);

/// @brief Applies the provided walls to the provided snaken's world.
/// @param snaken The snaken to apply walls to.
/// @param walls_length The length of the walls array.
/// @param walls The array of walls.
/// @return The code for the occurred error, [ERROR_NONE] if none.
snaken_error_code_t snaken2d_set_walls(snaken2d_t* snaken, snaken_world_size_t walls_length, snaken_world_size_t* walls);


// ########################################## Util functions ##########################################

/// @brief Moves the snake along its facing direction.
/// @param snaken The snaken to compute the movement on.
/// @param result The resulting snake head location.
/// @return The code for the occurred error, [ERROR_NONE] if none.
snaken_error_code_t snaken2d_move_snake(snaken2d_t* snaken, snaken_world_size_t* result);

snaken_error_code_t snaken2d_eat_apple(snaken2d_t* snaken, snaken_bool_t* result);

snaken_error_code_t snaken2d_hit_wall(snaken2d_t* snaken, snaken_bool_t* result);

snaken_error_code_t snaken2d_eat_body(snaken2d_t* snaken, snaken_bool_t* result);


#ifdef __cplusplus
}
#endif

#endif