/*
*****************************************************************
cortex.h

Copyright (C) 2024 Luka Micheletti
*****************************************************************
*/

#ifndef __SNAKEN__
#define __SNAKEN__

#include <stdint.h>
#include <stdlib.h>

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
typedef uint8_t snaken_snake_stamina_t;

typedef enum {
    SNAKEN_FALSE = 0x00,
    SNAKEN_TRUE = 0x01
} snaken_bool_t;

typedef enum {
    SNAKEN_UP = 0x00,
    SNAKEN_LEFT = 0x01,
    SNAKEN_DOWN = 0x02,
    SNAKEN_RIGHT = 0x03
} snaken_dir_t;

typedef enum {
    SNAKEN_EMPTY = 0x00,
    SNAKEN_SNAKE_HEAD = 0x01,
    SNAKEN_SNAKE_BODY = 0x02,
    SNAKEN_APPLE = 0x03,
    SNAKEN_WALL = 0x04
} snaken_cell_type_t;

#define SNAKEN_DEFAULT_SNAKE_SPEED 0xFEu
#define SNAKEN_DEFAULT_SNAKE_STAMINA 0x7Fu
#define SNAKEN_DEFAULT_SNAKE_VIEW_RADIUS 0x02u
#define SNAKEN_DEFAULT_APPLES_LENGTH 0x05u

#define SNAKEN_MAX_SNAKE_STAMINA 0xFFu

#define SNAKEN_SNAKE_STAMINA_LOW 0x11u
#define SNAKEN_SNAKE_STAMINA_MID 0x66u
#define SNAKEN_SNAKE_STAMINA_HIGH 0xCCu
#define SNAKEN_SNAKE_STAMINA_UNLIMITED SNAKEN_MAX_SNAKE_STAMINA

#define SNAKEN_STARTING_SNAKE_LENGTH 0x05u
#define SNAKEN_STARTING_SNAKE_DIR SNAKEN_UP

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

    // Snake speed buildup. Increases at every time step and gets back to 0 when the snake moves.
    snaken_snake_speed_t snake_speed_step;

    // Snake hunger. Goes from 0x00 to 0xFF, where 0x00 means the snake never shrinks from hunger and 0xFF Means the snake shrihks once every time step.
    snaken_snake_stamina_t snake_stamina;

    // Hunger buildup. Increases at every time step and gets back to 0 when the snake eats an apple.
    snaken_snake_stamina_t snake_stamina_step;

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
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_init(snaken2d_t** snaken, snaken_world_size_t world_width, snaken_world_size_t world_height);


// ########################################## Execution functions ##########################################

/// @brief Performs a single run cycle in the provided snaken.
/// @param snaken The snaken to run the loop in.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_tick(snaken2d_t* snaken);


// ########################################## Getter functions ##########################################

/// @brief Retrieves the current snake view and stores it in [view].
/// @param view The view to populate from the given snaken.
/// @param snaken The snaken to extract the view from.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_get_snake_view(snaken2d_t* snaken, snaken_cell_type_t* view);


// ########################################## Setter functions ##########################################

/// @brief Sets the snake facing direction.
/// @param snaken The snaken to apply the snake direction to.
/// @param direction The direction to set the snake to.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_set_snake_dir(snaken2d_t* snaken, snaken_dir_t direction);

/// @brief Turns the snake left relative to its current direction.
/// @param snaken The snaken to apply the turn to.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_turn_left(snaken2d_t* snaken);

/// @brief Turns the snake right relative to its current direction.
/// @param snaken The snaken to apply the turn to.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_turn_right(snaken2d_t* snaken);

/// @brief Sets the snake speed in the provided snaken.
/// @param snaken The snaken to apply the new speed to.
/// @param speed The new speed to apply.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_set_snake_speed(snaken2d_t* snaken, snaken_snake_speed_t speed);

/// @brief Sets the snake stamina in the provided snaken.
/// @param snaken The snaken to apply the new stamina to.
/// @param stamina The new stamina to apply.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_set_snake_stamina(snaken2d_t* snaken, snaken_snake_stamina_t stamina);

/// @brief Sets the provided snaken world to have [apples_count] apples at any time step.
/// @param snaken The snaken to apply changes to.
/// @param apples_count The amount of apples to be present at any time in the snaken world.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_set_apples_count(snaken2d_t* snaken, snaken_world_size_t apples_count);

/// @brief Updates the location of the apple at the provided index while avoiding putting it on walls.
/// @param snaken The snaken to apply changes to.
/// @param index The index of the apple to update.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_spawn_apple(snaken2d_t* snaken, snaken_world_size_t index);

/// @brief Applies the provided walls to the provided snaken's world.
/// @param snaken The snaken to apply walls to.
/// @param walls_length The length of the walls array.
/// @param walls The array of walls.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
/// @warning The provided walls will overwrite any existing walls. Use [snaken2d_add_walls] to add them instead.
snaken_error_code_t snaken2d_set_walls(snaken2d_t* snaken, snaken_world_size_t walls_length, snaken_world_size_t* walls);


/// @brief Adds the provided walls to the existing walls in the provided snaken's world.
/// @param snaken The snaken to apply walls to.
/// @param walls_length The length of the walls array to add.
/// @param walls The array of walls to add.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
/// @warning The provided walls will not overwrite any existing walls. Use [snaken2d_set_walls] to overwrite them instead.
snaken_error_code_t snaken2d_add_walls(snaken2d_t* snaken, snaken_world_size_t walls_length, snaken_world_size_t* walls);


// ########################################## Util functions ##########################################

/// @brief Moves the snake along its facing direction.
/// @param snaken The snaken to compute the movement on.
/// @param result The resulting snake head location.
/// @return The code for the occurred error, [SNAKEN_ERROR_NONE] if none.
snaken_error_code_t snaken2d_move_snake(snaken2d_t* snaken);

snaken_error_code_t snaken2d_eat_apple(snaken2d_t* snaken, snaken_bool_t* result);

snaken_error_code_t snaken2d_hit_wall(snaken2d_t* snaken, snaken_bool_t* result);

snaken_error_code_t snaken2d_eat_body(snaken2d_t* snaken, snaken_bool_t* result);


#ifdef __cplusplus
}
#endif

#endif
