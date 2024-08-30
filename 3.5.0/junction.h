/**
 * \file junction.h
 * \brief header file to define the network junctions.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
 */
#ifndef JUNCTION__H
#define JUNCTION__H 1

/**
 * \struct Junction
 * \brief struct to define the junctions.
 */
typedef struct
{
  double solute_output[MAX_SOLUTES];    ///< array of solute outputs.
  double species_output[MAX_SPECIES];   ///< array of species larva outputs.
  char id[MAX_LABEL_LENGTH];    ///< identifier.
  Pipe **inlet;                 ///< array of inlet pipe pointers.
  Pipe **outlet;                ///< array of outlet pipe pointers.
  Cell **cell;                  ///< array of cell pointers.
  Point *point;                 ///< point pointer.
  double output;                ///< water output.
  double volume;                ///< total volume.
  double demand;                ///< water demand.
  unsigned int ninlets;         ///< number of inlet pipes.
  unsigned int noutlets;        ///< number of outlet pipes.
  unsigned int ncells;          ///< number of cells.
  unsigned int set;             ///< 1 on more of one inlet/outlet, 0 otherwise.
} Junction;

void junction_destroy (Junction * junction);

/**
 * function to init an empty junction.
 */
static inline void
junction_null (Junction *junction)
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
junction_add_inlet (Junction *junction,
                    ///< pointer to the junction struct data.
                    Pipe *pipe)
                    ///< pointer to the inlet pipe struct data.
{
  unsigned int n;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_add_inlet: start\n");
  fprintf (stderr, "junction_add_inlet: ID=%s\n", pipe->inlet_id);
#endif
  if (!junction->ninlets && !junction->noutlets)
    memcpy (junction->id, pipe->inlet_id, MAX_LABEL_LENGTH);
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
junction_add_outlet (Junction *junction,
                     ///< pointer to the junction struct data.
                     Pipe *pipe)
                     ///< pointer to the outlet pipe struct data.
{
  unsigned int n;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_add_outlet: start\n");
  fprintf (stderr, "junction_add_outlet: ID=%s\n", pipe->outlet_id);
#endif
  if (!junction->ninlets && !junction->noutlets)
    memcpy (junction->id, pipe->outlet_id, MAX_LABEL_LENGTH);
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
junction_init (Junction *junction)
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
  for (i = 0; i < MAX_SOLUTES; ++i)
    junction->solute_output[i] = 0.;
  for (i = 0; i < MAX_SPECIES; ++i)
    junction->species_output[i] = 0.;
  junction->output = volume = 0.;
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
 * function to set the demand on a junction.
 */
static inline void
junction_set_demand (Junction *junction)
                     ///< pointer to the junction struct data.
{
  Pipe **pipe;
  double demand;
  unsigned int i;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_set_demand: start\n");
#endif
  pipe = junction->inlet;
  for (i = 0, demand = 0.; i < junction->ninlets; ++i)
    demand -= pipe[i]->discharge;
  pipe = junction->outlet;
  for (i = 0; i < junction->noutlets; ++i)
    demand += pipe[i]->discharge;
  junction->demand = demand;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_set_demand: end\n");
#endif
}

/**
 * function to set the variables on a junction.
 */
static inline void
junction_set (Junction *junction)
              ///< pointer to the junction struct data.
{
  double nc[MAX_SOLUTES], sc[MAX_SPECIES];
  Cell *cell;
  unsigned int i, j, n;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_set: start\n");
#endif
  if (!junction->set)
    goto no_set;
  n = junction->ncells;
  for (j = 0; j < MAX_SOLUTES; ++j)
    nc[j] = 0.;
  for (j = 0; j < MAX_SPECIES; ++j)
    sc[j] = 0.;
  for (i = 0; i < n; ++i)
    {
      cell = junction->cell[i];
      for (j = 0; j < MAX_SOLUTES; ++j)
        nc[j] += cell->volume * cell->solute_concentration[j];
      for (j = 0; j < MAX_SPECIES; ++j)
        sc[j] += cell->volume * cell->species_concentration[j];
    }
  for (j = 0; j < MAX_SOLUTES; ++j)
    nc[j] /= junction->volume;
  for (j = 0; j < MAX_SPECIES; ++j)
    sc[j] /= junction->volume;
  for (i = 0; i < n; ++i)
    {
      cell = junction->cell[i];
      memcpy (cell->solute_concentration, nc, MAX_SOLUTES * sizeof (double));
      memcpy (cell->species_concentration, sc, MAX_SPECIES * sizeof (double));
    }
no_set:
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_set: end\n");
#endif
  return;
}

/**
 * function to set the variables and the outputs on a junction.
 */
static inline void
junction_set_with_outputs (Junction *junction,
                           ///< pointer to the junction struct data.
                           double time_step)    ///< time step size.
{
  double nc[MAX_SOLUTES], sc[MAX_SPECIES];
  Cell *cell;
  double v;
  unsigned int i, j, n;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_set_with_outputs: start\n");
#endif
  n = junction->ncells;
  for (j = 0; j < MAX_SOLUTES; ++j)
    nc[j] = 0.;
  for (j = 0; j < MAX_SPECIES; ++j)
    sc[j] = 0.;
  for (i = 0; i < n; ++i)
    {
      cell = junction->cell[i];
      for (j = 0; j < MAX_SOLUTES; ++j)
        nc[j] += cell->volume * cell->solute_concentration[j];
      for (j = 0; j < MAX_SPECIES; ++j)
        sc[j] += cell->volume * cell->species_concentration[j];
    }
  for (j = 0; j < MAX_SOLUTES; ++j)
    nc[j] /= junction->volume;
  for (j = 0; j < MAX_SPECIES; ++j)
    sc[j] /= junction->volume;
  if (junction->set)
    for (i = 0; i < n; ++i)
      {
        cell = junction->cell[i];
        memcpy (cell->solute_concentration, nc, MAX_SOLUTES * sizeof (double));
        memcpy (cell->species_concentration, sc, MAX_SPECIES * sizeof (double));
      }
  v = junction->demand * time_step;
  junction->output += v;
  for (j = 0; j < MAX_SOLUTES; ++j)
    junction->solute_output[j] += v * nc[j];
  for (j = 0; j < MAX_SPECIES; ++j)
    junction->species_output[j] += v * sc[j];
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_set_with_outputs: end\n");
#endif
  return;
}

/**
 * function to write the junction data in a summary file.
 */
static inline void
junction_summary (Junction *junction,
                  ///< pointer to the junction struct data.
                  FILE *file)   ///< summary file.
{
  unsigned int i;
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_summary: start\n");
#endif
  fprintf (file, "Junction ID: %s output: %lg\n",
           junction->id, junction->output);
  for (i = 0; i < MAX_SOLUTES; ++i)
    fprintf (file, "solute: %s output: %lg\n",
             (char *) solute[i].name, junction->solute_output[i]);
  for (i = 0; i < MAX_SPECIES; ++i)
    fprintf (file, "species: %s output: %lg\n",
             (char *) species[i].name, junction->species_output[i]);
#if DEBUG_JUNCTION
  fprintf (stderr, "junction_summary: end\n");
#endif
}

#endif
