/**
 * \file junction.c
 * \brief source file to define the network junctions.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#define _GNU_SOURCE
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
