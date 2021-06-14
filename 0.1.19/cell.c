/**
 * \file cell.c
 * \brief source file to define the calculation node cells.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <math.h>
#include <libxml/parser.h>
#include "config.h"
#include "tools.h"
#include "nutrient.h"
#include "species.h"
#include "point.h"
#include "cell.h"

double current_time;            ///< current time.
double next_time;               ///< next time.
double time_step;               ///< time step size.

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
  cell->position = position;
  cell->distance = distance;
  cell->size = size;
  cell->area = area;
  cell->volume = area * size;
  cell->perimeter = perimeter;
  for (i = 0; i < nnutrients; ++i)
    cell->nutrient_concentration[i] = 0.;
  for (i = 0; i < nspecies; ++i)
    cell->species_concentration[i] = cell->species_infestation[i] = 0.;
#if DEBUG_CELL
  fprintf (stderr, "cell_init: end\n");
#endif
}

/**
 * function to set the flow properties on a cell.
 */
void
cell_set_flow (Cell * cell,     ///< pointer to the cell struct data.
               double discharge,        ///< flow discharge.
               double velocity) ///< flow velocity.
{
#if DEBUG_CELL
  fprintf (stderr, "cell_set_flow: start\n");
#endif
  cell->discharge = discharge;
  cell->velocity = velocity;
#if DEBUG_CELL
  fprintf (stderr, "cell_set_flow: end\n");
#endif
}
