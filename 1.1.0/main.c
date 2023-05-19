/**
 * \file main.c
 * \brief source file to define the main function.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <locale.h>
#include <libintl.h>
#include <math.h>
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
#include "junction.h"
#include "inlet.h"
#include "network.h"
#include "results.h"
#include "simulation.h"

///> enum to define the error code.
enum ErrorCode
{
  ERROR_CODE_NONE,              ///< none.
  ERROR_CODE_ARGUMENTS_NUMBER,  ///< bad command line arguments number.
  ERROR_CODE_SIMULATION,        ///< bad simulation input file.
  ERROR_CODE_RESULTS_BIN,       ///< bad results binary data base file.
  ERROR_CODE_RESULTS_XML,       ///< bad results configuration data base file.
};

/**
 * main function.
 *
 * \return error code.
 */
int
main (int argn,                 ///< number of command line arguments.
      char **argc)              ///< array of command line argument strings.
{
  union Input
  {
    Simulation simulation[1];
    Results results[1];
  } input;
  int error_code = ERROR_CODE_NONE;

#if DEBUG_MAIN
  fprintf (stderr, "main: start\n");
#endif

  // init locales and XML parser
#if DEBUG_MAIN
  fprintf (stderr, "main: init locales and XML parser\n");
#endif
  setlocale (LC_ALL, "");
  setlocale (LC_NUMERIC, "C");
  bindtextdomain (PROGRAM_NAME, LOCALES_DIR);
  bind_textdomain_codeset (PROGRAM_NAME, "UTF-8");
  textdomain (PROGRAM_NAME);
  xmlKeepBlanksDefault (0);

  // check arguments
#if DEBUG_MAIN
  fprintf (stderr, "main: start\n");
#endif
  switch (argn)
    {

      // simulation
    case 2:

#if DEBUG_MAIN
      fprintf (stderr, "main: open simulation\n");
#endif

      // open simulation
      if (!simulation_open_xml (input.simulation, argc[1]))
        {
          error_code = ERROR_CODE_SIMULATION;
          break;
        }
      simulation_run (input.simulation);

#if DEBUG_MAIN
      fprintf (stderr, "main: free memory\n");
#endif

      // free memory
      simulation_destroy (input.simulation);
      break;

      // results
    case 3:

#if DEBUG_MAIN
      fprintf (stderr, "main: read results data base file\n");
#endif


      // read results data base file
      if (!results_open_bin (input.results, argc[1]))
        {
          error_code = ERROR_CODE_RESULTS_BIN;
          break;
        }
      if (!results_open_xml (input.results, argc[2]))
        {
          error_code = ERROR_CODE_RESULTS_XML;
          results_destroy (input.results);
          break;
        }

#if DEBUG_MAIN
      fprintf (stderr, "main: free memory\n");
#endif

      // free memory
      results_destroy (input.results);
      break;

      // bad arguments number
    default:
      error_code = ERROR_CODE_ARGUMENTS_NUMBER;
      error_msg
        = (char *) g_strdup (_("The syntax is:\n./zebra simulation_file\nor:\n"
                               "./zebra data_base_file configuration_file\n"));
    }

  // error message
  if (error_code != ERROR_CODE_NONE)
    {
      printf ("%s\n%s\n", _("ERROR!"), error_msg);
      g_free (error_msg);
    }

  // end
#if DEBUG_MAIN
  fprintf (stderr, "main: end\n");
#endif
  return error_code;
}
