#define GRAPHICS
// #define PLOT

#include <stdio.h>
#include <getopt.h>
#include <snaken/snaken.h>
#include <behema/behema.h>

#ifdef GRAPHICS
#include "draw_snaken.h"
#include "draw_cortex.h"
#endif

#define POP_SIZE 20
#define MAX_EVAL_TIME 10000
#define GENERATIONS_COUNT 10000

#define WORLD_WIDTH 30
#define WORLD_HEIGHT 30

#define CORTICES_WIDTH 20
#define CORTICES_HEIGHT 4
#define CORTICES_NH_RADIUS 2
#define POP_MUT_CHANCE 0x0FFFFFFFU

#ifdef GRAPHICS
#define WINDOW_WIDTH WORLD_WIDTH * 20
#define WINDOW_HEIGHT WORLD_HEIGHT * 20
#endif

int clamp(int d, int min, int max) {
   const int t = d < min ? min : d;
   return t > max ? max : t;
}

/// @brief Maps the provided snake view to an array of pulses.
/// @param snake_view The snake view to map.
/// @param sample_window The window the view needs to be mapped to.
/// @return The mapped snake view.
bhm_ticks_count_t snake_view_to_pulse(
   snaken_cell_type_t snake_view,
   bhm_ticks_count_t sample_window
) {
   int damping = 10;
   // 4 is the max possible snaken_cell_type_t value.
   // printf("sample_window: %d, mapping: %d\n", sample_window, (sample_window / (4 + damping)) * (snake_view + damping));
   return (sample_window / (4 + damping)) * (snake_view + damping);
}

char cell_type_to_char(snaken_cell_type_t cell_type) {
    switch (cell_type) {
        case SNAKEN_SNAKE_HEAD:
            return 'X';
        case SNAKEN_SNAKE_BODY:
            return 'x';
        case SNAKEN_APPLE:
            return '@';
        case SNAKEN_WALL:
            return 'H';
        case SNAKEN_EMPTY:
        default:
            return '.';
    }
}

void print_snake_view(
   snaken_cell_type_t* view,
   snaken_world_size_t view_diameter
) {
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

bhm_error_code_t dummy_eval(
   bhm_cortex2d_t* cortex,
   bhm_cortex_fitness_t* fitness
) {
   *fitness = 0x00;
   return BHM_ERROR_NONE;
}

snaken_error_code_t create_snaken(
   snaken2d_t** snaken,
   int world_width,
   int world_height
) {
   snaken_error_code_t snaken_error;

   srand(2);

   snaken_error = snaken2d_init(snaken, world_width, world_height);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error initializing the snaken: %d\n", snaken_error);
      return snaken_error;
   }

   snaken_error = snaken2d_set_snake_length(*snaken, 0xFFu);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error updating the snake length: %d\n", snaken_error);
      return snaken_error;
   }

   snaken_error = snaken2d_set_snake_view_radius(*snaken, 0x03u);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error updating the snake view radius: %d\n", snaken_error);
      return snaken_error;
   }

   snaken_error = snaken2d_set_self_intersect(*snaken, SNAKEN_FALSE);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error updating intersection rules: %d\n", snaken_error);
      return snaken_error;
   }

   snaken_error = snaken2d_set_snake_speed(*snaken, 0xF5);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error updating the snake speed: %d\n", snaken_error);
      return snaken_error;
   }

   snaken_error = snaken2d_set_snake_stamina(*snaken, SNAKEN_SNAKE_STAMINA_LOW);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error updating the snake stamina: %d\n", snaken_error);
      return snaken_error;
   }

   snaken_error = snaken2d_set_apples_count(*snaken, 0x0F);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error setting the amount of apples: %d\n", snaken_error);
      return snaken_error;
   }

   return SNAKEN_ERROR_NONE;
}

