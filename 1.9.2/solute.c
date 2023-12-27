/**
 * \file solute.c
 * \brief source file to define the solutes.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
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

Solute solute[MAX_SOLUTES];     ///< array of solute struct data.

/**
 * function to set an error message opening a solutes input file.
 */
static inline void
solute_error (Solute *s,        ///< Solute struct.
              const char *msg)  ///< error message.
{
  const char *name;
  name = NULL;
  if (s && s->name)
    name = (const char *) s->name;
  error_message (_("Solute"), name, g_strdup (msg));
}

/**
 * function to free the memory used by the solutes data.
 */
void
solute_destroy ()
{
  unsigned int i;
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_destroy: start\n");
#endif
  for (i = 0; i < MAX_SOLUTES; ++i)
    xmlFree (solute[i].name);
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_destroy: end\n");
#endif
}

/**
 * function to get the index of a solute.
 *
 * \return solute index.
 */
unsigned int
solute_index (const xmlChar *name)      ///< solute name.
{
  unsigned int i;
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_index: start\n");
#endif
  if (!xmlStrcmp (name, XML_CHLORINE))
    i = SOLUTE_TYPE_CHLORINE;
  else if (!xmlStrcmp (name, XML_HYDROGEN_PEROXIDE))
    i = SOLUTE_TYPE_HYDROGEN_PEROXIDE;
  else if (!xmlStrcmp (name, XML_ORGANIC_MATTER))
    i = SOLUTE_TYPE_ORGANIC_MATTER;
  else if (!xmlStrcmp (name, XML_OXYGEN))
    i = SOLUTE_TYPE_OXYGEN;
  else
    i = MAX_SOLUTES;
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_index: end\n");
#endif
  return i;
}

/**
 * function to read the solutes data on a XML file.
 *
 * \return 1 on succes, 0 on error.
 */
int
solute_open_xml (const char *file_name) ///< input file name.
{
  Solute *s;
  xmlDoc *doc;
  xmlNode *node;
  xmlChar *name;
  GHashTable *hash;
  const char *m;
  int e;
  unsigned int i;

  // start
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_open_xml: start\n");
#endif
  for (i = 0; i < MAX_SOLUTES; ++i)
    solute[i].name = NULL;
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
  if (xmlStrcmp (node->name, XML_SOLUTE))
    {
      m = _("Bad root XML node");
      goto exit_on_error;
    }

  // reading solute
  hash = g_hash_table_new (g_str_hash, g_str_equal);
  for (node = node->children; node; node = node->next)
    {
      s = NULL;
      if (xmlStrcmp (node->name, XML_SOLUTE))
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
      i = solute_index (name);
      if (i == MAX_SOLUTES)
        {
          m = _("Unknown");
          xmlFree (name);
          goto exit_on_error;
        }
      s = solute + i;
      s->type = i;
      s->name = name;
      s->solubility
        = xml_node_get_float_with_default (node, XML_SOLUBILITY, &e, 0.);
      if (!e)
        {
          m = _("Bad solubility");
          goto exit_on_error;
        }
      s->time_decay
        = xml_node_get_float_with_default (node, XML_TIME_DECAY, &e, 0.);
      if (!e)
        {
          m = _("Bad time decay rate");
          goto exit_on_error;
        }
      s->decay_temperature
        = xml_node_get_float_with_default (node, XML_DECAY_TEMPERATURE, &e, 0.);
      if (!e)
        {
          m = _("Bad temperature decay rate");
          goto exit_on_error;
        }
      s->decay_surface
        = xml_node_get_float_with_default (node, XML_DECAY_SURFACE, &e, 0.);
      if (!e)
        {
          m = _("Bad surface decay rate");
          goto exit_on_error;
        }
      g_hash_table_insert (hash, name, NULL);
    }

  if (!g_hash_table_size (hash))
    {
      m = _("No solutes");
      goto exit_on_error;
    }

  // free memory
  g_hash_table_destroy (hash);
  xmlFreeDoc (doc);

  // exit on success
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  solute_error (s, m);

  // free memory
  if (hash)
    g_hash_table_destroy (hash);
  xmlFreeDoc (doc);
  solute_destroy ();

  // end
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_open_xml: end\n");
#endif
  return 0;
}
