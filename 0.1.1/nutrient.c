/**
 * \file nutrient.c
 * \brief source file to define the nutrients.
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

Nutrient *nutrient = NULL;      ///< array of nutrient struct data.
unsigned int nnutrients = 0;    ///< number of nutrients.

/**
 * function to set an error message opening a nutrients input file.
 */
static void
nutrient_error (const char *msg)        ///< error message.
{
  error_msg = (char *) g_strconcat (_("Nutrient"), ":\n", msg, NULL);
}

/**
 * function to free the memory used by the nutrients data.
 */
void
nutrient_destroy ()
{
  unsigned int i;
#if DEBUG_NUTRIENT
  fprintf (stderr, "nutrient_destroy: start\n");
#endif
  for (i = 0; i < nnutrients; ++i)
    xmlFree ((xmlChar *) nutrient[i].name);
  free (nutrient);
  nutrient = NULL;
  nnutrients = 0;
#if DEBUG_NUTRIENT
  fprintf (stderr, "nutrient_destroy: end\n");
#endif
}

/**
 * function to read the nutrients data on a XML file.
 *
 * \return 1 on succes, 0 on error.
 */
int
nutrient_open_xml (char *file_name)     ///< input file name.
{
  Nutrient *n;
  xmlDoc *doc;
  xmlNode *node;
  int error_code = 0;
  unsigned int i;

  // start
#if DEBUG_NUTRIENT
  fprintf (stderr, "nutrient_open_xml: start\n");
#endif

  // open file
  doc = xmlParseFile (file_name);
  if (!doc)
    {
      nutrient_error (_("Bad input file"));
      goto exit_on_error;
    }

  // open root XML node
  node = xmlDocGetRootElement (doc);
  if (!node)
    {
      nutrient_error (_("No root XML node"));
      goto exit_on_error;
    }
  if (xmlStrcmp (node->name, XML_NUTRIENT))
    {
      nutrient_error (_("Bad root XML node"));
      goto exit_on_error;
    }

  // reading nutrient
  for (node = node->children; node; node = node->next)
    {
      if (xmlStrcmp (node->name, XML_NUTRIENT))
        {
          nutrient_error (_("Bad XML node"));
          goto exit_on_error;
        }
      i = nnutrients;
      ++nnutrients;
      nutrient
        = (Nutrient *) realloc (nutrient, nnutrients * sizeof (Nutrient));
      n = nutrient + i;
      n->name = (char *) xmlGetProp (node, XML_NAME);
      if (!n->name)
        {
          nutrient_error (_("Bad name"));
          goto exit_on_error;
        }
    }

  if (!nnutrients)
    {
      nutrient_error (_("No nutrients"));
      goto exit_on_error;
    }

  // success
  error_code = 1;

exit_on_error:

  // free memory on error
  if (!error_code)
    nutrient_destroy ();

  // end
#if DEBUG_NUTRIENT
  fprintf (stderr, "nutrient_open_xml: end\n");
#endif
  return error_code;
}
