/**
 * \file junction.c
 * \brief source file to define the network junctions.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include "config.h"
#include "tools.h"
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
  junction->ninlets = junction->noutlets = 0;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_null: end\n");
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
