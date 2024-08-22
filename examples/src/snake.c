#include <raylib.h>
#include <snaken/snaken.h>

int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    //const int screen_width = 400;
    //const int screen_height = 250;

    const int world_width = 20;
    const int world_height = 20;

    const int cell_width = 10;
    const int cell_height = 10;

    const int screen_width = world_width * cell_width;
    const int screen_height = world_height * cell_height;

    snaken_error_code_t error;

    snaken2d_t* snaken;
    error = snaken2d_init(&snaken);
    if (error != SNAKEN_ERROR_NONE) {
        printf("There was an error initializing the snaken: %d\n", error);
    }

    InitWindow(screen_width, screen_height, "Snake");

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            // Draw snake.
            for (snaken_world_size_t i = 0; i < snaken->snake_length; i++) {
                snaken_world_size_t i_x = i / snaken->world_width;
                snaken_world_size_t i_y = i % snaken->world_width;

                DrawRectangle(i_x * cell_width, i_y * cell_height, cell_width, cell_height, RED);
            }
            //DrawRectangle(20, 20, 20, 20, RED);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}
