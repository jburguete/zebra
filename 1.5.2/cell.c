/**
 * \file cell.c
 * \brief source file to define the calculation node cells.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <math.h>
#include <libxml/parser.h>
#include <glib.h>
#include <gsl/gsl_rng.h>
#include "config.h"
#include "tools.h"
#include "solute.h"
#include "species.h"
#include "specimen.h"
#include "point.h"
#include "cell.h"

/**
 * function to set the initial conditions on a cell.
 */
void
cell_init (Cell * cell,         ///< pointer to the cell struct data.
           double position,     ///< longitudinal position.
           double distance,     ///< longitudinal distance to the next cell.
           double size,         ///< longitudinal size. 
           double area,         ///< cross sectional area.
           double perimeter)    ///< cross sectional perimeter.
{
  unsigned int i;
#if DEBUG_CELL
  fprintf (stderr, "cell_init: start\n");
#endif
  cell->recirculation = 0;
  cell->position = position;
  cell->distance = distance;
  cell->size = size;
  cell->area = area;
  cell->volume = area * size;
  cell->perimeter = perimeter;
  cell->lateral_area = perimeter * size;
  for (i = 0; i < MAX_SPECIES; ++i)
    {
      cell->species_concentration[i] = 0.;
      cell->list_specimens[i] = NULL;
    }
  // init solute concentrations
  for (i = 0; i < MAX_SOLUTES; ++i)
    cell->solute_concentration[i] = solute[i].initial_conditions;
#if DEBUG_CELL
  fprintf (stderr, "cell_init: end\n");
#endif
}
