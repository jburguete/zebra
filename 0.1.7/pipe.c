/**
 * \file pipe.h
 * \brief source file to define the network pipes.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libxml/parser.h>
#include "config.h"
#include "tools.h"
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
  double distance, size;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_create_mesh: start\n");
#endif
  pipe->area = M_PI * pipe->diameter * pipe->diameter;
  pipe->perimeter = 2. * M_PI * pipe->diameter;
  pipe->ncells = (unsigned int) ceil (pipe->length / cell_size);
  pipe->ncells = (2 > pipe->ncells) ? 2 : pipe->ncells;
  pipe->nwalls = n = pipe->ncells - 1;
  distance = pipe->length / n;
  size = 0.5 * distance;
  pipe->cell = cell = (Cell *) malloc (pipe->ncells * sizeof (Cell));
  pipe->wall = wall = (Wall *) malloc (n * sizeof (Wall));
  cell_init (cell, 0., distance, size, pipe->area, pipe->perimeter);
  for (i = 1; i < pipe->ncells - 1; ++i)
    cell_init (++cell, i * pipe->length / n, distance, distance, pipe->area,
               pipe->perimeter);
  cell_init (++cell, pipe->length, distance, size, pipe->area, pipe->perimeter);
  cell = pipe->cell;
  for (i = 0; i < pipe->nwalls; ++i, ++wall, ++cell)
    wall_init (wall, cell, cell + 1);
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
  unsigned int i;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_set_discharge: start\n");
#endif
  pipe->discharge = discharge;
  pipe->velocity = discharge / pipe->area;
  for (i = 0; i < pipe->ncells; ++i)
    cell_set_flow (pipe->cell + i, pipe->discharge, pipe->velocity);
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
  unsigned int i;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_set_velocity: start\n");
#endif
  pipe->velocity = velocity;
  pipe->discharge = velocity * pipe->area;
  for (i = 0; i < pipe->ncells; ++i)
    cell_set_flow (pipe->cell + i, pipe->discharge, pipe->velocity);
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
