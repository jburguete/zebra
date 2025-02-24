/**
 * \file species.c
 * \brief source file to define the species.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"
#include "solute.h"
#include "species.h"

Species species[MAX_SPECIES];   ///< array of species struct data.

/**
 * function to set an error message opening a species input file.
 */
static inline void
species_error (Species *s,      ///< species struct data.
               const char *msg) ///< error message.
{
  const char *name;
  name = NULL;
  if (s && s->name)
    name = (const char *) s->name;
  error_message (_("Species"), name, (char *) g_strdup (msg));
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
  for (i = 0; i < MAX_SPECIES; ++i)
    xmlFree (species[i].name);
#if DEBUG_SPECIES
  fprintf (stderr, "species_destroy: end\n");
#endif
}

/**
 * function to get the index of a species.
 *
 * \return species index.
 */
unsigned int
species_index (const xmlChar *name)     ///< species name.
{
  unsigned int i;
#if DEBUG_SOLUTE
  fprintf (stderr, "species_index: start\n");
#endif
  if (!xmlStrcmp (name, XML_ZEBRA_MUSSEL))
    i = SPECIES_TYPE_ZEBRA_MUSSEL;
  else
    i = MAX_SPECIES;
#if DEBUG_SOLUTE
  fprintf (stderr, "species_index: end\n");
#endif
  return i;
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
  xmlChar *name;
  GHashTable *hash;
  const char *m;
  int k;
  unsigned int i;

  // start
#if DEBUG_SPECIES
  fprintf (stderr, "species_open_xml: start\n");
#endif
  for (i = 0; i < MAX_SPECIES; ++i)
    species[i].name = NULL;
  hash = NULL;

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
  hash = g_hash_table_new (g_str_hash, g_str_equal);
  for (node = node->children; node; node = node->next)
    {
      s = NULL;
      if (xmlStrcmp (node->name, XML_SPECIES))
        {
          m = _("Bad XML node");
          goto exit_on_error;
        }
      name = xmlGetProp (node, XML_NAME);
      if (!name)
        {
          m = _("Bad name");
          goto exit_on_error;
        }
      if (g_hash_table_contains (hash, name))
        {
          m = _("Duplicated name");
          xmlFree (name);
          goto exit_on_error;
        }
      i = species_index (name);
      if (i == MAX_SPECIES)
        {
          m = _("Unknown");
          xmlFree (name);
          goto exit_on_error;
        }
      s = species + i;
      s->name = name;
      s->type = i;
      s->juvenile_age
        = xml_node_get_float_with_default (node, XML_JUVENILE_AGE, &k, 0.);
      if (!k || s->juvenile_age < 0.)
        {
          m = _("Bad juvenile age");
          goto exit_on_error;
        }
      s->maximum_velocity
        = xml_node_get_float_with_default (node, XML_MAXIMUM_VELOCITY, &k, 0.);
      if (!k || s->maximum_velocity < 0.)
        {
          m = _("Bad maximum velocity");
          goto exit_on_error;
        }
      s->settlement
        = xml_node_get_float_with_default (node, XML_SETTLEMENT, &k, 0.);
      if (!k || s->settlement < 0.)
        {
          m = _("Bad settlement coefficient");
          goto exit_on_error;
        }
      s->minimum_oxygen
        = xml_node_get_float_with_default (node, XML_MINIMUM_OXYGEN, &k, 0.);
      if (!k || s->minimum_oxygen < 0.)
        {
          m = _("Bad minimum oxygen");
          goto exit_on_error;
        }
      s->respiration
        = xml_node_get_float_with_default (node, XML_RESPIRATION, &k, 0.);
      if (!k || s->respiration < 0.)
        {
          m = _("Bad respiration coefficient");
          goto exit_on_error;
        }
      s->eat = xml_node_get_float_with_default (node, XML_EAT, &k, 0.);
      if (!k || s->eat < 0.)
        {
          m = _("Bad eat coefficient");
          goto exit_on_error;
        }
      s->grow = xml_node_get_float_with_default (node, XML_GROW, &k, 0.);
      if (!k || s->grow < 0.)
        {
          m = _("Bad grow coefficient");
          goto exit_on_error;
        }
      s->adult_decay
        = xml_node_get_float_with_default (node, XML_ADULT_DECAY, &k, 0.);
      if (!k || s->adult_decay < 0.)
        {
          m = _("Bad adult decay coefficient");
          goto exit_on_error;
        }
      s->larva_decay
        = xml_node_get_float_with_default (node, XML_LARVA_DECAY, &k, 0.);
      if (!k || s->larva_decay < 0.)
        {
          m = _("Bad larva decay coefficient");
          goto exit_on_error;
        }
      s->minimum_temperature
        = xml_node_get_float_with_default (node, XML_MINIMUM_TEMPERATURE, &k,
                                           0.);
      if (!k)
        {
          m = _("Bad minimum temperature");
          goto exit_on_error;
        }
      s->optimal_temperature
        = xml_node_get_float_with_default (node, XML_OPTIMAL_TEMPERATURE, &k,
                                           0.);
      if (!k || s->optimal_temperature < s->minimum_temperature)
        {
          m = _("Bad optimal temperature");
          goto exit_on_error;
        }
      s->maximum_temperature
        = xml_node_get_float_with_default (node, XML_MAXIMUM_TEMPERATURE, &k,
                                           0.);
      if (!k || s->maximum_temperature < s->optimal_temperature)
        {
          m = _("Bad maximum temperature");
          goto exit_on_error;
        }
      s->larva_minimum_chlorine
        = xml_node_get_float_with_default (node, XML_LARVA_MINIMUM_CHLORINE, &k,
                                           0.);
      if (!k || s->larva_minimum_chlorine < 0.)
        {
          m = _("Bad larva minimum chlorine");
          goto exit_on_error;
        }
      s->larva_maximum_chlorine
        = xml_node_get_float_with_default (node, XML_LARVA_MAXIMUM_CHLORINE, &k,
                                           0.);
      if (!k || s->larva_maximum_chlorine < s->larva_minimum_chlorine)
        {
          m = _("Bad larva maximum chlorine");
          goto exit_on_error;
        }
      s->larva_decay_chlorine
        = xml_node_get_float_with_default (node, XML_LARVA_DECAY_CHLORINE, &k,
                                           0.);
      if (!k || s->larva_decay_chlorine < 0.)
        {
          m = _("Bad larva chlorine decay");
          goto exit_on_error;
        }
      s->larva_minimum_hydrogen_peroxide
        = xml_node_get_float_with_default (node,
                                           XML_LARVA_MINIMUM_HYDROGEN_PEROXIDE,
                                           &k, 0.);
      if (!k || s->larva_minimum_hydrogen_peroxide < 0.)
        {
          m = _("Bad larva minimum hydrogen peroxide");
          goto exit_on_error;
        }
      s->larva_maximum_hydrogen_peroxide
        = xml_node_get_float_with_default (node,
                                           XML_LARVA_MAXIMUM_HYDROGEN_PEROXIDE,
                                           &k, 0.);
      if (!k || s->larva_maximum_hydrogen_peroxide
          < s->larva_minimum_hydrogen_peroxide)
        {
          m = _("Bad larva maximum hydrogen peroxide");
          goto exit_on_error;
        }
      s->larva_decay_hydrogen_peroxide
        = xml_node_get_float_with_default (node,
                                           XML_LARVA_DECAY_HYDROGEN_PEROXIDE,
                                           &k, 0.);
      if (!k || s->larva_decay_hydrogen_peroxide < 0.)
        {
          m = _("Bad larva hydrogen peroxide decay");
          goto exit_on_error;
        }
      s->adult_minimum_chlorine
        = xml_node_get_float_with_default (node, XML_ADULT_MINIMUM_CHLORINE, &k,
                                           0.);
      if (!k || s->adult_minimum_chlorine < 0.)
        {
          m = _("Bad adult minimum chlorine");
          goto exit_on_error;
        }
      s->adult_maximum_chlorine
        = xml_node_get_float_with_default (node, XML_ADULT_MAXIMUM_CHLORINE, &k,
                                           0.);
      if (!k || s->adult_maximum_chlorine < s->adult_minimum_chlorine)
        {
          m = _("Bad adult maximum chlorine");
          goto exit_on_error;
        }
      s->adult_decay_chlorine
        = xml_node_get_float_with_default (node, XML_ADULT_DECAY_CHLORINE, &k,
                                           0.);
      if (!k || s->adult_decay_chlorine < 0.)
        {
          m = _("Bad adult chlorine decay");
          goto exit_on_error;
        }
      s->adult_minimum_hydrogen_peroxide
        = xml_node_get_float_with_default (node,
                                           XML_ADULT_MINIMUM_HYDROGEN_PEROXIDE,
                                           &k, 0.);
      if (!k || s->adult_minimum_hydrogen_peroxide < 0.)
        {
          m = _("Bad adult minimum hydrogen peroxide");
          goto exit_on_error;
        }
      s->adult_maximum_hydrogen_peroxide
        = xml_node_get_float_with_default (node,
                                           XML_ADULT_MAXIMUM_HYDROGEN_PEROXIDE,
                                           &k, 0.);
      if (!k || s->adult_maximum_hydrogen_peroxide
          < s->adult_minimum_hydrogen_peroxide)
        {
          m = _("Bad adult maximum hydrogen peroxide");
          goto exit_on_error;
        }
      s->adult_decay_hydrogen_peroxide
        = xml_node_get_float_with_default (node,
                                           XML_ADULT_DECAY_HYDROGEN_PEROXIDE,
                                           &k, 0.);
      if (!k || s->adult_decay_hydrogen_peroxide < 0.)
        {
          m = _("Bad adult hydrogen peroxide decay");
          goto exit_on_error;
        }
      g_hash_table_insert (hash, name, NULL);
    }
  if (!g_hash_table_size (hash))
    {
      m = _("No species");
      goto exit_on_error;
    }

  // free memory
  g_hash_table_destroy (hash);
  xmlFreeDoc (doc);

  // exit on success
#if DEBUG_SPECIES
  species_print (s, stderr);
  fprintf (stderr, "species_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  species_error (s, m);

  // free memory on error
  if (hash)
    g_hash_table_destroy (hash);
  xmlFreeDoc (doc);
  species_destroy ();

  // end
#if DEBUG_SPECIES
  fprintf (stderr, "species_open_xml: end\n");
#endif
  return 0;
}
