/**
 * \file cell.h
 * \brief header file to define the calculation node cells.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#ifndef CELL__H
#define CELL__H 1

/**
 * \struct Cell
 * \brief struct to define a node cell for calculation.
 */
typedef struct
{
  double solute_concentration[MAX_SOLUTES];
  ///< array of solute concentrations.
  double species_concentration[MAX_SPECIES];
  ///< array of species concentrations.
  double species_infestation[MAX_SPECIES];
  ///< array of species infestations.
  GList *list_specimens[MAX_SPECIES];   ///< array of specimen lists.
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
 * function to cling the larvae in a cell.
 */
static inline void
cell_cling (Cell * cell,        ///< pointer to the cell struct data.
            gsl_rng * rng,      ///< GSL random numbers generator.
            double step)        ///< time step size.
{
  unsigned int i;
#if DEBUG_CELL
  fprintf (stderr, "cell_cling: start\n");
#endif
  for (i = 0; i < MAX_SPECIES; ++i)
    specimen_cling (cell->list_specimens + i, species + i, rng,
                    cell->species_concentration + i, cell->volume,
                    cell->lateral_area, cell->velocity, step,
                    cell->recirculation);
#if DEBUG_CELL
  fprintf (stderr, "cell_cling: end\n");
#endif
}

/**
 * function to grow the specimens in a cell.
 */
static inline void
cell_grow (Cell * cell,         ///< pointer to the cell struct data.
           double step)         ///< time step size.
{
  GList *list;
  unsigned int i;
#if DEBUG_CELL
  fprintf (stderr, "cell_grow: start\n");
#endif
  for (i = 0; i < MAX_SPECIES; ++i)
    for (list = cell->list_specimens[i]; list; list = list->next)
      specimen_grow (list->data, cell->volume, cell->velocity, step,
                     cell->recirculation, cell->solute_concentration);
#if DEBUG_CELL
  fprintf (stderr, "cell_grow: end\n");
#endif
}

/**
 * function to dead specimens in a cell.
 */
static inline void
cell_dead (Cell * cell,         ///< pointer to the cell struct data.
           gsl_rng * rng,       ///< GSL random numbers generator.
           double step)         ///< time step size.
{
  GList *list;
  unsigned int i;
#if DEBUG_CELL
  fprintf (stderr, "cell_dead: start\n");
#endif
  for (i = 0; i < MAX_SPECIES; ++i)
    for (list = cell->list_specimens[i]; list; list = list->next)
      if (specimen_dead (list->data, rng, cell->velocity, step,
                         cell->recirculation, cell->solute_concentration))
        list = g_list_remove (list, list->data);
#if DEBUG_CELL
  fprintf (stderr, "cell_dead: end\n");
#endif
}

/**
 * function to calculate the cell infestations by species.
 */
static inline void
cell_infestations (Cell * cell) ///< pointer to the cell struct data.
{
#if DEBUG_CELL
  fprintf (stderr, "cell_infestations: start\n");
#endif
  Specimen *specimen;
  GList *list;
  unsigned int i;
  for (i = 0; i < MAX_SPECIES; ++i)
    {
      cell->species_infestation[i] = 0.;
      for (list = cell->list_specimens[i]; list; list = list->next)
        {
          specimen = (Specimen *) list->data;
          cell->species_infestation[i] += specimen->size;
        }
      cell->species_infestation[i] /= cell->lateral_area;
    }
#if DEBUG_CELL
  fprintf (stderr, "cell_infestations: end\n");
#endif
}

#endif
