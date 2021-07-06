/**
 * \file simulation.h
 * \brief header file to define the simulation.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
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
  double initial_time;          ///< initial time in seconds since 1970.
  double final_time;            ///< final time in seconds since 1970.
  double cfl;                   ///< CFL number.
  double saving_step;           ///< time step size to save results.
} Simulation;

int simulation_open_xml (Simulation * simulation, char *file_name);

/**
 * function to free the memory used by a simulation struct.
 */
static inline void
simulation_destroy (Simulation * simulation)
                    ///< pointer to the simulation struct data.
{
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_destroy: start\n");
#endif
  network_destroy (simulation->network);
  species_destroy ();
  nutrient_destroy ();
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_destroy: end\n");
#endif
}

/**
 * function to run a simulation.
 */
static inline void
simulation_run (Simulation * simulation)
{
  Results results[1];
  Network *network;
  FILE *file;
  double initial_time, final_time, cfl, saving_time, saving_step;
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_run: start\n");
#endif

  // initial conditions
  network = simulation->network;
  initial_time = simulation->initial_time;
  final_time = simulation->final_time;
  saving_step = simulation->saving_step;
  cfl = simulation->cfl;
  network_set_discharges (network);
  network_initial (network);
  results_init (results, network, initial_time, final_time, saving_step);
  results_set (results, network);
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

          // calculate time step size
          next_time = network_maximum_time (network, saving_time, cfl);
          time_step = next_time - current_time;

          // perform numerical method
          network_step (network);

#if DEBUG_SIMULATION
          fprintf (stderr,
                   "simulation_run: current_time=%.14lg next_time=%.14lg "
                   "time_step=%lg\n", current_time, next_time, time_step);
#endif
        }

      // saving results
      results_set (results, network);
      results_write_variables (results, file);

    }

  // freeing
  fclose (file);
  results_destroy (results);

#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_run: end\n");
#endif
}

#endif
