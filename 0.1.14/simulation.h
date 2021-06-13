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

void simulation_destroy (Simulation * simulation);
int simulation_open_xml (Simulation * simulation, char *file_name);
void simulation_run (Simulation * simulation);

#endif
