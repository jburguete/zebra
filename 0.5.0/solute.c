/**
 * \file solute.c
 * \brief source file to define the solutes.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2022, Javier Burguete Tolosa.
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

Solute *solute = NULL;      ///< array of solute struct data.
unsigned int nsolutes = 0;    ///< number of solutes.

/**
 * function to set an error message opening a solutes input file.
 */
static inline void
solute_error (const char *msg)        ///< error message.
{
  error_message (_("Solute"), (char *) g_strdup (msg));
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
  solute = NULL;
  nsolutes = 0;
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
solute_open_xml (char *file_name)     ///< input file name.
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
      if (xmlStrcmp (node->name, XML_SOLUTE))
        {
          m = _("Bad XML node");
          goto exit_on_error;
        }
      i = nsolutes;
      ++nsolutes;
      solute
        = (Solute *) realloc (solute, nsolutes * sizeof (Solute));
      s = solute + i;
      s->name = xmlGetProp (node, XML_NAME);
      if (!s->name)
        {
          m = _("Bad name");
          goto exit_on_error;
        }
      if (!xmlStrcmp (s->name, XML_CHLORINE))
        s->type = SOLUTE_TYPE_CHLORINE;
      else if (!xmlStrcmp (s->name, XML_HIDROGEN_PEROXIDE))
        s->type = SOLUTE_TYPE_HIDROGEN_PEROXIDE;
      else if (!xmlStrcmp (s->name, XML_ORGANIC_MATTER))
        s->type = SOLUTE_TYPE_ORGANIC_MATTER;
      else if (!xmlStrcmp (s->name, XML_OXYGEN))
        s->type = SOLUTE_TYPE_OXYGEN;
      else
        {
          m = _("Unknown");
          goto exit_on_error;
        }
      s->decay = xml_node_get_float_with_default (node, XML_DECAY, &e, 0.);
      if (!e)
        {
          m = _("Bad decay rate");
          goto exit_on_error;
        }
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
  solute_error (m);

  // free memory
  xmlFreeDoc (doc);
  solute_destroy ();

  // end
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_open_xml: end\n");
#endif
  return 0;
}

/**
 * function to find the index of a solute on the solutes array.
 *
 * \return solute index on success, nsolutes on error.
 */
unsigned int
solute_index (const char *name)       ///< solute name.
{
  unsigned int i;
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_index: start\n");
#endif
  for (i = 0; i < nsolutes; ++i)
    if (!strcmp ((char *) solute[i].name, name))
      break;
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_index: end\n");
#endif
  return i;
}
