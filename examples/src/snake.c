#include <stdio.h>
#include <snaken/snaken.h>
#include <snaken/graphics.h>

char cell_type_to_char(snaken_cell_type_t cell_type) {
    switch (cell_type) {
        case SNAKEN_SNAKE_HEAD:
            return 'O';
        case SNAKEN_SNAKE_BODY:
            return 'o';
        case SNAKEN_APPLE:
            return '@';
        case SNAKEN_WALL:
            return 'H';
        case SNAKEN_EMPTY:
        default:
            return '+';
    }
}

void print_snake_view(snaken_cell_type_t* view, snaken_world_size_t view_diameter) {
    for (snaken_world_size_t y = view_diameter - 1; y >= 0; y--) {
    // for (snaken_world_size_t y = 0; y < view_diameter; y++) {
        for (snaken_world_size_t x = view_diameter - 1; x >= 0; x--) {
        // for (snaken_world_size_t x = 0; x < view_diameter; x++) {
            printf("%c ", cell_type_to_char(view[IDX2D(x, y, view_diameter)]));
        }
        printf("\n");
    }
    printf("\n");
}

int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int world_width = 30;
    const int world_height = 30;

    int screen_width = world_width * 10;
    int screen_height = world_height * 10;

    snaken_error_code_t error;

    snaken2d_t* snaken;
    error = snaken2d_init(&snaken, world_width, world_height);
    if (error != SNAKEN_ERROR_NONE) {
        printf("There was an error initializing the snaken: %d\n", error);
        return 1;
    }

    error = snaken2d_set_snake_speed(snaken, 0xF5);
    if (error != SNAKEN_ERROR_NONE) {
       printf("There was an error updating the snake speed: %d\n", error);
       return 1;
    }

    error = snaken2d_set_snake_stamina(snaken, 0xF0);
    if (error != SNAKEN_ERROR_NONE) {
       printf("There was an error updating the snake stamina: %d\n", error);
       return 1;
    }

    error = snaken2d_set_apples_count(snaken, 0x0F);
    if (error != SNAKEN_ERROR_NONE) {
       printf("There was an error setting the amount of apples: %d\n", error);
       return 1;
    }
    snaken_world_size_t snaken_view_width = NH_DIAM_2D(snaken->snake_view_radius);
    snaken_cell_type_t* view = (snaken_cell_type_t*) malloc(snaken_view_width * snaken_view_width * sizeof(snaken_cell_type_t));

    // Set up some walls.
    snaken_world_size_t* walls = (snaken_world_size_t*) malloc(10 * sizeof(snaken_world_size_t));
    if (walls == NULL) {
        printf("ERROR allocating walls\n");
        return 1;
    }
    for (snaken_world_size_t i = 0; i < 10; i++) {
        walls[i] = IDX2D(i, 0, snaken->world_width);
    }
    snaken2d_set_walls(snaken, 10, walls);

    snaken_graphics_begin(
        screen_width,
        screen_height
    );

    SetTargetFPS(60);
    //---------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && snaken->snake_alive) {
        // Update
        //----------------------------------------------------------------------------------
        // Check for user input.
        switch(GetKeyPressed()) {
            case KEY_UP:
            case KEY_W:
                snaken2d_set_snake_dir(snaken, SNAKEN_UP);
                break;
            case KEY_LEFT:
            case KEY_A:
                snaken2d_set_snake_dir(snaken, SNAKEN_LEFT);
                break;
            case KEY_DOWN:
            case KEY_S:
                snaken2d_set_snake_dir(snaken, SNAKEN_DOWN);
                break;
            case KEY_RIGHT:
            case KEY_D:
                snaken2d_set_snake_dir(snaken, SNAKEN_RIGHT);
                break;
            default:
                break;
        }
        error = snaken2d_get_snake_view(snaken, view);
        if (error != SNAKEN_ERROR_NONE) {
            printf("There was an error retrieving the snake view: %d\n", error);
            return 1;
        }
        print_snake_view(view, snaken_view_width);

        // Tick the snaken.
        error = snaken2d_tick(snaken);
        if (error != SNAKEN_ERROR_NONE) {
            printf("ERROR\n");
            return 1;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        snaken_graphics_draw(snaken);
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    snaken_graphics_end();
    //--------------------------------------------------------------------------------------

    return 0;
}
