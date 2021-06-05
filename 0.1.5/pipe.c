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
  double distance;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_create_mesh: start\n");
#endif
  pipe->ncells = (unsigned int) ceil (pipe->length / cell_size);
  pipe->ncells = (2 > pipe->ncells) ? 2 : pipe->ncells;
  n = pipe->ncells - 1;
  distance = pipe->length / n;
  pipe->cell = (Cell *) malloc (pipe->ncells * sizeof (Cell));
  for (i = 0; i < pipe->ncells; ++i)
    {
      cell = pipe->cell + i;
      cell->position = i * pipe->length / n;
      cell->size = cell->distance = distance;
      cell_init (cell);
    }
  cell = pipe->cell;
  cell->size = cell[n].size = 0.5 * distance;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_create_mesh: end\n");
#endif
}
