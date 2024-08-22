#include <raylib.h>
#include <snaken/snaken.h>

int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screen_width = 400;
    const int screen_height = 250;

    const int world_width = 20;
    const int world_height = 20;

    const float cell_width = screen_width / world_width;
    const float cell_height = screen_height / world_height;

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
            DrawRectangle(20, 20, 20, 20, RED);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}
