/**
 * \file simulation.h
 * \brief header file to define the simulation.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
 */
#ifndef SIMULATION__H
#define SIMULATION__H 1

/**
 * \struct Simulation
 * \brief struct to define a simulation.
 */
typedef struct
{
  Network network[1];           ///< network struct data.
  char results[BUFFER_SIZE];    ///< results file name.
  char summary[BUFFER_SIZE];    ///< summary file name.
  double initial_time;          ///< initial time in seconds since 1970.
  double final_time;            ///< final time in seconds since 1970.
  double cfl;                   ///< CFL number.
  double dispersion_cfl;        ///< dispersion CFL number.
  double saving_step;           ///< time step size to save results.
} Simulation;

int simulation_open_xml (Simulation * simulation, char *file_name);

/**
 * function to free the memory used by a simulation struct.
 */
static inline void
simulation_destroy (Simulation *simulation)
                    ///< pointer to the simulation struct data.
{
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_destroy: start\n");
#endif
  network_destroy (simulation->network);
  species_destroy ();
  solute_destroy ();
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_destroy: end\n");
#endif
}

/**
 * function to run a simulation.
 */
static inline void
simulation_run (Simulation *simulation)
{
  Results results[1];
  Network *network;
  gsl_rng *rng;
  FILE *file;
  double initial_time, final_time, cfl, dispersion_cfl, saving_time,
    saving_step;
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_run: start\n");
#endif

  // init GSL random numbers generator
  rng = gsl_rng_alloc (gsl_rng_taus2);
  gsl_rng_set (rng, RANDOM_SEED);

  // initial conditions
  network = simulation->network;
  initial_time = simulation->initial_time;
  final_time = simulation->final_time;
  biological_step = saving_step = simulation->saving_step;
  cfl = simulation->cfl;
  dispersion_cfl = simulation->dispersion_cfl;
  network_set_flow (network);
  network_initial (network);
  results_init (results, network, initial_time, final_time, saving_step);
  results_set (results, network);
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_run: results file=%s\n", simulation->results);
#endif
  file = fopen (simulation->results, "wb");
  results_write_header (results, file);

  // saving initial results
  results_set (results, network);
  results_write_variables (results, file);

  // bucle
  for (current_time = initial_time; current_time < final_time;
       current_time = saving_time)
    {

      // time for saving results
      saving_time = fmin (current_time + saving_step, final_time);

      // inner bucle
      for (; current_time < saving_time; current_time = next_time)
        {

          // update discharges and velocities
          network_update_discharges (network);

          // calculate time step sizes
          advection_step = network_step_size (network, cfl);
          dispersion_step = dispersion_cfl * advection_step;
          next_time = network_maximum_time (network, saving_time);

          // perform numerical method
          network_step (network, rng);

#if DEBUG_SIMULATION
          fprintf (stderr,
                   "simulation_run: current_time=%.14lg next_time=%.14lg\n",
                   current_time, next_time);
#endif
        }

      // saving results
      results_set (results, network);
      results_write_variables (results, file);

    }

  // saving summary
  network_summary (network, simulation->summary);

  // freeing
  fclose (file);
  gsl_rng_free (rng);
  results_destroy (results);

#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_run: end\n");
#endif
}

#endif