/// @brief Evaluates the provided cortex.
/// @param cortex The cortex to evaluate.
/// @param fitness The cortex fitness score as a result of the evaluation process.
/// @return The code for the occurred error, [BHM_ERROR_NONE] if none.
bhm_error_code_t eval_cortex(
   bhm_cortex2d_t* cortex,
   bhm_cortex_fitness_t* fitness
) {
   // ##########################################
   // Init cortices.
   // ##########################################
   bhm_error_code_t bhm_error;

   bhm_cortex2d_t* tmp_cortex;
   bhm_error = c2d_create(&tmp_cortex, cortex->width, cortex->height, cortex->nh_radius);
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error creating cortex %d\n", bhm_error);
      return bhm_error;
   }
   bhm_error = c2d_copy(tmp_cortex, cortex);
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error copying cortex %d\n", bhm_error);
      return bhm_error;
   }

   bhm_cortex_counts_t* counts = (bhm_cortex_counts_t*) malloc(sizeof(bhm_cortex_counts_t));
   counts->ticks_count = 0x00;
   counts->evols_count = 0x00;
   // ##########################################
   // ##########################################


   // ##########################################
   // Init snaken.
   // ##########################################
   snaken_error_code_t snaken_error;

   snaken2d_t* snaken = NULL;
   snaken_error = create_snaken(
      &snaken,
      WORLD_WIDTH,
      WORLD_HEIGHT
   );
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error creating the snaken: %d\n", snaken_error);
      return BHM_ERROR_EXTERNAL_CAUSES;
   }
   // ##########################################
   // ##########################################


   // ##########################################
   // Input init.
   // ##########################################
   bhm_ticks_count_t mean_input = 0;
   snaken_world_size_t snaken_view_width = NH_DIAM_2D(snaken->snake_view_radius);
   snaken_cell_type_t* snake_view = (snaken_cell_type_t*) malloc(snaken_view_width * snaken_view_width * sizeof(snaken_cell_type_t));
   bhm_cortex_size_t input_width = clamp(snaken_view_width * snaken_view_width, 0, cortex->width);
   bhm_input2d_t* input;
   bhm_error = i2d_init(
      &input,
      (cortex->width / 2) - (input_width / 2),
      0,
      (cortex->width / 2) + (input_width / 2),
      1,
      BHM_MAX_EXC_VALUE * 2,
      BHM_PULSE_MAPPING_FPROP
   );
   if (bhm_error != BHM_ERROR_NONE) {
      printf("i2d x0 %d x1 %d\n", (cortex->width / 2) - (input_width / 2), (cortex->width / 2) + (input_width / 2));
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

   bhm_ticks_count_t timestep = 0;

   // uint64_t t0 = nanos();
   // uint64_t t1 = t0;

   for (; timestep < MAX_EVAL_TIME; timestep++) {

      // Make sure the snake is still alive before going on.
      if (!snaken->snake_alive) break;

      bhm_cortex2d_t* prev_cortex = timestep % 2 ? tmp_cortex : cortex;
      bhm_cortex2d_t* next_cortex = timestep % 2 ? cortex : tmp_cortex;

      // Defines whether to evolve or not.
      // evol_step is incremented by 1 to account for edge cases and human readable behavior:
      // 0x0000 -> 0 + 1 = 1, so the cortex evolves at every tick, meaning that there are no free ticks between evolutions.
      // 0xFFFF -> 65535 + 1 = 65536, so the cortex never evolves, meaning that there is an infinite amount of ticks between evolutions.
      bhm_bool_t evolve = (counts->ticks_count % (((bhm_evol_step_t) prev_cortex->evol_step) + 1)) == 0;

      // Get snake view as input.
      snaken_error = snaken2d_get_snake_view(snaken, snake_view);
      if (snaken_error != SNAKEN_ERROR_NONE) {
         printf("There was an error retrieving the snake view: %d\n", snaken_error);
         return BHM_ERROR_EXTERNAL_CAUSES;
      }

      // t1 = nanos();
      // printf("Got snake view in %llu ns\n", t1 - t0);
      // t0 = nanos();

      // printf("SNAKE_DIR: %d\n", snaken->snake_direction);
      // print_snake_view(snake_view, snaken_view_width);

      // Feed input to the cortex.
      // Only the frontal snake view is fed as input to the network.
      for (bhm_cortex_size_t y = 0; y < input->y1 - input->y0; y++) {
         for (bhm_cortex_size_t x = 0; x < input->x1 - input->x0; x++) {
            // TODO Generalize linear interpolation: https://gist.github.com/Mathorga/3ec1425af3853a69ecc8f4e5d9d4d523
            float t = ((float) x) * (snaken_view_width - 1) / (input_width - 1);
            int index = (int) t;
            float frac = t - index;

            if (index < snaken_view_width - 1) {
               bhm_ticks_count_t lower_view_value = snake_view_to_pulse(snake_view[IDX2D(index, y, snaken_view_width)], prev_cortex->sample_window) * (1 - frac);
               bhm_ticks_count_t higher_view_value = snake_view_to_pulse(snake_view[IDX2D(index + 1, y, snaken_view_width)], prev_cortex->sample_window) * frac;
               input->values[IDX2D(x, y, input->x1 - input->x0)] = lower_view_value + higher_view_value;
            } else {
               input->values[IDX2D(x, y, input->x1 - input->x0)] = snake_view_to_pulse(snake_view[IDX2D(index, y, snaken_view_width)], prev_cortex->sample_window);
            }
         }
      }
      i2d_mean(input, &mean_input);
      c2d_feed2d(
         prev_cortex,
         input,
         timestep
      );

      // t1 = nanos();
      // printf("Fed cortex in %llu ns\n", t1 - t0);
      // t0 = nanos();

      // Tick the cortex.
      c2d_tick(
         prev_cortex,
         next_cortex,
         evolve
      );

      // t1 = nanos();
      // printf("Ticked cortex in %llu ns\n", t1 - t0);
      // t0 = nanos();

      counts->ticks_count++;
      // Increment evolutions count.
      if (evolve) counts->evols_count++;

      // Tick the snaken.
      snaken_error = snaken2d_tick(snaken);
      if (snaken_error != SNAKEN_ERROR_NONE) {
         printf("There was an error running the snaken: %d\n", snaken_error);
         return BHM_ERROR_EXTERNAL_CAUSES;
      }

      // t1 = nanos();
      // printf("Ticked snaken in %llu ns\n", t1 - t0);
      // t0 = nanos();

      // Read cortex output.
      c2d_read2d(next_cortex, left_output);
      c2d_read2d(next_cortex, right_output);

      // t1 = nanos();
      // printf("Read output in %llu ns\n", t1 - t0);
      // t0 = nanos();

      o2d_mean(left_output, &mean_left_output);
      o2d_mean(right_output, &mean_right_output);

      // t1 = nanos();
      // printf("Meaned output in %llu ns\n", t1 - t0);
      // t0 = nanos();

      // Use cortex output to control the snake.
      // The snake should do nothing (keep going straight) if the two outputs are equal.
      if (mean_left_output > mean_right_output) {
         snaken2d_turn_left(snaken);
      } else if (mean_right_output > mean_left_output) {
         snaken2d_turn_right(snaken);
      }

      // t1 = nanos();
      // printf("Applied turning in %llu ns\n\n", t1 - t0);
      // t0 = nanos();

      #ifdef GRAPHICS
      BeginDrawing();
         ClearBackground(BLACK);
         draw_snaken(
            snaken,
            WINDOW_WIDTH,
            WINDOW_HEIGHT
         );
         draw_cortex(
            prev_cortex,
            WINDOW_WIDTH,
            WINDOW_HEIGHT
         );
      EndDrawing();
      #endif

      // usleep(10000);
   }

   // printf("Evaluated cortex in %llu ms\n", millis() - t0);

   // ##########################################
   // ##########################################

   *fitness = snaken->snake_length + timestep;

   // Cleanup.
   bhm_error = c2d_destroy(tmp_cortex);
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error destroying the tmp cortex: %d\n", bhm_error);
      return bhm_error;
   }
   bhm_error = i2d_destroy(input);
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error destroying the cortex input: %d\n", bhm_error);
      return bhm_error;
   }
   bhm_error = o2d_destroy(left_output);
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error destroying the left output: %d\n", bhm_error);
      return bhm_error;
   }
   bhm_error = o2d_destroy(right_output);
   if (bhm_error != BHM_ERROR_NONE) {
      printf("There was an error destroying the right output: %d\n", bhm_error);
      return bhm_error;
   }
   snaken_error = snaken2d_destroy(snaken);
   if (snaken_error != SNAKEN_ERROR_NONE) {
      printf("There was an error destroying the snaken: %d\n", snaken_error);
      return BHM_ERROR_EXTERNAL_CAUSES;
   }

   free(snake_view);

   return BHM_ERROR_NONE;
}

