/**
 * \file junction.c
 * \brief source file to define the network junctions.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <libxml/parser.h>
#include "config.h"
#include "tools.h"
#include "nutrient.h"
#include "species.h"
#include "point.h"
#include "cell.h"
#include "wall.h"
#include "pipe.h"
#include "junction.h"

/**
 * function to init an empty junction.
 */
void
junction_null (Junction * junction)
               ///< pointer to the junction struct data.
{
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_null: start\n");
#endif
  junction->inlet = junction->outlet = NULL;
  junction->point = NULL;
  junction->cell = NULL;
  junction->ninlets = junction->noutlets = 0;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_null: end\n");
#endif
}

/**
 * function to free the memory used by a junction.
 */
void
junction_destroy (Junction * junction)
               ///< pointer to the junction struct data.
{
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_destroy: start\n");
#endif
  free (junction->cell);
  free (junction->outlet);
  free (junction->inlet);
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_destroy: end\n");
#endif
}

/**
 * function to add an inlet pipe to a junction.
 */
void
junction_add_inlet (Junction * junction,
                    ///< pointer to the junction struct data.
                    Pipe * pipe)
                    ///< pointer to the inlet pipe struct data.
{
  unsigned int n;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_add_inlet: start\n");
#endif
  n = junction->ninlets++;
  junction->inlet
    = (Pipe **) realloc (junction->inlet, junction->ninlets * sizeof (Pipe *));
  junction->inlet[n] = pipe;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_add_inlet: end\n");
#endif
}

/**
 * function to add an outlet pipe to a junction.
 */
void
junction_add_outlet (Junction * junction,
                     ///< pointer to the junction struct data.
                     Pipe * pipe)
                     ///< pointer to the outlet pipe struct data.
{
  unsigned int n;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_add_outlet: start\n");
#endif
  n = junction->noutlets++;
  junction->outlet
    = (Pipe **) realloc (junction->outlet,
                         junction->noutlets * sizeof (Pipe *));
  junction->outlet[n] = pipe;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_add_outlet: end\n");
#endif
}

/**
 * function to init the variables on a junction.
 */
void
junction_init (Junction * junction)
               ///< pointer to the junction struct data.
{
  Pipe *pipe;
  Cell *cell;
  double volume;
  unsigned int i, j, ninlets, noutlets;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_init: start\n");
#endif
  ninlets = junction->ninlets;
  noutlets = junction->noutlets;
  junction->ncells = ninlets + noutlets;
  junction->cell = (Cell **) malloc (junction->ncells * sizeof (Cell *));
  volume = 0.;
  for (i = 0; i < ninlets; ++i)
    {
      pipe = junction->inlet[i];
      junction->cell[i] = cell = pipe->cell;
      volume += cell->volume;
    }
  for (j = 0; j < noutlets; ++j, ++i)
    {
      pipe = junction->outlet[j];
      junction->cell[i] = cell = pipe->cell + pipe->ncells - 1;
      volume += cell->volume;
    }
  junction->volume = volume;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_init: end\n");
#endif
}

/**
 * function to set the variables on a junction.
 */
void
junction_set (Junction * junction)
              ///< pointer to the junction struct data.
{
  double nc[MAX_NUTRIENTS], sc[MAX_SPECIES];
  Cell *cell;
  unsigned int i, j, n;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_set: start\n");
#endif
  n = junction->ncells;
  for (j = 0; j < nnutrients; ++j)
    nc[j] = 0.;
  for (j = 0; j < nspecies; ++j)
    sc[j] = 0.;
  for (i = 0; i < n; ++i)
    {
      cell = junction->cell[i];
      for (j = 0; j < nnutrients; ++j)
        nc[j] += cell->volume * cell->nutrient_concentration[j];
      for (j = 0; j < nspecies; ++j)
        sc[j] += cell->volume * cell->species_concentration[j];
    }
  for (j = 0; j < nnutrients; ++j)
    nc[j] /= junction->volume;
  for (j = 0; j < nspecies; ++j)
    sc[j] /= junction->volume;
  for (i = 0; i < n; ++i)
    {
      cell = junction->cell[i];
      memcpy (cell->nutrient_concentration, nc, nnutrients * sizeof (double));
      memcpy (cell->species_concentration, sc, nspecies * sizeof (double));
    }
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_set: end\n");
#endif
}
