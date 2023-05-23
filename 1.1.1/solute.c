/**
 * \file solute.c
 * \brief source file to define the solutes.
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

Solute *solute;                 ///< array of solute struct data.
GHashTable *hash_solutes;       ///< hash table of solute names.
unsigned int nsolutes;          ///< number of solutes.
unsigned int flags_solutes;     ///< solutes flags.

/**
 * function to set an error message opening a solutes input file.
 */
static inline void
solute_error (Solute * s,       ///< Solute struct.
              const char *msg)  ///< error message.
{
  char *name;
  name = NULL;
  if (s && s->name)
    name = (char *) s->name;
  error_message (_("Solute"), name, (char *) g_strdup (msg));
}

/**
 * function to init null solutes data.
 */
static void
solute_null ()
{
  nsolutes = flags_solutes = 0;
  solute = NULL;
  hash_solutes = NULL;
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
  for (i = 0; i < nsolutes; ++i)
    xmlFree (solute[i].name);
  free (solute);
  if (hash_solutes)
    g_hash_table_destroy (hash_solutes);
  solute_null ();
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_destroy: end\n");
#endif
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
  const char *m;
  int e;
  unsigned int i;

  // start
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_open_xml: start\n");
#endif
  solute_null ();
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
  if (xmlStrcmp (node->name, XML_SOLUTE))
    {
      m = _("Bad root XML node");
      goto exit_on_error;
    }

  // reading solute
  for (node = node->children; node; node = node->next)
    {
      i = nsolutes;
      ++nsolutes;
      solute = (Solute *) realloc (solute, nsolutes * sizeof (Solute));
      s = solute + i;
      if (xmlStrcmp (node->name, XML_SOLUTE))
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
      if (hash_solutes && g_hash_table_contains (hash_solutes, s->name))
        {
          m = _("Duplicated name");
          goto exit_on_error;
        }
      if (!xmlStrcmp (s->name, XML_CHLORINE))
        s->type = SOLUTE_TYPE_CHLORINE;
      else if (!xmlStrcmp (s->name, XML_HYDROGEN_PEROXIDE))
        s->type = SOLUTE_TYPE_HYDROGEN_PEROXIDE;
      else if (!xmlStrcmp (s->name, XML_ORGANIC_MATTER))
        s->type = SOLUTE_TYPE_ORGANIC_MATTER;
      else if (!xmlStrcmp (s->name, XML_OXYGEN))
        s->type = SOLUTE_TYPE_OXYGEN;
      else
        {
          m = _("Unknown");
          goto exit_on_error;
        }
      flags_solutes |= s->type;
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
      g_hash_table_insert (hash_solutes, s->name, s);
      if (nsolutes == MAX_SOLUTES)
        break;
    }

  if (!nsolutes)
    {
      m = _("No solutes");
      goto exit_on_error;
    }

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
  xmlFreeDoc (doc);
  solute_destroy ();

  // end
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_open_xml: end\n");
#endif
  return 0;
}
