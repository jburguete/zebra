/**
 * \file temperature.h
 * \brief header file to define the temperatures.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#ifndef TEMPERATURE__H
#define TEMPERATURE__H 1

/**
 * \struct Temperature
 * \brief struct to define the temperatures.
 */
typedef struct
{
  double *time;                 ///< array of times.
  double *temperature;          ///< array of temperatures.
  unsigned int ntimes;          ///< number of temperature times.
} Temperature;

extern double current_time;
extern double current_temperature;

void temperature_destroy (Temperature * temperature);
int temperature_open_xml (Temperature * temperature, xmlNode * node,
                          char *directory);

/**
 * function to calculate the maximum time allowed by a temperature.
 *
 * \return maximum allowed time in seconds since 1970.
 */
static inline double
temperature_maximum_time (Temperature *temperature,
                          ///< pointer to the temperature struct data.
                          double maximum_time)
                          ///< maximum allowed time in seconds since 1970.
{
  double *date;
  double t, tmax;
  unsigned int k, last, n;
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_maximum_time: start\n");
#endif
  t = tmax = maximum_time;
  date = temperature->time;
  n = temperature->ntimes;
  if (!n)
    goto end;
  last = n - 1;
  if (current_time >= date[last])
    goto end;
  k = array_search (current_time, date, n);
  while (k < last)
    {
      t = fmin (tmax, date[k + 1]);
#if DEBUG_TEMPERATURE
      fprintf (stderr,
               "temperature_maximum_time: tmax=%.19lg t=%.19lg\n",
               t, current_time);
#endif
      if (t <= current_time)
        ++k;
      else
        break;
    }
end:
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_maximum_time: t=%lg\n", t);
  fprintf (stderr, "temperature_maximum_time: end\n");
#endif
  return t;
}

/**
 * function to set the variables on an temperature.
 */
static inline void
temperature_set (Temperature *temperature)
                 ///< pointer to the temperature struct data.
{
  unsigned int n;
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_set: start\n");
#endif
  n = temperature->ntimes;
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_set: ntimes=%u\n", n);
#endif
  if (n)
    current_temperature
      = array_interpolate (current_time, temperature->time,
                           temperature->temperature, n);
  else
    current_temperature = DEFAULT_TEMPERATURE;
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_set: end\n");
#endif
}

#endif
