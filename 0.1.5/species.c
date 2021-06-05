/**
 * \file species.c
 * \brief source file to define the species.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"
#include "species.h"

Species *species = NULL;        ///< array of species struct data.
unsigned int nspecies = 0;      ///< number of species.

/**
 * function to set an error message opening a species input file.
 */
static inline void
species_error (const char *msg) ///< error message.
{
  error_message (_("Species"), (char *) g_strdup (msg));
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
  const char *m;
  int k;
  unsigned int i;

  // start
#if DEBUG_SPECIES
  fprintf (stderr, "species_open_xml: start\n");
#endif

  // open file
  doc = xmlParseFile (file_name);
  if (!doc)
    {
      m = _("Bad input file");
      goto exit_on_error;
    }

  // open root XML node
  node = xmlDocGetRootElement (doc);
  if (!node)
    {
      m = _("No root XML node");
      goto exit_on_error;
    }
  if (xmlStrcmp (node->name, XML_SPECIES))
    {
      m = _("Bad root XML node");
      goto exit_on_error;
    }

  // reading species
  for (node = node->children; node; node = node->next)
    {
      if (xmlStrcmp (node->name, XML_SPECIES))
        {
          m = _("Bad XML node");
          goto exit_on_error;
        }
      i = nspecies;
      ++nspecies;
      species = (Species *) realloc (species, nspecies * sizeof (Species));
      s = species + i;
      s->name = (char *) xmlGetProp (node, XML_NAME);
      if (!s->name)
        {
          m = _("Bad name");
          goto exit_on_error;
        }
      s->decay = xml_node_get_float_with_default (node, XML_DECAY, &k, 0.);
      if (!k)
        {
          m = _("Bad decay coefficient");
          goto exit_on_error;
        }
      s->grow = xml_node_get_float_with_default (node, XML_GROW, &k, 0.);
      if (!k)
        {
          m = _("Bad grow coefficient");
          goto exit_on_error;
        }
      s->cling = xml_node_get_float_with_default (node, XML_CLING, &k, 0.);
      if (!k)
        {
          m = _("Bad cling coefficient");
          goto exit_on_error;
        }
      s->eat = xml_node_get_float_with_default (node, XML_EAT, &k, 0.);
      if (!k)
        {
          m = _("Bad eat coefficient");
          goto exit_on_error;
        }
      s->maximum_velocity
        = xml_node_get_float_with_default (node, XML_MAXIMUM_VELOCITY, &k, 0.);
      if (!k)
        {
          m = _("Bad maximum velocity");
          goto exit_on_error;
        }
      if (nspecies == MAX_SPECIES)
        break;
    }

  if (!nspecies)
    {
      m = _("No species");
      goto exit_on_error;
    }

  // exit on success
#if DEBUG_SPECIES
  fprintf (stderr, "species_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  species_error (m);

  // free memory on error
  species_destroy ();

  // end
#if DEBUG_SPECIES
  fprintf (stderr, "species_open_xml: end\n");
#endif
  return 0;
}

/**
 * function to find the index of a species on the species array.
 *
 * \return species index on success, nspecies on error.
 */
unsigned int
species_index (const char *name)        ///< species name.
{
  unsigned int i;
#if DEBUG_SPECIES
  fprintf (stderr, "species_index: start\n");
#endif
  for (i = 0; i < nspecies; ++i)
    if (!strcmp (species[i].name, name))
      break;
#if DEBUG_SPECIES
  fprintf (stderr, "species_index: end\n");
#endif
  return i;
}
