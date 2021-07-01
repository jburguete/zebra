/**
 * \file junction.h
 * \brief header file to define the network junctions.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef JUNCTION__H
#define JUNCTION__H 1

/**
 * \struct Junction
 * \brief struct to define the junctions.
 */
typedef struct
{
  Pipe **inlet;                 ///< array of inlet pipe pointers.
  Pipe **outlet;                ///< array of outlet pipe pointers.
  Cell **cell;                  ///< array of cell pointers.
  Point *point;                 ///< Point pointer.
  double volume;
  unsigned int ninlets;         ///< number of inlet pipes.
  unsigned int noutlets;        ///< number of outlet pipes.
  unsigned int ncells;
} Junction;

void junction_destroy (Junction * junction);

/**
 * function to init an empty junction.
 */
static inline void
junction_null (Junction * junction)
               ///< pointer to the junction struct data.
{
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_null: start\n");
#endif
  junction->inlet = junction->outlet = NULL;
  junction->point = NULL;
  junction->cell = NULL;
  junction->ninlets = junction->noutlets = 0;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_null: end\n");
#endif
}

/**
 * function to add an inlet pipe to a junction.
 */
static inline void
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
static inline void
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

/**
 * function to init the variables on a junction.
 */
static inline void
junction_init (Junction * junction)
               ///< pointer to the junction struct data.
{
  Pipe *pipe, *pipe2;
  Cell *cell;
  double volume, length;
  unsigned int i, j, ninlets, noutlets;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_init: start\n");
#endif
  ninlets = junction->ninlets;
  noutlets = junction->noutlets;
  junction->ncells = ninlets + noutlets;
  junction->cell = (Cell **) malloc (junction->ncells * sizeof (Cell *));
  volume = 0.;
  for (i = 0; i < ninlets; ++i)
    {
      pipe = junction->inlet[i];
      junction->cell[i] = cell = pipe->cell;
      volume += cell->volume;
    }
  for (j = 0; j < noutlets; ++j, ++i)
    {
      pipe = junction->outlet[j];
      junction->cell[i] = cell = pipe->cell + pipe->ncells - 1;
      volume += cell->volume;
    }
  junction->volume = volume;
  if (junction->ncells == 2)
    switch (junction->ninlets)
      {
      case 0:
        pipe = junction->outlet[0];
        pipe2 = junction->outlet[1];
        length = RECIRCULATION_LENGTH * (pipe2->radius - pipe->radius);
        if (length > 0.)
          pipe_outlet_add_recirculation (pipe, length);
        else if (length < 0.)
          pipe_outlet_add_recirculation (pipe2, -length);
        break;
      case 2:
        pipe = junction->inlet[0];
        pipe2 = junction->inlet[1];
        length = RECIRCULATION_LENGTH * (pipe2->radius - pipe->radius);
        if (length > 0.)
          pipe_inlet_add_recirculation (pipe, length);
        else if (length < 0.)
          pipe_inlet_add_recirculation (pipe2, -length);
        break;
      default:
        pipe = junction->inlet[0];
        pipe2 = junction->outlet[0];
        length = RECIRCULATION_LENGTH * (pipe2->radius - pipe->radius);
        if (length > 0.)
          pipe_inlet_add_recirculation (pipe, length);
        else if (length < 0.)
          pipe_outlet_add_recirculation (pipe2, -length);
      }
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_init: end\n");
#endif
}

/**
 * function to set the variables on a junction.
 */
static inline void
junction_set (Junction * junction)
              ///< pointer to the junction struct data.
{
  double nc[MAX_NUTRIENTS], sc[MAX_SPECIES];
  Cell *cell;
  unsigned int i, j, n;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_set: start\n");
#endif
  n = junction->ncells;
  for (j = 0; j < nnutrients; ++j)
    nc[j] = 0.;
  for (j = 0; j < nspecies; ++j)
    sc[j] = 0.;
  for (i = 0; i < n; ++i)
    {
      cell = junction->cell[i];
      for (j = 0; j < nnutrients; ++j)
        nc[j] += cell->volume * cell->nutrient_concentration[j];
      for (j = 0; j < nspecies; ++j)
        sc[j] += cell->volume * cell->species_concentration[j];
    }
  for (j = 0; j < nnutrients; ++j)
    nc[j] /= junction->volume;
  for (j = 0; j < nspecies; ++j)
    sc[j] /= junction->volume;
  for (i = 0; i < n; ++i)
    {
      cell = junction->cell[i];
      memcpy (cell->nutrient_concentration, nc, nnutrients * sizeof (double));
      memcpy (cell->species_concentration, sc, nspecies * sizeof (double));
    }
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_set: end\n");
#endif
}

#endif
