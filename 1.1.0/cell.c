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
           double perimeter,    ///< cross sectional perimeter.
           unsigned int nsolutes,       ///< solutes number.   
           ...)
{
  va_list list;
  double *solute;
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
  for (i = 0; i < nspecies; ++i)
    {
      cell->species_concentration[i] = 0.;
      cell->list_specimens[i] = NULL;
    }
  // init solute concentrations
  va_start (list, nsolutes);
  i = 0;
  if (flags_solutes & SOLUTE_TYPE_CHLORINE)
    {
      solute = va_arg (list, double *);
      cell->solute_concentration[i++] = *solute;
    }
  if (flags_solutes & SOLUTE_TYPE_HYDROGEN_PEROXIDE)
    {
      solute = va_arg (list, double *);
      cell->solute_concentration[i++] = *solute;
    }
  if (flags_solutes & SOLUTE_TYPE_ORGANIC_MATTER)
    {
      solute = va_arg (list, double *);
      cell->solute_concentration[i++] = *solute;
    }
  if (flags_solutes & SOLUTE_TYPE_OXYGEN)
    {
      solute = va_arg (list, double *);
      cell->solute_concentration[i++] = *solute;
    }
  va_end (list);
#if DEBUG_CELL
  fprintf (stderr, "cell_init: end\n");
#endif
}
