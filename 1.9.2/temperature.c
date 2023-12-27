/**
 * \file temperature.c
 * \brief source file to define the temperatures.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include <gsl/gsl_rng.h>
#include "config.h"
#include "tools.h"
#include "solute.h"
#include "species.h"
#include "specimen.h"
#include "point.h"
#include "cell.h"
#include "wall.h"
#include "pipe.h"
#include "temperature.h"

Temperature temperature[1];     ///< struct defining the temperatures.
double current_time;            ///< current time in seconds since 1970.
double current_temperature;     ///< current temperature.

/**
 * function to set an error message opening a temperatures input file.
 */
static inline void
temperature_error (const char *msg)     ///< error message.
{
  error_message (_("Temperature"), NULL, (char *) g_strdup (msg));
}

/**
 * function to free the memory used by the temperature data.
 */
void
temperature_destroy ()
{
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_destroy: start\n");
#endif
  free (temperature->temperature);
  free (temperature->time);
  temperature_null (temperature);
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_destroy: end\n");
#endif
}

/**
 * function to read the temperature data on a XML node.
 *
 * \return 1 on success, 0 on error.
 */
int
temperature_open_xml (const char *name) ///< file name.
{
  const char *m;

  // start
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_open_xml: start\n");
#endif

  // reading temperature
  if (!read_file (name, &temperature->temperature, &temperature->time,
                  &temperature->ntimes))
    {
      m = _("Bad temperature input file");
      goto exit_on_error;
    }

  // exit on success
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  temperature_error (m);

  // free memory on error
  temperature_destroy ();

  // end
#if DEBUG_TEMPERATURE
  fprintf (stderr, "temperature_open_xml: end\n");
#endif
  return 0;
}