int evolve(
   int pop_size,
   int max_eval_time,
   int gens_count,
   char* pop_file_name
) {
   bhm_error_code_t bhm_error;
   bhm_population2d_t* population;

   if (pop_file_name != NULL) {
      // ##########################################
      // Read population from file.
      // ##########################################

      printf(
         "Evolving population from file %s\n",
         pop_file_name
      );

      // When reading a population from file, the population must be allocated first, since p2d_from_file does not manage allocation by itself.
      population = (bhm_population2d_t *) malloc(sizeof(bhm_cortex2d_t));
      p2d_from_file(population, pop_file_name);
      p2d_set_eval_function(population, &eval_cortex);
      // p2d_set_eval_function(population, &dummy_eval);

      // for (bhm_population_size_t i; i < population->size; i++) {
      //    char pop_string[500];
      //    c2d_to_string(&(population->cortices[i]), pop_string);
      //    printf("FITNESS: %d\n", population->cortices_fitness[i]);
      //    printf("%s\n", pop_string);
      // }

      // ##########################################
      // ##########################################
   } else {
      // ##########################################
      // Init cortices population.
      // ##########################################

      printf("Evolving population with params:\n");
      printf(
         "pop_size: %d\nmax_eval_time: %d\ngens_count: %d\n",
         pop_size,
         max_eval_time,
         gens_count
      );

      const int population_selection_pool_size = (int) (POP_SIZE / 10);

      bhm_error = p2d_init(
         &population,
         pop_size,
         population_selection_pool_size,
         POP_MUT_CHANCE,
         &eval_cortex
         // &dummy_eval
      );
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error initializing the population: %d\n", bhm_error);
         return 1;
      }

      bhm_error = p2d_populate(
         population,
         CORTICES_WIDTH,
         CORTICES_HEIGHT,
         CORTICES_NH_RADIUS
      );
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error populating the cortices: %d\n", bhm_error);
         return 1;
      }
      // ##########################################
      // ##########################################
   }

   #ifdef GRAPHICS
   InitWindow(
      WINDOW_WIDTH,
      WINDOW_HEIGHT,
      "BHM SNAKE"
   );

   SetTargetFPS(960);
   #endif

   #ifdef PLOT
   // Start real-time plotting.
   FILE* gnuplotPipe = popen("gnuplot -persistent", "w");
   
   if (gnuplotPipe == NULL) {
      printf("Could not open pipe to gnuplot\n");
      return 1;
   }
   
   // Initial gnuplot configuration.
   fprintf(gnuplotPipe, "set title 'Best cortex fitness'\n");
   fprintf(gnuplotPipe, "set yrange [2000:10000]\n");
   fprintf(gnuplotPipe, "set xrange [0:%d]\n", gens_count);
   double x_plot_data[gens_count];
   double y_plot_data[gens_count];
   #endif
   
   // ##########################################
   // Evolve the population.
   // ##########################################
   for (uint16_t i = 0; i < gens_count; i++) {
      uint64_t t0 = millis();
      bhm_error = p2d_evaluate(population);
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error evaluating the cortices: %d\n", bhm_error);
         return 1;
      }
      printf("Evaluated generation %d in %llu ms\n", i, millis() - t0);

      // Save the population to file before evaluation.
      char file_name[100];
      snprintf(file_name, 100, "out/pop_%d.p2d", i);
      p2d_to_file(population, file_name);

      bhm_error = p2d_select(population);
      if (bhm_error != BHM_ERROR_NONE) {
         printf("There was an error selecting survivors: %d\n", bhm_error);
         return 1;
      }

      #ifdef PLOT
      x_plot_data[i] = i;
      y_plot_data[i] = population->cortices_fitness[population->selection_pool[0]];

      // Plot data.
      if (i % 10 == 0) {
         // Tell gnuplot to plot data from standard input ('-').
         fprintf(gnuplotPipe, "plot '-' with lines title 'Signal'\n");

         // Send the raw X/Y data directly through the pipe.
         for (int j = 0; j <= i; j++) {
            fprintf(gnuplotPipe, "%f %f\n", x_plot_data[j], y_plot_data[j]);
         }

         // Send the 'e' character to tell gnuplot the data block is finished.
         fprintf(gnuplotPipe, "e\n");
         
         // Flush the pipe to ensure the commands are executed immediately.
         fflush(gnuplotPipe);
      }
      #endif

      printf(
         "Best of generation %d: cortex %d with fitness %d\n",
         i,
         population->selection_pool[0],
         population->cortices_fitness[population->selection_pool[0]]
      );

      #ifdef GRAPHICS
      char cortex_string[100];
      c2d_to_string(&(population->cortices[population->selection_pool[0]]), cortex_string);
      printf("%s", cortex_string);
      #endif

      // Save the best cortex to file before the population is reset by crossover.
      // char file_name[100];
      // snprintf(file_name, 100, "out/bog_%d.c2d", i);
      // c2d_to_file(&(population->cortices[population->selection_pool[0]]), file_name);

      bhm_error = p2d_crossover(population, BHM_TRUE);
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
   #ifdef GRAPHICS
   CloseWindow();
   #endif

   #ifdef PLOT
   pclose(gnuplotPipe);
   #endif

   p2d_destroy(population);
   // ##########################################
   // ##########################################

   return 0;
}

