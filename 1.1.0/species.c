/**
 * \file species.c
 * \brief source file to define the species.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <math.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"
#include "solute.h"
#include "species.h"

Species *species;               ///< array of species struct data.
GHashTable *hash_species;       ///< hash table of species names.
unsigned int nspecies;          ///< number of species.
unsigned int flags_species;     ///< species flags.

/**
 * function to set an error message opening a species input file.
 */
static inline void
species_error (Species * s,     ///< species struct data.
               const char *msg) ///< error message.
{
  char *name;
  name = NULL;
  if (s && s->name)
    name = (char *) s->name;
  error_message (_("Species"), name, (char *) g_strdup (msg));
}

/**
 * function to init null species data.
 */
static void
species_null ()
{
#if DEBUG_SPECIES
  fprintf (stderr, "species_null: start\n");
#endif
  species = NULL;
  hash_species = NULL;
  nspecies = flags_species = 0;
#if DEBUG_SPECIES
  fprintf (stderr, "species_null: end\n");
#endif
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
    xmlFree (species[i].name);
  free (species);
  g_hash_table_destroy (hash_species);
  species_null ();
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
  species_null ();
  s = NULL;

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
      i = nspecies;
      ++nspecies;
      species = (Species *) realloc (species, nspecies * sizeof (Species));
      s = species + i;
      if (xmlStrcmp (node->name, XML_SPECIES))
        {
          m = _("Bad XML node");
          s->name = NULL;
          goto exit_on_error;
        }
      s->name = xmlGetProp (node, XML_NAME);
      if (!s->name)
        {
          m = _("Bad name");
          goto exit_on_error;
        }
      if (hash_species && g_hash_table_contains (hash_species, s->name))
        {
          m = _("Duplicated name");
          goto exit_on_error;
        }
      if (!xmlStrcmp (s->name, XML_ZEBRA_MUSSEL))
        s->type = SPECIES_TYPE_ZEBRA_MUSSEL;
      else
        {
          m = _("Unknown");
          goto exit_on_error;
        }
      flags_species |= s->type;
      s->maximum_velocity = xml_node_get_float (node, XML_MAXIMUM_VELOCITY, &k);
      if (!k || s->maximum_velocity < 0.)
        {
          m = _("Bad maximum velocity");
          goto exit_on_error;
        }
      s->cling = xml_node_get_float (node, XML_CLING, &k);
      if (!k || s->cling < 0.)
        {
          m = _("Bad cling coefficient");
          goto exit_on_error;
        }
      s->minimum_oxygen = xml_node_get_float (node, XML_MINIMUM_OXYGEN, &k);
      if (!k || s->minimum_oxygen < 0.)
        {
          m = _("Bad minimum oxygen");
          goto exit_on_error;
        }
      s->respiration = xml_node_get_float (node, XML_RESPIRATION, &k);
      if (!k || s->respiration < 0.)
        {
          m = _("Bad respiration coefficient");
          goto exit_on_error;
        }
      s->eat = xml_node_get_float (node, XML_EAT, &k);
      if (!k || s->eat < 0.)
        {
          m = _("Bad eat coefficient");
          goto exit_on_error;
        }
      s->grow = xml_node_get_float (node, XML_GROW, &k);
      if (!k || s->grow < 0.)
        {
          m = _("Bad grow coefficient");
          goto exit_on_error;
        }
      s->decay = xml_node_get_float (node, XML_DECAY, &k);
      if (!k || s->decay < 0.)
        {
          m = _("Bad decay coefficient");
          goto exit_on_error;
        }
      s->minimum_temperature
        = xml_node_get_float (node, XML_MINIMUM_TEMPERATURE, &k);
      if (!k)
        {
          m = _("Bad minimum temperature");
          goto exit_on_error;
        }
      s->optimal_temperature
        = xml_node_get_float (node, XML_OPTIMAL_TEMPERATURE, &k);
      if (!k || s->optimal_temperature < s->minimum_temperature)
        {
          m = _("Bad optimal temperature");
          goto exit_on_error;
        }
      s->maximum_temperature
        = xml_node_get_float (node, XML_MAXIMUM_TEMPERATURE, &k);
      if (!k || s->maximum_temperature < s->optimal_temperature)
        {
          m = _("Bad maximum temperature");
          goto exit_on_error;
        }
      s->minimum_chlorine = xml_node_get_float (node, XML_MINIMUM_CHLORINE, &k);
      if (!k || s->minimum_chlorine < 0.)
        {
          m = _("Bad minimum chlorine");
          goto exit_on_error;
        }
      s->maximum_chlorine = xml_node_get_float (node, XML_MAXIMUM_CHLORINE, &k);
      if (!k || s->maximum_chlorine < s->minimum_chlorine)
        {
          m = _("Bad maximum chlorine");
          goto exit_on_error;
        }
      s->decay_chlorine = xml_node_get_float (node, XML_DECAY_CHLORINE, &k);
      if (!k || s->decay_chlorine < 0.)
        {
          m = _("Bad chlorine decay");
          goto exit_on_error;
        }
      s->minimum_hydrogen_peroxide
        = xml_node_get_float (node, XML_MINIMUM_HYDROGEN_PEROXIDE, &k);
      if (!k || s->minimum_hydrogen_peroxide < 0.)
        {
          m = _("Bad minimum hydrogen peroxide");
          goto exit_on_error;
        }
      s->maximum_hydrogen_peroxide
        = xml_node_get_float (node, XML_MAXIMUM_HYDROGEN_PEROXIDE, &k);
      if (!k || s->maximum_hydrogen_peroxide < s->minimum_hydrogen_peroxide)
        {
          m = _("Bad maximum hydrogen peroxide");
          goto exit_on_error;
        }
      s->decay_hydrogen_peroxide
        = xml_node_get_float (node, XML_DECAY_HYDROGEN_PEROXIDE, &k);
      if (!k || s->decay_hydrogen_peroxide < 0.)
        {
          m = _("Bad hydrogen peroxide decay");
          goto exit_on_error;
        }
      g_hash_table_insert (hash_species, s->name, s);
      if (nspecies == MAX_SPECIES)
        break;
    }

  if (!nspecies)
    {
      m = _("No species");
      goto exit_on_error;
    }

  xmlFreeDoc (doc);

  // exit on success
#if DEBUG_SPECIES
  fprintf (stderr, "species_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  species_error (s, m);

  // free memory on error
  xmlFreeDoc (doc);
  species_destroy ();

  // end
#if DEBUG_SPECIES
  fprintf (stderr, "species_open_xml: end\n");
#endif
  return 0;
}
