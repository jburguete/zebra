/**
 * \file inlet.h
 * \brief header file to define the inlets.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#ifndef INLET__H
#define INLET__H 1

/**
 * \struct Inlet
 * \brief struct to define the inlets.
 */
typedef struct
{
  char id[MAX_LABEL_LENGTH];    ///< node identifier.
  Cell **cell;                  ///< array of pointers to cells.
  double **solute_concentration;
  ///< array of pointers to arrays of solute concentration.
  double **solute_time;
  ///< array of pointers to arrays of solute time.
  double **species_concentration;
  ///< array of pointers to arrays of species concentration.
  double **species_time;
  ///< array of pointers to arrays of species time.
  unsigned int *nsolute_times;  ///< array of numbers of solute times.
  unsigned int *nspecies_times; ///< array of numbers of species times.
  unsigned int ncells;          ///< number of cells.
} Inlet;

void inlet_destroy (Inlet * inlet);
int inlet_open_xml (Inlet * inlet, xmlNode * node, Pipe * pipe,
                    unsigned int npipes, char *directory);

/**
 * function to calculate the maximum time allowed by an inlet.
 *
 * \return maximum allowed time in seconds since 1970.
 */
static inline double
inlet_maximum_time (Inlet * inlet,      ///< pointer to the inlet struct data.
                    double maximum_time)
                    ///< maximum allowed time in seconds since 1970.
{
  double *date;
  double t, tmax;
  unsigned int j, k, last, n;
#if DEBUG_INLET
  fprintf (stderr, "inlet_maximum_time: start\n");
#endif
  tmax = maximum_time;
  for (j = 0; j < nsolutes; ++j)
    {
      date = inlet->solute_time[j];
      n = inlet->nsolute_times[j];
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
  for (j = 0; j < nspecies; ++j)
    {
      date = inlet->species_time[j];
      n = inlet->nspecies_times[j];
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
inlet_set (Inlet * inlet,       ///< pointer to the inlet struct data.
           double time)         ///< time in seconds since 1970.
{
  Cell **cell;
  double *v, *t;
  double variable;
  unsigned int i, j, n, ncells;
#if DEBUG_INLET
  fprintf (stderr, "inlet_set: start\n");
#endif
  cell = inlet->cell;
  ncells = inlet->ncells;
  for (i = 0; i < nsolutes; ++i)
    {
      n = inlet->nsolute_times[i];
      v = inlet->solute_concentration[i];
      t = inlet->solute_time[i];
      variable = array_interpolate (time, t, v, n);
      for (j = 0; j < ncells; ++j)
        cell[j]->solute_concentration[i] = variable;
    }
  for (i = 0; i < nspecies; ++i)
    {
      n = inlet->nspecies_times[i];
      v = inlet->species_concentration[i];
      t = inlet->species_time[i];
      variable = array_interpolate (time, t, v, n);
      for (j = 0; j < ncells; ++j)
        cell[j]->species_concentration[i] = variable;
    }
#if DEBUG_INLET
  fprintf (stderr, "inlet_set: end\n");
#endif
}

#endif
