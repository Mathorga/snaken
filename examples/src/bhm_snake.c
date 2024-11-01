#include <stdio.h>
#include <snaken/snaken.h>
#include <behema/behema.h>

/// @brief Evaluates the provided cortex.
/// @param cortex The cortex to evaluate.
/// @param fitness The cortex fitness score as a result of the evaluation process.
/// @return The code for the occurred error, [BHM_ERROR_NONE] if none.
bhm_cortex_fitness_t eval_cortex(bhm_cortex2d_t* cortex, bhm_cortex_fitness_t fitness) {
   const int world_width = 30;
   const int world_height = 30;

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

   snaken_error = snaken2d_set_snake_stamina(snaken, 0xF0);
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
   bhm_error_code_t bhm_error;
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
   // Run evaluation.
   // ##########################################
   for (bhm_ticks_count_t i = 0; i < 10000; i++) {

   }
   // ##########################################
   // ##########################################

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
   // ##########################################
   // ##########################################


   // ##########################################
   // Evolve the population.
   // ##########################################
   for (uint16_t i = 0; i < 1000; i++) {
      bhm_error = p2d_evaluate(cortices_pop);
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error evaluating the cortices: %d\n", bhm_error);
         return 1;
      }
      bhm_error = p2d_select(cortices_pop);
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error selecting survivors: %d\n", bhm_error);
         return 1;
      }
      bhm_error = p2d_crossover(cortices_pop);
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error crossing survivors over: %d\n", bhm_error);
         return 1;
      }
      bhm_error = p2d_mutate(cortices_pop);
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error mutating the cortices: %d\n", bhm_error);
         return 1;
      }
   }
   // ##########################################
   // ##########################################


   // ##########################################
   // Cleanup.
   // ##########################################
   p2d_destroy(cortices_pop);
   // ##########################################
   // ##########################################

   return 0;
}