struct option evolve_options[] = {
   {"pop_size", required_argument, 0, 's'},
   {"max_eval_time", required_argument, 0, 't'},
   {"gens_count", required_argument, 0, 'g'},
   {"pop_file_path", required_argument, 0, 'f'},
   {0, no_argument, 0, 0}
};

int main(int argc, char** argv) {
   srand(time(NULL));

   // ##########################################
   // Input handling.
   // ##########################################

   // Make sure a command was provided.
   if (argc < 2) {
      printf("No command provided. Type \"bhm_snake help\" for a list of all available commands\n");
      return 1;
   }

   // Parse command.
   if (strcmp(argv[1], "evolve") == 0) {
      int pop_size = POP_SIZE;
      int max_eval_time = MAX_EVAL_TIME;
      int gens_count = GENERATIONS_COUNT;
      char* pop_file_path = NULL;

      int opt;
      int option_index = 0;

      // Loop through all arguments
      while ((opt = getopt_long(argc - 1, argv + 1, "s:o:", evolve_options, &option_index)) != -1) {
         switch (opt) {
            case 's':
               pop_size = atoi(optarg);
               break;
            case 't':
               max_eval_time = atoi(optarg);
               break;
            case 'g':
               gens_count = atoi(optarg);
               break;
            case 'f':
               pop_file_path = optarg;
               break;
            case '?':
               printf("Unknown option or missing value.\n");
               return 1;
            default:
               abort();
         }
      }

      printf("Running evolve with pop_size %d, max_eval_time %d, gens_count %d, pop_file_path %s\n", pop_size, max_eval_time, gens_count, pop_file_path);

      // Evolve.
      return evolve(
         pop_size,
         max_eval_time,
         gens_count,
         pop_file_path
      );
   }

   if (strcmp(argv[1], "help") == 0) {
      // TODO.
      printf("Help requested\n");
      return 0;
   }

   if (strcmp(argv[1], "run") == 0) {
      // TODO.
      printf("Unimplemented\n");
      return 0;
   }
   // ##########################################
   // ##########################################

   return 0;
}
