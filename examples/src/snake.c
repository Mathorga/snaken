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
            for (int j = 0; j < world_height; j++) {
                for (int i = 0; i < world_width; i++) {
                    DrawRectangle(i * cell_width + 1, j * cell_height + 1, cell_width - 2, cell_height - 2, RED);
                }
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
