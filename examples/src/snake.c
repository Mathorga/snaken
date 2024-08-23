#include <stdio.h>
#include <raylib.h>
#include <snaken/snaken.h>

int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int world_width = 20;
    const int world_height = 20;

    const int cell_width = 10;
    const int cell_height = 10;

    int screen_width = world_width * cell_width;
    int screen_height = world_height * cell_height;

    snaken_error_code_t error;

    snaken2d_t* snaken;
    error = snaken2d_init(&snaken, world_width, world_height);
    if (error != SNAKEN_ERROR_NONE) {
        printf("There was an error initializing the snaken: %d\n", error);
        return 1;
    }

    InitWindow(screen_width, screen_height, "Snake");

    SetTargetFPS(15);
    //---------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && snaken->snake_alive) {
        // Update
        //----------------------------------------------------------------------------------
        // Check for user input.
        switch(GetKeyPressed()) {
            case KEY_UP:
                snaken2d_set_snake_dir(snaken, SNAKEN_UP);
                break;
	    case KEY_LEFT:
                snaken2d_set_snake_dir(snaken, SNAKEN_LEFT);
                break;
            case KEY_DOWN:
                snaken2d_set_snake_dir(snaken, SNAKEN_DOWN);
                break;
            case KEY_RIGHT:
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
            for (snaken_world_size_t i = snaken->snake_length - 1; i >= 0; i--) {
                snaken_world_size_t i_x = snaken->snake_body[i] % snaken->world_width;
                snaken_world_size_t i_y = snaken->snake_body[i] / snaken->world_width;

                DrawRectangle(i_x * cell_width, i_y * cell_height, cell_width, cell_height, i == 0 ? YELLOW : GREEN);
            }

            // Draw apples.
            for (snaken_world_size_t i = 0; i < snaken->apples_length; i++) {
                snaken_world_size_t i_x = snaken->apples[i] % snaken->world_width;
                snaken_world_size_t i_y = snaken->apples[i] / snaken->world_width;

                DrawRectangle(i_x * cell_width, i_y * cell_height, cell_width, cell_height, RED);
                //DrawCircle(i_x * cell_width + cell_width / 2, i_y * cell_height + cell_height / 2, cell_width / 2, RED);
            }

            // Draw snake length.
            DrawText(TextFormat("Length: %i", snaken->snake_length), 10, 10, 20, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}
