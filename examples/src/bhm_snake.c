#include <stdio.h>
#include <snaken/snaken.h>
#include <behema/behema.h>

/// @brief Evaluates the provided cortex.
/// @param cortex The cortex to evaluate.
/// @param fitness The cortex fitness score as a result of the evaluation process.
/// @return The code for the occurred error, [BHM_ERROR_NONE] if none.
bhm_error_code_t eval_cortex(bhm_cortex2d_t* cortex, bhm_cortex_fitness_t* fitness) {
   const int world_width = 30;
   const int world_height = 30;


   // ##########################################
   // Init cortices.
   // ##########################################
   bhm_error_code_t bhm_error;

   bhm_cortex2d_t* tmp_cortex;
   bhm_error = c2d_init(&tmp_cortex, cortex->width, cortex->height, cortex->nh_radius);
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error allocating input %d\n", bhm_error);
      return bhm_error;
   }
   bhm_error = c2d_copy(tmp_cortex, cortex);
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error allocating input %d\n", bhm_error);
      return bhm_error;
   }
   // ##########################################
   // ##########################################


   // ##########################################
   // Init snaken.
   // ##########################################
   snaken_error_code_t snaken_error;

   snaken2d_t* snaken;
   snaken_error = snaken2d_init(&snaken, world_width, world_height);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error initializing the snaken: %d\n", snaken_error);
      return BHM_ERROR_EXTERNAL_CAUSES;
   }

   snaken_error = snaken2d_set_snake_speed(snaken, 0xF5);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error updating the snake speed: %d\n", snaken_error);
      return BHM_ERROR_EXTERNAL_CAUSES;
   }

   snaken_error = snaken2d_set_snake_stamina(snaken, 0xFF);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error updating the snake stamina: %d\n", snaken_error);
      return BHM_ERROR_EXTERNAL_CAUSES;
   }

   snaken_error = snaken2d_set_apples_count(snaken, 0x0F);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error setting the amount of apples: %d\n", snaken_error);
      return BHM_ERROR_EXTERNAL_CAUSES;
   }
   // ##########################################
   // ##########################################


   // ##########################################
   // Input init.
   // ##########################################
   snaken_world_size_t snaken_view_width = (snaken->snake_view_radius * 2) + 1;
   bhm_cortex_size_t input_width = snaken_view_width * snaken_view_width;
   bhm_input2d_t* input;
   bhm_error = i2d_init(
      &input,
      (cortex->width / 2) - (input_width / 2),
      0,
      (cortex->width / 2) + (input_width / 2),
      1,
      BHM_DEFAULT_EXC_VALUE * 2,
      BHM_PULSE_MAPPING_FPROP
   );
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error allocating input %d\n", bhm_error);
      return bhm_error;
   }
   // ##########################################
   // ##########################################


   // ##########################################
   // Output init.
   // ##########################################
   bhm_cortex_size_t output_width = 4;

   // The left output is in the bottom left part of the cortex.
   bhm_ticks_count_t mean_left_output = 0;
   bhm_output2d_t* left_output;
   bhm_error = o2d_init(
      &left_output,
      (cortex->width / 4) - (output_width / 2),
      cortex->height - 1,
      (cortex->width / 4) + (output_width / 2),
      cortex->height
   );
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error allocating left output %d\n", bhm_error);
      return bhm_error;
   }

   // The right output is in the bottom right part of the cortex.
   bhm_ticks_count_t mean_right_output = 0;
   bhm_output2d_t* right_output;
   bhm_error = o2d_init(
      &right_output,
      (cortex->width * 3 / 4) - (output_width / 2),
      cortex->height - 1,
      (cortex->width * 3 / 4) + (output_width / 2),
      cortex->height
   );
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error allocating right output %d\n", bhm_error);
      return bhm_error;
   }
   // ##########################################
   // ##########################################


   // ##########################################
   // Run evaluation.
   // ##########################################
   for (bhm_ticks_count_t i = 0; i < 10000; i++) {
      // Make sure the snake is still alive before going on.
      if (!snaken->snake_alive) break;

      // printf("Snake length: %d\n", snaken->snake_length);

      bhm_cortex2d_t* prev_cortex = i % 2 ? tmp_cortex : cortex;
      bhm_cortex2d_t* next_cortex = i % 2 ? cortex : tmp_cortex;

      // Get snake view as input.
      snaken_cell_type_t* view = (snaken_cell_type_t*) malloc(snaken_view_width * snaken_view_width * sizeof(snaken_cell_type_t));
      snaken_error = snaken2d_get_snake_view(snaken, view);
      if (snaken_error != SNAKEN_ERROR_NONE) {
         printf("There was an error retrieving the snake view: %d\n", snaken_error);
         return BHM_ERROR_EXTERNAL_CAUSES;
      }

      // Feed input to the cortex.
      for (bhm_cortex_size_t y = 0; y < input->y1 - input->y0; y++) {
         for (bhm_cortex_size_t x = 0; x < input->x1 - input->x0; x++) {
            input->values[IDX2D(x, y, input->x1 - input->x0)] = view[IDX2D(x, y, input->x1 - input->x0)];
         }
      }
      c2d_feed2d(prev_cortex, input);

      // Tick the cortex.
      c2d_tick(prev_cortex, next_cortex);

      // Tick the snaken.
      snaken_error = snaken2d_tick(snaken);
      if (snaken_error != SNAKEN_ERROR_NONE) {
         printf("There was an error running the snaken: %d\n", snaken_error);
         return BHM_ERROR_EXTERNAL_CAUSES;
      }

      // Read cortex output.
      c2d_read2d(next_cortex, left_output);
      c2d_read2d(next_cortex, right_output);
      o2d_mean(left_output, &mean_left_output);
      o2d_mean(left_output, &mean_right_output);

      // printf("%d|%d\n", mean_left_output, mean_right_output);

      // Use cortex output to control the snake.
      if (mean_left_output > mean_right_output) {
         printf("turning left\n");
         snaken2d_turn_left(snaken);
      } else if (mean_right_output > mean_left_output) {
         printf("turning right\n");
         snaken2d_turn_right(snaken);
      }
   }
   // ##########################################
   // ##########################################

   *fitness = snaken->snake_length;
   printf("Evaluated the cortex: %d\n", *fitness);

   return BHM_ERROR_NONE;
}


