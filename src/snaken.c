#include "snaken.h"


// ########################################## Initialization functions ##########################################

snaken_error_code_t snaken2d_init(snaken2d_t** snaken, snaken_world_size_t world_width, snaken_world_size_t world_height) {
    // Allocate the snaken.
    (*snaken) = (snaken2d_t*) malloc(sizeof(snaken2d_t));
    if ((*snaken) == NULL) {
        return SNAKEN_ERROR_FAILED_ALLOC;
    }

    // Store world size.
    (*snaken)->world_width = world_width;
    (*snaken)->world_height = world_height;

    // Allocate walls.
    (*snaken)->walls_length = 0;
    (*snaken)->walls = (snaken_world_size_t*) malloc((*snaken)->walls_length * sizeof(snaken_world_size_t));
    if ((*snaken)->walls == NULL) {
        return SNAKEN_ERROR_FAILED_ALLOC;
    }

    // Allocate apples.
    (*snaken)->apples_length = DEFAULT_APPLES_LENGTH;
    (*snaken)->apples = (snaken_world_size_t*) malloc((*snaken)->apples_length * sizeof(snaken_world_size_t));
    if ((*snaken)->apples == NULL) {
        return SNAKEN_ERROR_FAILED_ALLOC;
    }

    // Populate apples.
    for (snaken_world_size_t i = 0; i < (*snaken)->apples_length; i++) {
        snaken_world_size_t apple_x = rand() % world_width;
        snaken_world_size_t apple_y = rand() & world_height;

        (*snaken)->apples[i] = IDX2D(apple_x, apple_y, world_width);
    }

    // Allocate snake body.
    (*snaken)->snake_length = STARTING_SNAKE_LENGTH;
    (*snaken)->snake_body = (snaken_world_size_t*) malloc((*snaken)->snake_length * sizeof(snaken_world_size_t));
    if ((*snaken)->snake_body == NULL) {
        return SNAKEN_ERROR_FAILED_ALLOC;
    }

    // Populate the snake head.
    (*snaken)->snake_body[0] = IDX2D(world_width / 2, world_height / 2, world_width);

    // Populate the snake body.
    for (snaken_world_size_t i = 1; i < (*snaken)->snake_length; i++) {
        (*snaken)->snake_body[i] = (*snaken)->snake_body[0];
    }

    (*snaken)->snake_speed = DEFAULT_SNAKE_SPEED;
    (*snaken)->snake_speed_step = 0;
    (*snaken)->snake_stamina = DEFAULT_SNAKE_STAMINA;
    (*snaken)->snake_stamina_step = 0;
    (*snaken)->snake_direction = STARTING_SNAKE_DIR;
    (*snaken)->self_intersection = SNAKEN_FALSE;
    (*snaken)->snake_alive = SNAKEN_TRUE;
    (*snaken)->snake_view_radius = DEFAULT_SNAKE_VIEW_RADIUS;

    return SNAKEN_ERROR_NONE;
}


// ########################################## Execution functions ##########################################

snaken_error_code_t snaken2d_tick(snaken2d_t* snaken) {
    snaken_error_code_t error = SNAKEN_ERROR_NONE;

    // 1: Move the snake along its facing direction.
    error = snaken2d_move_snake(snaken);
    if (error != SNAKEN_ERROR_NONE) {
        return error;
    }

    // 2: Let the snake eat any apple in its way.
    snaken_bool_t apple_found = SNAKEN_FALSE;
    error = snaken2d_eat_apple(snaken, &apple_found);
    if (error != SNAKEN_ERROR_NONE) {
        return error;
    }

    // If any apple was found, then no wall can, so just end here.
    if (apple_found) {
        return SNAKEN_ERROR_NONE;
    }

    // 3: Check for walls.
    snaken_bool_t wall_found = SNAKEN_FALSE;
    error = snaken2d_hit_wall(snaken, &wall_found);
    if (error != SNAKEN_ERROR_NONE) {
        return error;
    }

    if (wall_found) {
        return SNAKEN_ERROR_NONE;
    }

    // 4: Check for body if so specified.
    snaken_bool_t body_found = SNAKEN_FALSE;
    error = snaken2d_eat_body(snaken, &body_found);
    if (error != SNAKEN_ERROR_NONE) {
        return error;
    }

    // 5: Check for hunger.
    snaken->snake_stamina_step++;
    if (snaken->snake_stamina_step > snaken->snake_stamina) {
        // Reset hunger.
        snaken->snake_stamina_step = 0;

        // Decrease the snake length.
        snaken->snake_length--;
        snaken->snake_body = (snaken_world_size_t*) realloc(snaken->snake_body, snaken->snake_length * sizeof(snaken_world_size_t));
        if (snaken->snake_body == NULL) {
            return SNAKEN_ERROR_FAILED_ALLOC;
        }

        // Let the snake die of hunger.
        if (snaken->snake_length <= 0) {
            snaken->snake_alive = SNAKEN_FALSE;
        }
    }

    return SNAKEN_ERROR_NONE;
}

