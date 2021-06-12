/**
 * \file pipe.h
 * \brief source file to define the network pipes.
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

/**
 * function to init an empty pipe.
 */
void
pipe_null (Pipe * pipe)         ///< pointer to the pipe struct data.
{
#if DEBUG_PIPE
  fprintf (stderr, "pipe_null: start\n");
#endif
  pipe->cell = NULL;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_null: end\n");
#endif
}

/**
 * function free the memory used by a pipe.
 */
void
pipe_destroy (Pipe * pipe)      ///< pointer to the pipe struct data.
{
#if DEBUG_PIPE
  fprintf (stderr, "pipe_destroy: start\n");
#endif
  free (pipe->wall);
  free (pipe->cell);
#if DEBUG_PIPE
  fprintf (stderr, "pipe_destroy: end\n");
#endif
}

/**
 * function to create a mesh of cells in a pipe.
 */
void
pipe_create_mesh (Pipe * pipe,  ///< pointer to the pipe struct data.
                  double cell_size)     ///< maximum cell size.
{
  Cell *cell;
  Wall *wall;
  double distance, size, area, perimeter;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_create_mesh: start\n");
#endif
  pipe->perimeter = perimeter = M_PI * pipe->diameter;
  pipe->area = area = 0.25 * perimeter * pipe->diameter;
  pipe->ncells = (unsigned int) ceil (pipe->length / cell_size);
  pipe->ncells = (2 > pipe->ncells) ? 2 : pipe->ncells;
  pipe->nwalls = n = pipe->ncells - 1;
  distance = pipe->length / n;
  size = 0.5 * distance;
  pipe->cell = cell = (Cell *) malloc (pipe->ncells * sizeof (Cell));
  pipe->wall = wall = (Wall *) malloc (n * sizeof (Wall));
  cell_init (cell, 0., distance, size, pipe->area, pipe->perimeter);
  for (i = 1; i < pipe->ncells - 1; ++i)
    cell_init (cell + i, i * pipe->length / n, distance, distance, area,
               perimeter);
  cell_init (cell + i, pipe->length, distance, size, area, perimeter);
  for (i = 0; i < pipe->nwalls; ++i)
    wall_init (wall + i, cell + i, cell + i + 1);
#if NUMERICAL_ORDER > 1
  for (i = 1; i < pipe->nwalls - 1; ++i)
    wall_init_2 (wall + i, cell + i - 1, cell + i + 2);
#endif
#if DEBUG_PIPE
  fprintf (stderr, "pipe_create_mesh: end\n");
#endif
}

/**
 * function to set the flow discharge in a pipe.
 */
void
pipe_set_discharge (Pipe * pipe,        ///< pointer to the pipe struct data.
                    double discharge)   ///< flow discharge.
{
  Wall *wall;
  Cell *cell;
  double v;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_set_discharge: start\n");
#endif
  pipe->discharge = discharge;
  pipe->velocity = v = discharge / pipe->area;
  cell = pipe->cell;
  n = pipe->ncells;
  for (i = 0; i < n; ++i)
    cell_set_flow (cell + i, discharge, v);
  v *= time_step;
  wall = pipe->wall;
  n = pipe->nwalls;
  for (i = 0; i < n; ++i)
    wall_set_flow (wall + i, v);
#if DEBUG_PIPE
  fprintf (stderr, "pipe_set_discharge: end\n");
#endif
}

/**
 * function to set the flow velocity in a pipe.
 */
void
pipe_set_velocity (Pipe * pipe, ///< pointer to the pipe struct data.
                   double velocity)     ///< flow velocity.
{
  Cell *cell;
  double q;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_set_velocity: start\n");
#endif
  pipe->velocity = velocity;
  pipe->discharge = q = velocity * pipe->area;
  cell = pipe->cell;
  n = pipe->ncells;
  for (i = 0; i < n; ++i)
    cell_set_flow (cell + i, q, velocity);
#if DEBUG_PIPE
  fprintf (stderr, "pipe_set_velocity: end\n");
#endif
}

/**
 * function to get the cell of a node identifier in a pipe.
 *
 * \return cell pointer on succes, NULL on error.
 */
Cell *
pipe_node_cell (Pipe * pipe,    ///< pointer to the pipe struct data.
                unsigned int id)        ///< node identifier.
{
  Cell *cell = NULL;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_node_cell: start\n");
#endif
  if (id == pipe->inlet_id)
    cell = pipe->cell;
  if (id == pipe->outlet_id)
    cell = pipe->cell + pipe->ncells - 1;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_node_cell: end\n");
#endif
  return cell;
}

/**
 * function to return the maximum next time allowed by a pipe.
 *
 * \return maximum time in seconds since 1970.
 */
double
pipe_maximum_time (Pipe * pipe, ///< pointer to the pipe struct data.
                   double cfl)  ///< CFL number.
{
  double t, v;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_maximum_time: start\n");
#endif
  v = fabs (pipe->velocity);
  t = current_time + cfl * pipe->cell->size / v;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_maximum_time: t=%.14lg v=%lg\n", t, v);
  fprintf (stderr, "pipe_maximum_time: end\n");
#endif
  return t;
}

/**
 * function to set the initial conditions on a pipe.
 */
void
pipe_initial (Pipe * pipe,      ///< pointer to the pipe struct data.
              double *nutrient_concentration,
              ///< array of nutrient concetrations.
              double *species_concentration)
              ///< array of species concetrations.
{
  Cell *cell;
  size_t sn, ss;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_initial: start\n");
#endif
  n = pipe->ncells;
  sn = nnutrients * sizeof (double);
  ss = nspecies * sizeof (double);
  for (i = 0; i < n; ++i)
    {
      cell = pipe->cell + i;
      memcpy (cell->nutrient_concentration, nutrient_concentration, sn);
      memcpy (cell->species_concentration, species_concentration, ss);
    }
#if DEBUG_PIPE
  fprintf (stderr, "pipe_initial: end\n");
#endif
}

/**
 * function to perform a numerical method step on a pipe.
 */
void
pipe_step (Pipe * pipe)         ///< pointer to the pipe struct data.
{
  Wall *wall;
  double v;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_step: start\n");
#endif
  wall = pipe->wall;
  n = pipe->nwalls;
  for (i = 0; i < n; ++i)
    wall_set (wall + i);
  v = pipe->velocity;
  if (v > 0.)
    for (i = 0; i < n; ++i)
      wall_increments_p (wall + i);
  else if (v < 0.)
    for (i = 0; i < n; ++i)
      wall_increments_n (wall + i);
#if DEBUG_PIPE
  fprintf (stderr, "pipe_step: end\n");
#endif
}