int main(void) {
   // ##########################################
   // Init cortices population.
   // ##########################################
   bhm_error_code_t bhm_error;
   bhm_population2d_t* cortices_pop;
   bhm_error = p2d_init(
      &cortices_pop,
      200,
      20,
      0x0000FF,
      &eval_cortex
   );
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error initializing the population: %d\n", bhm_error);
      return 1;
   }
   bhm_error = p2d_populate(cortices_pop, 64, 48, 2);
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error population the cortices: %d\n", bhm_error);
      return 1;
   }
   // ##########################################
   // ##########################################


   // ##########################################
   // Evolve the population.
   // ##########################################
   for (uint16_t i = 0; i < 1000; i++) {
      printf("Evaluation %d\n", i);
      bhm_error = p2d_evaluate(cortices_pop);
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error evaluating the cortices: %d\n", bhm_error);
         return 1;
      }
      printf("Selection %d\n", i);
      bhm_error = p2d_select(cortices_pop);
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error selecting survivors: %d\n", bhm_error);
         return 1;
      }
      printf("Crossover %d\n", i);
      bhm_error = p2d_crossover(cortices_pop, BHM_TRUE);
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error crossing survivors over: %d\n", bhm_error);
         return 1;
      }
   }
   // ##########################################
   // ##########################################


   // ##########################################
   // Cleanup.
   // ##########################################
   // p2d_destroy(cortices_pop);
   // ##########################################
   // ##########################################

   return 0;
}
