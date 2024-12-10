#include "graphics.h"

int snaken_graphics_window_width = 10;
int snaken_graphics_window_height = 10;

snaken_error_code_t snaken_graphics_begin(
    int window_width,
    int window_height
) {
    snaken_graphics_window_width = window_width;
    snaken_graphics_window_height = window_height;
    InitWindow(window_width, window_width, "Snaken");
    return SNAKEN_ERROR_NONE;
}

snaken_error_code_t snaken_graphics_draw(
    snaken2d_t* snaken
) {
    const int cell_width = snaken_graphics_window_width / snaken->world_width;
    const int cell_height = snaken_graphics_window_height / snaken->world_height;
    Color snake_head_color = (Color) {0x7F, 0xFF, 0x55, 0xFF};
    Color snake_tail_color = (Color) {0x11, 0x33, 0x44, 0xFF};
    Color apple_color = (Color) {0xFF, 0x5A, 0x5A, 0xFF};
    Color wall_color = (Color) {0xFF, 0xFF, 0xFF, 0xFF};

    BeginDrawing();

        ClearBackground(BLACK);

        // Draw snake (tail to head in order to always show the head on top).
        for (int i = snaken->snake_length - 1; i >= 0; i--) {
            snaken_world_size_t section_location_x = snaken->snake_body[i] % snaken->world_width;
            snaken_world_size_t section_location_y = snaken->snake_body[i] / snaken->world_width;

            double segment_position = ((double) i) / (snaken->snake_length - 1);

            Color segment_color = (Color) {
                lerp(snake_head_color.r, snake_tail_color.r, segment_position),
                lerp(snake_head_color.g, snake_tail_color.g, segment_position),
                lerp(snake_head_color.b, snake_tail_color.b, segment_position),
                0xFF
            };

            DrawRectangle(
                section_location_x * cell_width,
                section_location_y * cell_height,
                cell_width, cell_height,
                segment_color
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

    return SNAKEN_ERROR_NONE;
}

snaken_error_code_t snaken_graphics_end() {
    CloseWindow();
    return SNAKEN_ERROR_NONE;
}