/**
 * \file inlet.c
 * \brief source file to define the inlets.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include <gsl/gsl_rng.h>
#include "config.h"
#include "tools.h"
#include "temperature.h"
#include "solute.h"
#include "species.h"
#include "specimen.h"
#include "point.h"
#include "cell.h"
#include "wall.h"
#include "pipe.h"
#include "inlet.h"

/**
 * function to set an error message opening an inlets input file.
 */
static inline void
inlet_error (Inlet *inlet,      ///< pointer to the inlet struct data.
             const char *msg)   ///< error message.
{
  error_message (_("Inlet"), inlet->id, (char *) g_strdup (msg));
}

/**
 * function to init an empty inlet.
 */
static inline void
inlet_null (Inlet *inlet)       ///< pointer to the inlet struct data.
{
  unsigned int i;
#if DEBUG_INLET
  fprintf (stderr, "inlet_null: start\n");
#endif
  inlet->cell = NULL;
  for (i = 0; i < MAX_SOLUTES; ++i)
    {
      inlet->nsolute_times[i] = 0;
      inlet->solute_concentration[i] = inlet->solute_time[i] = NULL;
    }
  for (i = 0; i < MAX_SPECIES; ++i)
    {
      inlet->nspecies_times[i] = 0;
      inlet->species_concentration[i] = inlet->species_time[i] = NULL;
    }
  inlet->ncells = 0;
  inlet->id[0] = 0;
#if DEBUG_INLET
  fprintf (stderr, "inlet_null: end\n");
#endif
}

/**
 * function to free the memory used by the inlet data.
 */
void
inlet_destroy (Inlet *inlet)    ///< pointer to the inlet struct data.
{
  unsigned int i;
#if DEBUG_INLET
  fprintf (stderr, "inlet_destroy: start\n");
#endif
  for (i = 0; i < MAX_SOLUTES; ++i)
    {
      free (inlet->solute_concentration[i]);
      free (inlet->solute_time[i]);
    }
  for (i = 0; i < MAX_SPECIES; ++i)
    {
      free (inlet->species_concentration[i]);
      free (inlet->species_time[i]);
    }
  free (inlet->cell);
  inlet_null (inlet);
#if DEBUG_INLET
  fprintf (stderr, "inlet_destroy: end\n");
#endif
}

/**
 * function to read the inlet data on a XML node.
 *
 * \return 1 on success, 0 on error.
 */
int
inlet_open_xml (Inlet *inlet,   ///< pointer to the inlet struct data.
                xmlNode *node,  ///< XML node.
                Pipe *pipe,     ///< array of pointers to pipe struct data.
                unsigned int npipes,    ///< number of pipes. 
                char *directory)        ///< directory string.
{
  char name[BUFFER_SIZE];
  Cell *cell;
  xmlChar *buffer;
  const char *m;
  double x;
  int e;
  unsigned int i, j;

  // start
#if DEBUG_INLET
  fprintf (stderr, "inlet_open_xml: start\n");
#endif
  inlet_null (inlet);

  // setting up node
  buffer = xmlGetProp (node, XML_NODE);
  if (!buffer || !xmlStrlen (buffer) || xmlStrlen (buffer) >= MAX_LABEL_LENGTH)
    {
      m = _("Bad node identifier");
      goto exit_on_error;
    }
  snprintf (inlet->id, MAX_LABEL_LENGTH, "%s", (char *) buffer);
  xmlFree (buffer);
  buffer = xmlGetProp (node, XML_TYPE);
  if (!buffer)
    inlet->type = INLET_TYPE_SET;
  else if (!xmlStrcmp (buffer, XML_INJECT))
    inlet->type = INLET_TYPE_INJECT;
  else if (!xmlStrcmp (buffer, XML_SET))
    inlet->type = INLET_TYPE_SET;
  else
    {
      m = _("Bad inlet type");
      goto exit_on_error;
    }

  inlet->volume = 0.;
  for (i = 0; i < npipes; ++i)
    {
      cell = pipe_node_cell (pipe + i, inlet->id);
      if (cell)
        {
          j = inlet->ncells++;
          inlet->cell
            = (Cell **) realloc (inlet->cell, inlet->ncells * sizeof (Cell *));
          inlet->cell[j] = cell;
          inlet->volume += cell->volume;
        }
    }
  if (!inlet->ncells)
    {
      m = _("The node is not assigned to a pipe");
      goto exit_on_error;
    }

  // reading inlet
  for (node = node->children; node; node = node->next)
    {
      if (!xmlStrcmp (node->name, XML_SOLUTE))
        {
          xmlFree (buffer);
          buffer = xmlGetProp (node, XML_NAME);
          if (!buffer)
            {
              m = _("No solute name");
              goto exit_on_error;
            }
          i = solute_index (buffer);
          if (i == MAX_SOLUTES)
            {
              m = _("Unknown solute");
              goto exit_on_error;
            }
          x = xml_node_get_float_with_default (node, XML_INITIAL_CONDITIONS, &e,
                                               0.);
          if (!e || x < 0.)
            {
              m = _("Bad initial conditions");
              goto exit_on_error;
            }
          solute[i].initial_conditions = x;
          xmlFree (buffer);
          buffer = xmlGetProp (node, XML_FILE);
          if (!buffer)
            {
              m = _("No solute file");
              goto exit_on_error;
            }
          snprintf (name, BUFFER_SIZE, "%s/%s", directory, (char *) buffer);
          if (!read_file (name,
                          inlet->solute_concentration + i,
                          inlet->solute_time + i, inlet->nsolute_times + i))
            {
              m = _("Bad solute input file");
              goto exit_on_error;
            }
        }
      else if (!xmlStrcmp (node->name, XML_SPECIES))
        {
          xmlFree (buffer);
          buffer = xmlGetProp (node, XML_NAME);
          if (!buffer)
            {
              m = _("No species name");
              goto exit_on_error;
            }
          i = species_index (buffer);
          if (i == MAX_SPECIES)
            {
              m = _("Unknown species");
              goto exit_on_error;
            }
          xmlFree (buffer);
          buffer = xmlGetProp (node, XML_FILE);
          if (!buffer)
            {
              m = _("No species file");
              goto exit_on_error;
            }
          snprintf (name, BUFFER_SIZE, "%s/%s", directory, (char *) buffer);
          if (!read_file (name,
                          inlet->species_concentration + i,
                          inlet->species_time + i, inlet->nspecies_times + i))
            {
              m = _("Bad species input file");
              goto exit_on_error;
            }
        }
      else
        {
          m = _("Unknown node");
          goto exit_on_error;
        }
    }

  // free memory
  xmlFree (buffer);

  // exit on success
#if DEBUG_INLET
  fprintf (stderr, "inlet_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  inlet_error (inlet, m);

  // free memory on error
  xmlFree (buffer);
  inlet_destroy (inlet);

  // end
#if DEBUG_INLET
  fprintf (stderr, "inlet_open_xml: end\n");
#endif
  return 0;
}
