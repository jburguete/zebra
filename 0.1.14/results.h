/**
 * \file results.h
 * \brief header file to define the results data base file.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef RESULTS__H
#define RESULTS__H 1

/**
 * \struct ResultsHeader
 * \brief struct to define the results data base header.
 */
typedef struct
{
  double initial_time;          ///< initial time in seconds since 1970.
  double final_time;            ///< final time in seconds since 1970.
  double saving_step;           ///< time step size to save results.
  double cell_size;             ///< maximum cell size.
  unsigned int nnutrients;      ///< number of nutrients.
  unsigned int nspecies;        ///< number of species.
  unsigned int npoints;         ///< number of points.
  unsigned int npipes;          ///< number of pipes.
} ResultsHeader;

typedef struct
{
  ResultsHeader header[1];      ///< header data.
  double *pipe_length;          ///< array of pipe lengths.
  unsigned int *point_id;       ///< array of point identifiers.
  unsigned int *pipe_id;        ///< array of pipe identifiers.
  unsigned int *pipe_cell;      ///< array of inlet pipe cells.
  double *variable;             ///< array of variables.
  unsigned int nvariables;      ///< number of variables.
} Results;

void results_destroy (Results * results);
void results_init (Results * results, Network * network, double initial_time,
                   double final_time, double saving_step);
void results_set (Results * results, Network * network);
void results_write_header (Results * results, FILE * file);
void results_write_variables (Results * results, FILE * file);

#endif
