/**
 * \file inlet.h
 * \brief header file to define the inlets.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
 */
#ifndef INLET__H
#define INLET__H 1

///> type of inlet.
enum InletType
{
  INLET_TYPE_SET = 0,           ///< set the variable concentration.
  INLET_TYPE_INJECT = 1         ///< inject variable discharge.
};

/**
 * \struct Inlet
 * \brief struct to define the inlets.
 */
typedef struct
{
  double *solute_concentration[MAX_SOLUTES];
  ///< array of pointers to arrays of solute concentration.
  double *solute_time[MAX_SOLUTES];
  ///< array of pointers to arrays of solute time.
  double *species_concentration[MAX_SPECIES];
  ///< array of pointers to arrays of species concentration.
  double *species_time[MAX_SPECIES];
  ///< array of pointers to arrays of species time.
  double solute_input[MAX_SOLUTES];     ///< array of solute inputs.
  double species_input[MAX_SPECIES];    ///< array of species inputs.
  unsigned int nsolute_times[MAX_SOLUTES];
  ///< array of numbers of solute times.
  unsigned int nspecies_times[MAX_SPECIES];
  ///< array of numbers of species times.
  char id[MAX_LABEL_LENGTH];    ///< node identifier.
  Cell **cell;                  ///< array of pointers to cells.
  double input;                 ///< water input.
  double volume;                ///< volume of the cells.
  unsigned int ncells;          ///< number of cells.
  unsigned int type;            ///< type (0: set, 1: inject)
} Inlet;

void inlet_destroy (Inlet * inlet);
int inlet_open_xml (Inlet * inlet, xmlNode * node, Pipe * pipe,
                    unsigned int npipes, char *directory);

/**
 * function to init inlet variables.
 */
static inline void
inlet_init (Inlet *inlet)       ///< pointer to the inlet struct data.
{
  unsigned int i;
#if DEBUG_INLET
  fprintf (stderr, "inlet_init: start\n");
#endif
  inlet->input = 0.;
  for (i = 0; i < MAX_SOLUTES; ++i)
    inlet->solute_input[i] = 0.;
  for (i = 0; i < MAX_SPECIES; ++i)
    inlet->species_input[i] = 0.;
#if DEBUG_INLET
  fprintf (stderr, "inlet_init: end\n");
#endif
}

/**
 * function to calculate the maximum time allowed by an inlet.
 *
 * \return maximum allowed time in seconds since 1970.
 */
static inline double
inlet_maximum_time (Inlet *inlet,       ///< pointer to the inlet struct data.
                    double maximum_time)
                    ///< maximum allowed time in seconds since 1970.
{
  double *date;
  double t, tmax;
  unsigned int j, k, last, n;
#if DEBUG_INLET
  fprintf (stderr, "inlet_maximum_time: start\n");
#endif
  t = tmax = maximum_time;
  for (j = 0; j < MAX_SOLUTES; ++j)
    {
      date = inlet->solute_time[j];
      n = inlet->nsolute_times[j];
      if (!n)
        continue;
      last = n - 1;
      if (current_time >= date[last])
        continue;
      k = array_search (current_time, date, n);
      while (k < last)
        {
          t = fmin (tmax, date[k + 1]);
#if DEBUG_INLET
          fprintf (stderr,
                   "inlet_maximum_time: solute=%u tmax=%.19lg t=%.19lg\n",
                   j, t, current_time);
#endif
          if (t <= current_time)
            ++k;
          else
            break;
        }
      tmax = t;
    }
  for (j = 0; j < MAX_SPECIES; ++j)
    {
      date = inlet->species_time[j];
      n = inlet->nspecies_times[j];
      if (!n)
        continue;
      last = n - 1;
      if (current_time >= date[last])
        continue;
      k = array_search (current_time, date, n);
      while (k < last)
        {
          t = fmin (tmax, date[k + 1]);
#if DEBUG_INLET
          fprintf (stderr,
                   "inlet_maximum_time: species=%u tmax=%.19lg t=%.19lg\n",
                   j, t, current_time);
#endif
          if (t <= current_time)
            ++k;
          else
            break;
        }
      tmax = t;
    }
#if DEBUG_INLET
  fprintf (stderr, "inlet_maximum_time: t=%lg\n", t);
  fprintf (stderr, "inlet_maximum_time: end\n");
#endif
  return t;
}

/**
 * function to set the variables on an inlet.
 */
static inline void
inlet_set (Inlet *inlet,        ///< pointer to the inlet struct data.
           double initial_time, ///< initial time in seconds since 1970.
           double final_time)   ///< final time in seconds since 1970.
{
  Cell **cell;
  double *v, *t;
  double variable, volume, dt;
  unsigned int i, j, n, ncells;
#if DEBUG_INLET
  fprintf (stderr, "inlet_set: start\n");
#endif
  cell = inlet->cell;
  ncells = inlet->ncells;
  dt = final_time - initial_time;
  volume = cell[0]->discharge * dt;
  inlet->input += volume;
  for (i = 0; i < MAX_SOLUTES; ++i)
    {
      n = inlet->nsolute_times[i];
#if DEBUG_INLET
      fprintf (stderr, "inlet_set: solute=%u nsolute_times=%u\n", i, n);
#endif
      if (n)
        {
          v = inlet->solute_concentration[i];
          t = inlet->solute_time[i];
          if (inlet->type == INLET_TYPE_SET)
            {
              variable = array_interpolate (final_time, t, v, n);
              inlet->solute_input[i] += volume * variable;
            }
          else
            {
              variable = array_integrate (t, v, n, initial_time, final_time);
              inlet->solute_input[i] += variable;
              variable /= inlet->volume;
            }
        }
      else
        variable = 0.;
      if (inlet->type == INLET_TYPE_SET)
        for (j = 0; j < ncells; ++j)
          cell[j]->solute_concentration[i] = variable;
      else
        for (j = 0; j < ncells; ++j)
          cell[j]->solute_concentration[i] += variable;
    }
  for (i = 0; i < MAX_SPECIES; ++i)
    {
      n = inlet->nspecies_times[i];
#if DEBUG_INLET
      fprintf (stderr, "inlet_set: species=%u nspecies_times=%u\n", i, n);
#endif
      if (n)
        {
          v = inlet->species_concentration[i];
          t = inlet->species_time[i];
          variable = array_interpolate (final_time, t, v, n);
          inlet->species_input[i] += volume * variable;
        }
      else
        variable = 0.;
      for (j = 0; j < ncells; ++j)
        cell[j]->species_concentration[i] = variable;
    }
#if DEBUG_INLET
  fprintf (stderr, "inlet_set: end\n");
#endif
}

/**
 * function to write the inlet data in a summary file.
 */
static inline void
inlet_summary (Inlet *inlet,    ///< pointer to the inlet struct data.
               FILE *file)      ///< summary file.
{
  unsigned int i;
#if DEBUG_INLET
  fprintf (stderr, "inlet_summary: start\n");
#endif
  fprintf (file, "Inlet ID: %s input: %lg\n", inlet->id, inlet->input);
  for (i = 0; i < MAX_SOLUTES; ++i)
    fprintf (file, "solute: %s input: %lg\n",
             (char *) solute[i].name, inlet->solute_input[i]);
  for (i = 0; i < MAX_SPECIES; ++i)
    fprintf (file, "species: %s input: %lg\n",
             (char *) species[i].name, inlet->species_input[i]);
#if DEBUG_INLET
  fprintf (stderr, "inlet_summary: end\n");
#endif
}

#endif
