/**
 * \file inlet.c
 * \brief source file to define the inlets.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"
#include "nutrient.h"
#include "species.h"
#include "inlet.h"

Inlet *inlet = NULL;            ///< array of inlet struct data.
unsigned int ninlets = 0;       ///< number of inlets.

/**
 * function to set an error message opening a inlets input file.
 */
static void
inlet_error (const char *msg)   ///< error message.
{
  error_msg = (char *) g_strconcat (_("Inlet"), ":\n", msg, NULL);
}

/**
 * function to free the memory used by the inlets data.
 */
void
inlet_destroy ()
{
  Inlet *in;
  unsigned int i, j;
#if DEBUG_INLET
  fprintf (stderr, "inlet_destroy: start\n");
#endif
  for (j = 0; j < ninlets; ++j)
    {
      in = inlet + j;
      if (in->nutrient_concentration)
        {
          for (i = 0; i < nnutrients; ++i)
            free (in->nutrient_concentration[i]);
          free (in->nutrient_concentration);
        }
      if (in->nutrient_time)
        {
          for (i = 0; i < nnutrients; ++i)
            free (in->nutrient_time[i]);
          free (in->nutrient_time);
        }
      if (in->species_concentration)
        {
          for (i = 0; i < nspecies; ++i)
            free (in->species_concentration[i]);
          free (in->species_concentration);
        }
      if (in->species_time)
        {
          for (i = 0; i < nspecies; ++i)
            free (in->species_time[i]);
          free (in->species_time);
        }
    }
  free (inlet);
  inlet = NULL;
  ninlets = 0;
#if DEBUG_INLET
  fprintf (stderr, "inlet_destroy: end\n");
#endif
}

/**
 * function to read the inlets data on a XML file.
 *
 * \return 1 on succes, 0 on error.
 */
int
inlet_open_xml (char *file_name)        ///< input file name.
{
  Inlet *in;
  xmlDoc *doc;
  xmlNode *node, *child;
  int error_code = 0;
  unsigned int i, j;

  // start
#if DEBUG_INLET
  fprintf (stderr, "inlet_open_xml: start\n");
#endif

  // open file
  doc = xmlParseFile (file_name);
  if (!doc)
    {
      inlet_error (_("Bad input file"));
      goto exit_on_error;
    }

  // open root XML node
  node = xmlDocGetRootElement (doc);
  if (!node)
    {
      inlet_error (_("No root XML node"));
      goto exit_on_error;
    }
  if (xmlStrcmp (node->name, XML_INLET))
    {
      inlet_error (_("Bad root XML node"));
      goto exit_on_error;
    }

  // reading inlet
  for (node = node->children; node; node = node->next)
    {
      if (xmlStrcmp (node->name, XML_INLET))
        {
          inlet_error (_("Bad XML node"));
          goto exit_on_error;
        }
      j = ninlets;
      ++ninlets;
      inlet = (Inlet *) realloc (inlet, ninlets * sizeof (Inlet));
      in = inlet + j;
      child = node->children;
    }

  if (!ninlets)
    {
      inlet_error (_("No inlets"));
      goto exit_on_error;
    }

  // success
  error_code = 1;

exit_on_error:

  // free memory on error
  if (!error_code)
    inlet_destroy ();

  // end
#if DEBUG_INLET
  fprintf (stderr, "inlet_open_xml: end\n");
#endif
  return error_code;
}
