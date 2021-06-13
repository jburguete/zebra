/**
 * \file results.c
 * \brief source file to define the results data base file.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"
#include "nutrient.h"
#include "species.h"
#include "point.h"
#include "cell.h"
#include "wall.h"
#include "pipe.h"
#include "junction.h"
#include "inlet.h"
#include "network.h"
#include "results.h"

/**
 * function to init the header of a results data base.
 */
static inline void
results_header_init (ResultsHeader * header,
                     ///< pointer to the results data base header struct.
                     Network * network, ///< pointer to the network data struct.
                     double initial_time,
                     ///< initial time in seconds since 1970.
                     double final_time, ///< final time in seconds since 1970.
                     double saving_step)        ///< time step size to save results.
{
#if DEBUG_RESULTS
  fprintf (stderr, "results_header_init: start\n");
#endif
  header->initial_time = initial_time;
  header->final_time = final_time;
  header->saving_step = saving_step;
  header->cell_size = network->cell_size;
  header->nnutrients = nnutrients;
  header->nspecies = nspecies;
  header->npoints = network->npoints;
  header->npipes = network->npipes;
#if DEBUG_RESULTS
  fprintf (stderr, "results_header_init: end\n");
#endif
}

/**
 * function to free the memory used by a results data base.
 */
void
results_destroy (Results * results)
{
#if DEBUG_RESULTS
  fprintf (stderr, "results_destroy: start\n");
#endif
  free (results->variable);
  free (results->pipe_cell);
  free (results->pipe_id);
  free (results->point_id);
  free (results->pipe_length);
#if DEBUG_RESULTS
  fprintf (stderr, "results_destroy: end\n");
#endif
}

/**
 * function to init a results data base.
 */
void
results_init (Results * results,
              ///< pointer to the results data base struct.
              Network * network,        ///< pointer to the network data struct.
              double initial_time,
              ///< initial time in seconds since 1970.
              double final_time,        ///< final time in seconds since 1970.
              double saving_step)       ///< time step size to save results.
{
  Pipe *pipe;
  Point *point;
  unsigned int i, j, n;
#if DEBUG_RESULTS
  fprintf (stderr, "results_init: start\n");
#endif
  results_header_init (results->header, network, initial_time, final_time,
                       saving_step);
  n = network->npoints;
  results->point_id = (unsigned int *) malloc (n * sizeof (unsigned int));
  point = network->point;
  for (i = 0; i < n; ++i)
    results->point_id[i] = point[i].id;
  n = network->npipes;
  results->pipe_length = (double *) malloc (n * sizeof (double));
  results->pipe_id = (unsigned int *) malloc (n * sizeof (unsigned int));
  results->pipe_cell
    = (unsigned int *) malloc ((n + 1) * sizeof (unsigned int));
  pipe = network->pipe;
  results->pipe_cell[0] = j = 0;
  for (i = 0; i < n; ++i)
    {
      results->pipe_length[i] = pipe[i].length;
      results->pipe_id[i] = pipe[i].id;
      results->pipe_cell[i + 1] = j += pipe[i].ncells;
    }
  results->nvariables = j *= nnutrients + nspecies;
  results->variable = (double *) malloc (j * sizeof (double));
#if DEBUG_RESULTS
  fprintf (stderr, "results_init: end\n");
#endif
}

/**
 * function to set the variables on the results data base.
 */
void
results_set (Results * results,
             ///< pointer to the results data base struct.
             Network * network) ///< pointer to the network data struct.
{
  Pipe *pipe;
  Cell *cell;
  double *variable;
  unsigned int i, j, k, npipes, ncells;
#if DEBUG_RESULTS
  fprintf (stderr, "results_set: end\n");
#endif
  variable = results->variable;
  pipe = network->pipe;
  npipes = network->npipes;
  for (i = k = 0; i < npipes; ++i, ++pipe)
    {
      cell = pipe->cell;
      ncells = pipe->ncells;
      for (j = 0; j < ncells; ++j, ++cell)
        {
          memcpy (variable + k, cell->nutrient_concentration,
                  nnutrients * sizeof (double));
          k += nnutrients;
          memcpy (variable + k, cell->species_concentration,
                  nspecies * sizeof (double));
          k += nspecies;
        }
    }
#if DEBUG_RESULTS
  fprintf (stderr, "results_set: end\n");
#endif
}

/**
 * function to write the results data base header on a file.
 */
void
results_write_header (Results * results,
                      ///< pointer to the results data base struct.
                      FILE * file)      ///< file.
{
  ResultsHeader *header;
  unsigned int n;
#if DEBUG_RESULTS
  fprintf (stderr, "results_write_header: start\n");
#endif
  header = results->header;
  fwrite (header, sizeof (ResultsHeader), 1, file);
  fwrite (results->point_id, sizeof (unsigned int), header->npoints, file);
  n = header->npipes;
  fwrite (results->pipe_id, sizeof (unsigned int), n, file);
  fwrite (results->pipe_length, sizeof (double), n, file);
  fwrite (results->pipe_cell, sizeof (unsigned int), n + 1, file);
#if DEBUG_RESULTS
  fprintf (stderr, "results_write_header: end\n");
#endif
}

/**
 * function to write the results data base variables on a file.
 */
void
results_write_variables (Results * results,
                         ///< pointer to the results data base struct.
                         FILE * file)   ///< file.
{
#if DEBUG_RESULTS
  fprintf (stderr, "results_write_variables: start\n");
#endif
  fwrite (results->variable, sizeof (double), results->nvariables, file);
#if DEBUG_RESULTS
  fprintf (stderr, "results_write_variables: end\n");
#endif
}