snaken_error_code_t snaken2d_spawn_apple(snaken2d_t* snaken) {
    // TODO.
    return SNAKEN_ERROR_NONE;
}


// ########################################## Getter functions ##########################################

snaken_error_code_t snaken2d_get_snake_view(snaken2d_t* snaken, snaken_cell_type_t* view) {
    snaken_world_size_t snake_view_diameter = NH_DIAM_2D(snaken->snake_view_radius);

    // Populate the resulting snake view.
    for (snaken_world_size_t j = 0; j < snake_view_diameter; j++) {
        // Compute world-space j.
        snaken_world_size_t global_j = snaken->snake_body[0] - j - snaken->snake_view_radius;
        for (snaken_world_size_t i = 0; i < snake_view_diameter; i++) {
            // Compute world-space i.
            snaken_world_size_t global_i = snaken->snake_body[0] - i - snaken->snake_view_radius;

            snaken_world_size_t local_location = IDX2D(i, j, snake_view_diameter);
            snaken_world_size_t global_location = IDX2D(global_i, global_j, snaken->world_width);

            // Prepopulate with empty space.
            view[local_location] = SNAKEN_EMPTY;

            // Check for snake head.
            if (snaken->snake_body[0] == global_location) {
                view[local_location] = SNAKEN_SNAKE_HEAD;
                continue;
            }

            // Check for snake body.
            snaken_bool_t snake_found = SNAKEN_FALSE;
            for (snaken_world_size_t k = 1; k < snaken->snake_length; k++) {
                if (snaken->snake_body[k] == global_location) {
                    view[local_location] = SNAKEN_SNAKE_BODY;
                    snake_found = SNAKEN_TRUE;
                    break;
                }
            }
            if (snake_found) {
                continue;
            }

            // Check for apples.
            snaken_bool_t apple_found = SNAKEN_FALSE;
            for (snaken_world_size_t k = 0; k < snaken->apples_length; k++) {
                if (snaken->apples[k] == global_location) {
                    view[local_location] = SNAKEN_APPLE;
                    apple_found = SNAKEN_TRUE;
                    break;
                }
            }
            if (apple_found) {
                continue;
            }

            // Check for walls.
            snaken_bool_t wall_found = SNAKEN_FALSE;
            for (snaken_world_size_t k = 0; k < snaken->walls_length; k++) {
                if (snaken->walls[k] == global_location) {
                    view[local_location] = SNAKEN_WALL;
                    wall_found = SNAKEN_TRUE;
                    break;
                }
            }
            if (wall_found) {
                continue;
            }
        }
    }
    return SNAKEN_ERROR_NONE;
}


// ########################################## Setter functions ##########################################

snaken_error_code_t snaken2d_set_snake_dir(snaken2d_t* snaken, snaken_dir_t direction) {
    snaken->snake_direction = direction;

    return SNAKEN_ERROR_NONE;
}

snaken_error_code_t snaken2d_set_snake_speed(snaken2d_t* snaken, snaken_snake_speed_t speed) {
    snaken->snake_speed = speed;

    return SNAKEN_ERROR_NONE;
}

snaken_error_code_t snaken2d_set_snake_stamina(snaken2d_t* snaken, snaken_snake_stamina_t stamina) {
    snaken->snake_stamina = stamina;

    return SNAKEN_ERROR_NONE;
}

snaken_error_code_t snaken2d_set_apples_count(snaken2d_t* snaken, snaken_world_size_t apples_count) {
    // Save the old count for later use.
    snaken_world_size_t old_apples_count = snaken->apples_length;

    // Resize the apples array.
    snaken->apples_length = apples_count;
    snaken->apples = (snaken_world_size_t*) realloc(snaken->apples, snaken->apples_length * sizeof(snaken_world_size_t));
    if (snaken->apples == NULL) {
        return SNAKEN_ERROR_FAILED_ALLOC;
    }

    // If the amount of apples increased, then spawn new ones.
    for (snaken_world_size_t i = old_apples_count; i < snaken->apples_length; i++) {
        snaken_world_size_t apple_x = rand() % snaken->world_width;
        snaken_world_size_t apple_y = rand() & snaken->world_height;

        snaken->apples[i] = IDX2D(apple_x, apple_y, snaken->world_width);
    }

    return SNAKEN_ERROR_NONE;
}

snaken_error_code_t snaken2d_set_walls(snaken2d_t* snaken, snaken_world_size_t walls_length, snaken_world_size_t* walls) {
    // TODO.
    return SNAKEN_ERROR_NONE;
}


