/**
 * \file main.c
 * \brief source file to define the main function.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <locale.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"
#include "nutrient.h"
#include "species.h"
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
  Simulation simulation[1];
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
  if (argn != 2)
    {
      error_code = ERROR_CODE_ARGUMENTS_NUMBER;
      error_msg
        = (char *) g_strdup (_("The syntax is:\n./zebra simulation_file"));
      goto exit_on_error;
    }

  // open simulation
#if DEBUG_MAIN
  fprintf (stderr, "main: open simulation\n");
#endif
  if (!simulation_open_xml (simulation, argc[1]))
    {
      error_code = ERROR_CODE_SIMULATION;
      goto exit_on_error;
    }
  simulation_run (simulation);

  // error code
exit_on_error:
  if (error_code != ERROR_CODE_NONE)
    {
      printf ("%s\n%s\n", _("ERROR!"), error_msg);
      g_free (error_msg);
    }

  // free memory
#if DEBUG_MAIN
  fprintf (stderr, "main: free memory\n");
#endif
  simulation_destroy (simulation);

  // end
#if DEBUG_MAIN
  fprintf (stderr, "main: end\n");
#endif
  return error_code;
}
