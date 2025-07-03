/**
 * \file cell.h
 * \brief header file to define the calculation node cells.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
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
  GList *list_specimens[MAX_SPECIES];   ///< array of specimen lists.
  double species_concentration[MAX_SPECIES];
  ///< array of species concentrations.
  double species_infestation_volume[MAX_SPECIES];
  ///< array of species infestation volumes.
  double species_infestation_number[MAX_SPECIES];
  ///< array of species infestation numbers.
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
cell_set_flow (Cell *cell,      ///< pointer to the cell struct data.
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
 * function to settlement the larvae in a cell.
 */
static inline void
cell_settlement (Cell *cell,         ///< pointer to the cell struct data.
            gsl_rng *rng,       ///< GSL random numbers generator.
            double step)        ///< time step size.
{
  unsigned int i;
#if DEBUG_CELL
  fprintf (stderr, "cell_settlement: start\n");
#endif
  for (i = 0; i < MAX_SPECIES; ++i)
    {
#if DEBUG_CELL
      fprintf (stderr, "cell_settlement: species=%u specimen-list=%lu\n",
               i, (size_t) cell->list_specimens[i]);
#endif
      specimen_settlement (cell->list_specimens + i, species + i, rng,
                      cell->species_concentration + i, cell->volume,
                      cell->lateral_area, cell->velocity, step,
                      cell->recirculation);
#if DEBUG_CELL
      fprintf (stderr, "cell_settlement: species=%u specimen-list=%lu\n",
               i, (size_t) cell->list_specimens[i]);
#endif
    }
#if DEBUG_CELL
  fprintf (stderr, "cell_settlement: end\n");
#endif
}

/**
 * function to grow the specimens in a cell.
 */
static inline void
cell_grow (Cell *cell,          ///< pointer to the cell struct data.
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
 * function to calculate specimens death in a cell.
 */
static inline void
cell_death (Cell *cell,         ///< pointer to the cell struct data.
            gsl_rng *rng,       ///< GSL random numbers generator.
            double step)        ///< time step size.
{
  GList *list[1];
  double decay;
  unsigned int i, j;
#if DEBUG_CELL
  fprintf (stderr, "cell_death: start\n");
#endif
  for (i = 0; i < MAX_SPECIES; ++i)
    {
      decay = species_larva_decay (species + i, cell->solute_concentration,
                                   step);
      cell->species_concentration[i] *= decay;
#if DEBUG_CELL
      fprintf (stderr, "cell_death: species=%u specimen-list=%lu\n",
               i, (size_t) cell->list_specimens[i]);
#endif
      for (j = 0, list[0] = cell->list_specimens[i]; list[0];
           list[0] = list[0]->next)
        {
          if (specimen_death (list[0]->data, rng, step,
                              cell->solute_concentration))
            {
              list[0] = g_list_remove (list[0], list[0]->data);
              if (!j)
                {
#if DEBUG_CELL
                  fprintf (stderr, "cell_death: list1=%lu list2=%lu\n",
                           (size_t) cell->list_specimens[i], (size_t) list[0]);
#endif
                  cell->list_specimens[i] = list[0];
                }
              if (!list[0])
                break;
            }
          else
            ++j;
        }
#if DEBUG_CELL
      fprintf (stderr, "cell_death: species=%u specimen-list=%lu\n",
               i, (size_t) cell->list_specimens[i]);
#endif
    }
#if DEBUG_CELL
  fprintf (stderr, "cell_death: end\n");
#endif
}

/**
 * function to calculate the cell infestations by species.
 */
static inline void
cell_infestations (Cell *cell)  ///< pointer to the cell struct data.
{
#if DEBUG_CELL
  fprintf (stderr, "cell_infestations: start\n");
#endif
  Specimen *specimen;
  GList *list;
  double v;
  unsigned int i, n;
  for (i = 0; i < MAX_SPECIES; ++i)
    {
      v = 0.;
      n = 0;
      for (list = cell->list_specimens[i]; list; list = list->next)
        {
          specimen = (Specimen *) list->data;
          ++n;
          v += specimen->size;
        }
      cell->species_infestation_volume[i] = v / cell->lateral_area;
      cell->species_infestation_number[i] = ((double) n) / cell->lateral_area;
    }
#if DEBUG_CELL
  fprintf (stderr, "cell_infestations: end\n");
#endif
}

#endif
