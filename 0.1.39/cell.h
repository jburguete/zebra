/**
 * \file cell.h
 * \brief header file to define the calculation node cells.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef CELL__H
#define CELL__H 1

/**
 * \struct Cell
 * \brief struct to define a node cell for calculation.
 */
typedef struct
{
  double nutrient_concentration[MAX_NUTRIENTS];
  ///< array of nutrient concentrations.
  double species_concentration[MAX_SPECIES];
  ///< array of species concentrations.
  double species_infestation[MAX_SPECIES];
  ///< array of species infestation rate.
  double position;              ///< longitudinal position in the pipe.
  double size;                  ///< longitudinal size.
  double distance;              ///< longitudinal distance to the next cell.
  double area;                  ///< cross sectional area.
  double perimeter;             ///< cross sectional perimeter.
  double volume;                ///< volume.
  double lateral_area;          ///< lateral area.
  double velocity;              ///< flow velocity.
  double discharge;             ///< flow discharge.
  double dispersion;            ///< water dispersion coefficient.
  double roughness;             ///< roughness length.
  double friction_factor;       ///< friction factor.
  unsigned int recirculation;   ///< 1 on recirculation flow, 0 otherwise.
} Cell;

extern double current_time;
extern double next_time;

void cell_init (Cell * cell, double position, double distance, double size,
                double area, double perimeter);

/**
 * function to set the flow properties on a cell.
 */
static inline void
cell_set_flow (Cell * cell,     ///< pointer to the cell struct data.
               double discharge,        ///< flow discharge.
               double velocity, ///< flow velocity.
               double friction_factor,  ///< friction factor.
               double dispersion)       ///< dispersion coefficient.
{
#if DEBUG_CELL
  fprintf (stderr, "cell_set_flow: start\n");
#endif
  cell->discharge = discharge;
  cell->velocity = velocity;
  cell->friction_factor = friction_factor;
  cell->dispersion = dispersion;
#if DEBUG_CELL
  fprintf (stderr, "cell_set_flow: end\n");
#endif
}

/**
 * function to grow the species in a cell.
 */
static inline void
cell_grow (Cell * cell)         ///< pointer to the cell struct data.
{
  Species *s;
  Nutrient *n;
  double *nc[MAX_NUTRIENTS];
  unsigned int i, j;
#if DEBUG_CELL
  fprintf (stderr, "cell_grow: start\n");
#endif
  for (i = 0; i < nspecies; ++i)
    {
      s = species + i;
      switch (s->type)
        {
        case SPECIES_TYPE_ZEBRA_MUSSEL:
          for (j = 0; j < nnutrients; ++j)
            {
              n = nutrient + j;
              switch (n->type)
                {
                case NUTRIENT_TYPE_OXYGEN:
                  nc[0] = cell->nutrient_concentration + j;
                  break;
                case NUTRIENT_TYPE_ORGANIC_MATTER:
                  nc[1] = cell->nutrient_concentration + j;
                }
            }
          zebra_mussel_grow (species + i, cell->species_infestation + i,
                             cell->species_concentration + i, cell->volume,
                             cell->area, cell->lateral_area, cell->velocity,
                             cell->recirculation, 2, nc[0], nc[1]);
        }
    }
#if DEBUG_CELL
  fprintf (stderr, "cell_grow: end\n");
#endif
}

#endif
