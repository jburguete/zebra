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
#include "tool.h"
#include "nutrient.h"
#include "species.h"
#include "point.h"
#include "cell.h"
#include "pipe.h"
#include "junction.h"

///> enum to define the error code.
enum ErrorCode
{
  ERROR_CODE_NONE,              ///< none.
  ERROR_CODE_ARGUMENTS_NUMBER,  ///< bad command line arguments number.
  ERROR_CODE_NUTRIENT,          ///< bad nutrients input file.
  ERROR_CODE_SPECIES,           ///< bad species input file.
};

char *error_msg = NULL;         ///< error message string.

/**
 * main function.
 *
 * \return error code.
 */
int
main (int argn,                 ///< number of command line arguments.
      char **argc)              ///< array of command line argument strings.
{
  int error_code = ERROR_CODE_NONE;

  // init locales and XML parser
  setlocale (LC_ALL, "");
  setlocale (LC_NUMERIC, "C");
  bindtextdomain (PROGRAM_NAME, LOCALES_DIR);
  bind_textdomain_codeset (PROGRAM_NAME, "UTF-8");
  textdomain (PROGRAM_NAME);
  xmlKeepBlanksDefault (0);

  // check arguments
  if (argn != 3)
    {
      error_code = ERROR_CODE_ARGUMENTS_NUMBER;
      error_msg
        = (char *) g_strdup (_("The syntax is:\n./zebra "
                               "nutrients_file species_file"));
      goto exit_on_error;
    }

  // open nutrients
  if (!nutrient_open_xml (argc[1]))
    {
      error_code = ERROR_CODE_NUTRIENT;
      goto exit_on_error;
    }

  // open species
  if (!species_open_xml (argc[2]))
    {
      error_code = ERROR_CODE_SPECIES;
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
  species_destroy ();
  nutrient_destroy ();

  // end
  return error_code;
}
