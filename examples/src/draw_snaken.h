#include <raylib.h>
#include <snaken/snaken.h>
#include <snaken/utils.h>

snaken_error_code_t draw_snaken(
    snaken2d_t* snaken,
    int window_width,
    int window_height
) {
    const int cell_width = window_width / snaken->world_width;
    const int cell_height = window_height / snaken->world_height;
    const Color snake_head_color = (Color) {0x7F, 0xFF, 0x55, 0xFF};
    const Color snake_tail_color = (Color) {0x11, 0x33, 0x44, 0xFF};
    const Color apple_color = (Color) {0xFF, 0x5A, 0x5A, 0xFF};
    const Color wall_color = (Color) {0xFF, 0xFF, 0xFF, 0xFF};

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

    const int font_size = 20;
    const int text_padding_x = 8;
    const int text_padding_y = 8;

    // Draw snake length.
    DrawText(
        TextFormat("Length: %i", snaken->snake_length),
        text_padding_x,
        text_padding_y,
        font_size,
        RAYWHITE
    );
    DrawText(
        TextFormat("speed: %i", snaken->snake_speed),
        text_padding_x,
        text_padding_y + font_size,
        font_size,
        RAYWHITE
    );
    DrawText(
        TextFormat("speed_step: %i", snaken->snake_speed_step),
        text_padding_x,
        text_padding_y + font_size * 2,
        font_size,
        RAYWHITE
    );
    DrawText(
        TextFormat("stamina: %i", snaken->snake_stamina),
        text_padding_x,
        text_padding_y + font_size * 3,
        font_size,
        RAYWHITE
    );
    DrawText(
        TextFormat("stamina_step: %i", snaken->snake_stamina_step),
        text_padding_x,
        text_padding_y + font_size * 4,
        font_size,
        RAYWHITE
    );

    return SNAKEN_ERROR_NONE;
}