// ########################################## Util functions ##########################################

snaken_error_code_t snaken2d_move_snake(snaken2d_t* snaken) {
    snaken->snake_speed_step++;

    if (snaken->snake_speed_step < (snaken_snake_speed_t) (~snaken->snake_speed)) {
        return SNAKEN_ERROR_NONE;
    }

    // Reset speed buildup and then move the snake.
    snaken->snake_speed_step = 0;

    // Save the previous head location in order to move its neck to it.
    snaken_world_size_t section_location = snaken->snake_body[0];

    // Compute the x and y commponents of the head position.
    snaken_world_size_t x_location = snaken->snake_body[0] % snaken->world_width;
    snaken_world_size_t y_location = snaken->snake_body[0] / snaken->world_width;

    // Update the snake head.
    switch (snaken->snake_direction) {
        case SNAKEN_UP:
            snaken->snake_body[0] = IDX2D(x_location, WRAP(y_location - 1, snaken->world_height), snaken->world_width);
            break;
        case SNAKEN_LEFT:
            snaken->snake_body[0] = IDX2D(WRAP(x_location - 1, snaken->world_width), y_location, snaken->world_width);
            break;
        case SNAKEN_DOWN:
            snaken->snake_body[0] = IDX2D(x_location, WRAP(y_location + 1, snaken->world_height), snaken->world_width);
            break;
        case SNAKEN_RIGHT:
            snaken->snake_body[0] = IDX2D(WRAP(x_location + 1, snaken->world_width), y_location, snaken->world_width);
            break;
        default:
            break;
    }

    // Update the snake body accordingly without reallocating it.
    for (snaken_world_size_t i = 1; i < snaken->snake_length; i++) {
        // Temporarily save the current body section location.
        snaken_world_size_t old_location = snaken->snake_body[i];

        // Update the body section location with the old location of the last visited body section.
        snaken->snake_body[i] = section_location;

        // Save the current snake body section location for the next body section.
        section_location = old_location;
    }

    return SNAKEN_ERROR_NONE;
}

snaken_error_code_t snaken2d_eat_apple(snaken2d_t* snaken, snaken_bool_t* result) {
    (*result) = SNAKEN_FALSE;

    for (snaken_world_size_t i = 0; i < snaken->apples_length; i++) {
        // The snake head can be at most on one apple, so leave as soon as one is found.
        if (snaken->snake_body[0] == snaken->apples[i]) {
            // An apple was found, so eat it and increase the snake length:
            (*result) = SNAKEN_TRUE;

            // Eat the apple and spawn a new one.
            snaken_world_size_t new_apple_x = rand() % snaken->world_width;
            snaken_world_size_t new_apple_y = rand() % snaken->world_height;
            snaken->apples[i] = IDX2D(new_apple_x, new_apple_y, snaken->world_width);

            // Increase the snake length.
            snaken->snake_length++;
            snaken->snake_body = (snaken_world_size_t*) realloc(snaken->snake_body, snaken->snake_length * sizeof(snaken_world_size_t));
            if (snaken->snake_body == NULL) {
                return SNAKEN_ERROR_FAILED_ALLOC;
            }

            break;
        }
    }

    return SNAKEN_ERROR_NONE;
}

snaken_error_code_t snaken2d_hit_wall(snaken2d_t* snaken, snaken_bool_t* result) {
    (*result) = SNAKEN_FALSE;

    for (snaken_world_size_t i = 0; i < snaken->walls_length; i++) {
        // The snake head can be at most on one wall, so leave as soon as one is found.
        if (snaken->snake_body[0] == snaken->walls[i]) {
            // A wall was found, so hit it and let the snake die:
            (*result) = SNAKEN_TRUE;

            // Let the snake die.
            snaken->snake_alive = SNAKEN_FALSE;

            break;
        }
    }

    return SNAKEN_ERROR_NONE;
}

snaken_error_code_t snaken2d_eat_body(snaken2d_t* snaken, snaken_bool_t* result) {
    (*result) = SNAKEN_FALSE;

    // Make sure no check is performed if so specified.
    if (!snaken->self_intersection) {
        return SNAKEN_ERROR_NONE;
    }

    // Start from 1 since the first element is actually the snake head.
    for (snaken_world_size_t i = 1; i < snaken->snake_length; i++) {
        // The snake head can be at most on one wall, so leave as soon as one is found.
        if (snaken->snake_body[0] == snaken->snake_body[i]) {
            // A wall was found, so hit it and let the snake die:
            (*result) = SNAKEN_TRUE;

            // Let the snake die.
            snaken->snake_alive = SNAKEN_FALSE;

            break;
        }
    }

    return SNAKEN_ERROR_NONE;
}
