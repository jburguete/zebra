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

extern Temperature temperature[1];
extern double current_time;
extern double current_temperature;

void temperature_destroy ();
int temperature_open_xml (const char *name);

/**
 * function to init an empty temperature.
 */
static inline void
temperature_null ()
{
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_null: start\n");
#endif
  temperature->ntimes = 0;
  temperature->temperature = temperature->time = NULL;
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_null: end\n");
#endif
}

/**
 * function to set the variables on an temperature.
 */
static inline void
temperature_set (double t)      ///< current time in seconds since 1970.
{
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_set: start\n");
  fprintf (stderr, "temperature_set: ntimes=%u\n", temperature->ntimes);
#endif
  if (temperature->ntimes)
    current_temperature
      = array_interpolate (t, temperature->time, temperature->temperature,
                           temperature->ntimes);
  else
    current_temperature = DEFAULT_TEMPERATURE;
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_set: current_temperature=%lg\n",
           current_temperature);
  fprintf (stderr, "temperature_set: end\n");
#endif
}

#endif
