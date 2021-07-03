/**
 * \file pipe.c
 * \brief Source file to define the network pipes.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
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
 * Function to set a recirculation zone at the inlet of a pipe.
 */
void
pipe_inlet_add_recirculation (Pipe * pipe,
                              ///< pointer to the pipe data struct.
                              double length)
                              ///< length of the recirculation zone.
{
  Cell *cell;
  unsigned int i;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_inlet_add_recirculation: end\n");
#endif
  for (i = 0; i < pipe->ncells; ++i)
    {
      cell = pipe->cell + i;
      if (cell->position < length)
        cell->recirculation = 1;
      else
        break;
    }
#if DEBUG_PIPE
  fprintf (stderr, "pipe_inlet_add_recirculation: end\n");
#endif
}

/**
 * Function to set a recirculation zone at the outlet of a pipe.
 */
void
pipe_outlet_add_recirculation (Pipe * pipe,
                               ///< pointer to the pipe data struct.
                               double length)
                               ///< length of the recirculation zone.
{
  Cell *cell;
  double x;
  unsigned int i;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_outlet_add_recirculation: end\n");
#endif
  if (length <= 0.)
    goto exit_on_end;
  i = pipe->nwalls;
  cell = pipe->cell + i;
  x = cell->position - length;
  cell->recirculation = 1;
  while (i)
    {
      --i;
      --cell;
      if (cell->position > x)
        cell->recirculation = 1;
      else
        break;
    }
exit_on_end:
#if DEBUG_PIPE
  fprintf (stderr, "pipe_outlet_add_recirculation: end\n");
#endif
  return;
}
