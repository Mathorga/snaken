#include <stdio.h>
#include <raylib.h>
#include <snaken/snaken.h>

uint32_t map(uint32_t input, uint32_t input_start, uint32_t input_end, uint32_t output_start, uint32_t output_end) {
    uint32_t slope = (output_end - output_start) / (input_end - input_start);
    return output_start + slope * (input - input_start);
}

int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int world_width = 30;
    const int world_height = 30;

    const int cell_width = 10;
    const int cell_height = 10;

    int screen_width = world_width * cell_width;
    int screen_height = world_height * cell_height;

    Color snake_head_color = (Color) {0x7F, 0xFF, 0x55, 0xFF};
    Color snake_body_color = (Color) {0x00, 0x7F, 0x55, 0xFF};
    Color apple_color = (Color) {0xFF, 0x5A, 0x5A, 0xFF};
    Color wall_color = (Color) {0xFF, 0xFF, 0xFF, 0xFF};

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

    InitWindow(screen_width, screen_height, "Snake");

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

        // Tick the snaken.
        error = snaken2d_tick(snaken);
        if (error != SNAKEN_ERROR_NONE) {
            printf("ERROR\n");
            return 1;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            // Draw snake (tail to head in order to always show the head on top).
            for (int i = snaken->snake_length - 1; i >= 0; i--) {
                snaken_world_size_t section_location_x = snaken->snake_body[i] % snaken->world_width;
                snaken_world_size_t section_location_y = snaken->snake_body[i] / snaken->world_width;

                DrawRectangle(
                    section_location_x * cell_width,
                    section_location_y * cell_height,
                    cell_width, cell_height,
                    i == 0 ? snake_head_color : (Color) {
                        map(i, 0, snaken->snake_length - 1, snake_body_color.r, snake_head_color.r),
                        map(i, 0, snaken->snake_length - 1, snake_body_color.g, snake_head_color.g),
                        map(i, 0, snaken->snake_length - 1, snake_body_color.b, snake_head_color.b),
                        0xFF
                    }
                );
            }

            // Draw walls.
            for (int i = 0; i < snaken->walls_length; i++) {
                snaken_world_size_t wall_location_x = snaken->walls[i] % snaken->world_width;
                snaken_world_size_t wall_location_y = snaken->walls[i] / snaken->world_width;

                DrawRectangle(wall_location_x * cell_width, wall_location_y * cell_height, cell_width, cell_height, wall_color);
            }

            // Draw apples.
            for (int i = 0; i < snaken->apples_length; i++) {
                snaken_world_size_t apple_location_x = snaken->apples[i] % snaken->world_width;
                snaken_world_size_t apple_location_y = snaken->apples[i] / snaken->world_width;

                DrawRectangle(apple_location_x * cell_width, apple_location_y * cell_height, cell_width, cell_height, apple_color);
            }

            // Draw snake length.
            DrawText(TextFormat("Length: %i", snaken->snake_length), 10, 10, 20, WHITE);
            DrawText(TextFormat("speed: %i", snaken->snake_speed), 10, 30, 20, WHITE);
            DrawText(TextFormat("speed_step: %i", snaken->snake_speed_step), 10, 50, 20, WHITE);
            DrawText(TextFormat("stamina: %i", snaken->snake_stamina), 10, 70, 20, WHITE);
            DrawText(TextFormat("stamina_step: %i", snaken->snake_stamina_step), 10, 90, 20, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}
