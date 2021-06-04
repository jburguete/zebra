/**
 * \file main.c
 * \brief source file to define the main function.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <locale.h>
#include <libintl.h>
#include <math.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"
#include "nutrient.h"
#include "species.h"
#include "point.h"
#include "cell.h"
#include "pipe.h"
#include "junction.h"
#include "inlet.h"
#include "outlet.h"
#include "network.h"

///> enum to define the error code.
enum ErrorCode
{
  ERROR_CODE_NONE,              ///< none.
  ERROR_CODE_ARGUMENTS_NUMBER,  ///< bad command line arguments number.
  ERROR_CODE_NUTRIENT,          ///< bad nutrients input file.
  ERROR_CODE_SPECIES,           ///< bad species input file.
  ERROR_CODE_NETWORK,           ///< bad network input file.
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
  Network network[1];
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

  // init variables
  network_null (network);

  // check arguments
#if DEBUG_MAIN
  fprintf (stderr, "main: start\n");
#endif
  if (argn != 4)
    {
      error_code = ERROR_CODE_ARGUMENTS_NUMBER;
      error_msg
        = (char *) g_strdup (_("The syntax is:\n./zebra "
                               "nutrients_file species_file network_file"));
      goto exit_on_error;
    }

  // open nutrients
#if DEBUG_MAIN
  fprintf (stderr, "main: open nutrients\n");
#endif
  if (!nutrient_open_xml (argc[1]))
    {
      error_code = ERROR_CODE_NUTRIENT;
      goto exit_on_error;
    }

  // open species
#if DEBUG_MAIN
  fprintf (stderr, "main: open species\n");
#endif
  if (!species_open_xml (argc[2]))
    {
      error_code = ERROR_CODE_SPECIES;
      goto exit_on_error;
    }

  // open network
#if DEBUG_MAIN
  fprintf (stderr, "main: open network\n");
#endif
  if (!network_open_xml (network, argc[3]))
    {
      error_code = ERROR_CODE_NETWORK;
      goto exit_on_error;
    }

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
  network_destroy (network);
  species_destroy ();
  nutrient_destroy ();

  // end
#if DEBUG_MAIN
  fprintf (stderr, "main: end\n");
#endif
  return error_code;
}
