#include "snaken.h"


// ########################################## Initialization functions ##########################################

snaken_error_code_t snaken2d_init(snaken2d_t** snaken, snaken_world_size_t world_width, snaken_world_size_t world_height) {
    // Allocate the snaken.
    (*snaken) = (snaken2d_t*) malloc(sizeof(snaken2d_t));
    if ((*snaken) == NULL) {
        return ERROR_FAILED_ALLOC;
    }

    // Allocate the world.
    (*snaken)->world_width = world_width;
    (*snaken)->world_height = world_height;
    (*snaken)->world = (snaken_cell_type_t*) malloc((*snaken)->world_width * (*snaken)->world_height * sizeof(snaken_cell_type_t));
    if ((*snaken)->world == NULL) {
        return ERROR_FAILED_ALLOC;
    }

    // Allocate walls helper.
    (*snaken)->walls_length = 0;
    (*snaken)->walls = (snaken_world_size_t*) malloc((*snaken)->walls_length * sizeof(snaken_world_size_t));
    if ((*snaken)->walls == NULL) {
        return ERROR_FAILED_ALLOC;
    }

    // Allocate apples helper.
    (*snaken)->apples_length = 0;
    (*snaken)->apples = (snaken_world_size_t*) malloc((*snaken)->apples_length * sizeof(snaken_world_size_t));
    if ((*snaken)->apples == NULL) {
        return ERROR_FAILED_ALLOC;
    }

    // Allocate snake body helper.
    (*snaken)->snake_length = STARTING_SNAKE_LENGTH;
    (*snaken)->snake_body = (snaken_world_size_t*) malloc((*snaken)->snake_length * sizeof(snaken_world_size_t));
    if ((*snaken)->snake_body == NULL) {
        return ERROR_FAILED_ALLOC;
    }

    (*snaken)->snake_speed = DEFAULT_SNAKE_SPEED;
    (*snaken)->snake_direction = STARTING_SNAKE_DIR;
    (*snaken)->self_intersection = FALSE;
    (*snaken)->snake_alive = TRUE;
    (*snaken)->snake_view_radius = DEFAULT_SNAKE_VIEW_RADIUS;

    return ERROR_NONE;
}


// ########################################## Execution functions ##########################################

snaken_error_code_t snaken2d_update_content(snaken2d_t* snaken) {
    snaken_error_code_t error = ERROR_NONE;

    // 1: Move the snake along its facing direction.
    error = snaken2d_move_snake(snaken);
    if (error != ERROR_NONE) {
        return error;
    }

    // 2: Let the snake eat any apple in its way.
    snaken_bool_t apple_found = FALSE;
    error = snaken2d_eat_apple(snaken, &apple_found);
    if (error != ERROR_NONE) {
        return error;
    }

    // If any apple was found, then no wall can, so just end here.
    if (apple_found) {
        return ERROR_NONE;
    }

    // 3: Check for walls.
    snaken_bool_t wall_found = FALSE;
    error = snaken2d_hit_wall(snaken, &wall_found);
    if (error != ERROR_NONE) {
        return error;
    }

    if (wall_found) {
        return ERROR_NONE;
    }

    // 4: Check for body if so specified.
    snaken_bool_t body_found = FALSE;
    error = snaken2d_eat_body(snaken, &body_found);
    if (error != ERROR_NONE) {
        return error;
    }

    return ERROR_NONE;
}

snaken_error_code_t snaken2d_update_workd(snaken2d_t* snaken) {
    // Loop through the whole world and update each cell according to the current snaken content (snake, apples, walls) state.
    for (snaken_world_size_t y = 0; y < snaken->world_height; y++) {
        for (snaken_world_size_t x = 0; x < snaken->world_width; x++) {
            
        }
    }

    return ERROR_NONE;
}

