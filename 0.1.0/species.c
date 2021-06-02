/**
 * \file species.c
 * \brief source file to define the species.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tool.h"
#include "species.h"

Species *species = NULL;        ///< array of species struct data.
unsigned int nspecies = 0;      ///< number of species.

/**
 * function to set an error message opening a species input file.
 */
static void
species_error (const char *msg) ///< error message.
{
  error_msg = (char *) g_strconcat (_("Species"), ":\n", msg, NULL);
}

/**
 * function to free the memory used by the species data.
 */
void
species_destroy ()
{
  unsigned int i;
#if DEBUG_SPECIES
  fprintf (stderr, "species_destroy: start\n");
#endif
  for (i = 0; i < nspecies; ++i)
    xmlFree ((xmlChar *) species[i].name);
  free (species);
  species = NULL;
  nspecies = 0;
#if DEBUG_SPECIES
  fprintf (stderr, "species_destroy: end\n");
#endif
}

/**
 * function to read the species data on a XML file.
 *
 * \return 1 on succes, 0 on error.
 */
int
species_open_xml (char *file_name)      ///< input file name.
{
  Species *s;
  xmlDoc *doc;
  xmlNode *node;
  int error_code = 0;
  unsigned int i;

  // start
#if DEBUG_SPECIES
  fprintf (stderr, "species_open_xml: start\n");
#endif

  // open file
  doc = xmlParseFile (file_name);
  if (!doc)
    {
      species_error (_("Bad input file"));
      goto exit_on_error;
    }

  // open root XML node
  node = xmlDocGetRootElement (doc);
  if (!node)
    {
      species_error (_("No root XML node"));
      goto exit_on_error;
    }
  if (xmlStrcmp (node->name, XML_SPECIES))
    {
      species_error (_("Bad root XML node"));
      goto exit_on_error;
    }

  // reading species
  for (node = node->children; node; node = node->next)
    {
      if (xmlStrcmp (node->name, XML_SPECIES))
        {
          species_error (_("Bad XML node"));
          goto exit_on_error;
        }
      i = nspecies;
      ++nspecies;
      species = (Species *) realloc (species, nspecies * sizeof (Species));
      s = species + i;
      s->name = (char *) xmlGetProp (node, XML_NAME);
      if (!s->name)
        {
          species_error (_("Bad name"));
          goto exit_on_error;
        }
    }

  if (!nspecies)
    {
      species_error (_("No species"));
      goto exit_on_error;
    }

  // success
  error_code = 1;

exit_on_error:

  // free memory on error
  if (!error_code)
    species_destroy ();

  // end
#if DEBUG_SPECIES
  fprintf (stderr, "species_open_xml: end\n");
#endif
  return error_code;
}
