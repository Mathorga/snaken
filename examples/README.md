# Examples

You can use these examples to test Snaken.<br/>

## Snake Game

Classic snake game built with Snaken just to demonstrate it can do it.

Compile with `make snake`

Run with `./bin/snake`

## BHM Snake

Custom environment for training and running a Behema cortex on the snake game.

Compile with `make bhm_snake`

Run with `./bin/bhm_snake evolve/run/help`

### Evolve

Params:

`--pop_size [int]`: Sets the size of the population to evolve.

`--max_eval_time [int]`: In steps, the maximum evaluation time for each cortex.

`--gens_count [int]`: The number of generations to evolve for (vaguely corresponds to epochs in supervised learning or episodes in reinforcement learning).

`--pop_file_path [string]`: The path to the population to train.

### Run

### Help