snaken_error_code_t snaken2d_tick(snaken2d_t* snaken) {
    snaken_error_code_t error = ERROR_NONE;

    // Move the snake one step and check for apples/walls/body along the way.
    error = snaken2d_update_content(snaken);
    if (error != ERROR_NONE) {
        return error;
    }

    // Update the world view matrix according to the updated content.
    error = snaken2d_update_workd(snaken);
    if (error != ERROR_NONE) {
        return error;
    }

    return ERROR_NONE;
}


// ########################################## Getter functions ##########################################

snaken_error_code_t snaken2d_get_snake_view(snaken_world_size_t* view, snaken2d_t* snaken) {
    // TODO.
    return ERROR_NONE;
}


// ########################################## Setter functions ##########################################

snaken_error_code_t snaken2d_set_snake_dir(snaken2d_t* snaken, snaken_dir_t direction) {
    snaken->snake_direction = direction;

    return ERROR_NONE;
}


// ########################################## Util functions ##########################################

snaken_error_code_t snaken2d_move_snake(snaken2d_t* snaken) {
    // Save the previous head location in order to move its neck to it.
    snaken_world_size_t section_location = snaken->snake_body[0];

    // Update the snake head.
    switch (snaken->snake_direction) {
        case UP:
            snaken->snake_body[0] -= snaken->world_width;
            break;
        case LEFT:
            snaken->snake_body[0] -= 1;
            break;
        case DOWN:
            snaken->snake_body[0] += snaken->world_width;
            break;
        case RIGHT:
            snaken->snake_body[0] += 1;
            break;
        default:
            break;
    }

    // Update the snake body accordingly without reallocating it.
    for (snaken_world_size_t i = 1; i < snaken->snake_length; i++) {
        // Save the current snake body section location.
        section_location = snaken->snake_body[i];

        // Update the body section.
        snaken->snake_body[i] = snaken->snake_body[i - 1];
    }

    return ERROR_NONE;
}

snaken_error_code_t snaken2d_eat_apple(snaken2d_t* snaken, snaken_bool_t* result) {
    result = FALSE;

    for (snaken_world_size_t i = 0; i < snaken->apples_length; i++) {
        // The snake head can be at most on one apple, so leave as soon as one is found.
        if (snaken->snake_body[0] == snaken->apples[i]) {
            // An apple was found, so eat it and increase the snake length:
            result = TRUE;

            // Eat the apple.
            snaken->apples_length--;
            snaken_world_size_t* new_apples = realloc(snaken->apples, snaken->apples_length);
            if (new_apples == NULL) {
                return ERROR_FAILED_ALLOC;
            }
            free(snaken->apples);
            snaken->apples = new_apples;

            // Increase the snake length.
            snaken->snake_length++;
            snaken_world_size_t* new_snake_body = realloc(snaken->snake_body, snaken->snake_length);
            if (new_snake_body == NULL) {
                return ERROR_FAILED_ALLOC;
            }
            free(snaken->snake_body);
            snaken->snake_body = new_snake_body;

            break;
        }
    }

    return ERROR_NONE;
}

snaken_error_code_t snaken2d_hit_wall(snaken2d_t* snaken, snaken_bool_t* result) {
    result = FALSE;

    for (snaken_world_size_t i = 0; i < snaken->walls_length; i++) {
        // The snake head can be at most on one wall, so leave as soon as one is found.
        if (snaken->snake_body[0] == snaken->walls[i]) {
            // A wall was found, so hit it and let the snake die:
            result = TRUE;

            // Let the snake die.
            snaken->snake_alive = FALSE;

            break;
        }
    }

    return ERROR_NONE;
}

snaken_error_code_t snaken2d_eat_body(snaken2d_t* snaken, snaken_bool_t* result) {
    result = FALSE;

    // Start from 1 since the first element is actually the snake head.
    for (snaken_world_size_t i = 1; i < snaken->snake_length; i++) {
        // The snake head can be at most on one wall, so leave as soon as one is found.
        if (snaken->snake_body[0] == snaken->snake_body[i]) {
            // A wall was found, so hit it and let the snake die:
            result = TRUE;

            // Let the snake die.
            snaken->snake_alive = FALSE;

            break;
        }
    }

    return ERROR_NONE;
}