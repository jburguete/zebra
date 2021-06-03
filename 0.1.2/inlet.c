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

/**
 * function to set an error message opening an inlets input file.
 */
static inline void
inlet_error (const char *msg)   ///< error message.
{
  error_message (_("Inlet"), (char *) g_strdup (msg));
}

/**
 * function to init an empty inlet.
 */
static inline void
inlet_null (Inlet * inlet)      ///< pointer to the inlet struct data.
{
#if DEBUG_INLET
  fprintf (stderr, "inlet_null: start\n");
#endif
  inlet->nutrient_concentration = inlet->nutrient_time
    = inlet->species_concentration = inlet->species_time = NULL;
  inlet->nnutrient_times = inlet->nspecies_times = NULL;
#if DEBUG_INLET
  fprintf (stderr, "inlet_null: end\n");
#endif
}

/**
 * function to free the memory used by the inlet data.
 */
void
inlet_destroy (Inlet * inlet)   ///< pointer to the inlet struct data.
{
  unsigned int i;
#if DEBUG_INLET
  fprintf (stderr, "inlet_destroy: start\n");
#endif
  if (inlet->nutrient_concentration)
    {
      for (i = 0; i < nnutrients; ++i)
        free (inlet->nutrient_concentration[i]);
      free (inlet->nutrient_concentration);
    }
  if (inlet->nutrient_time)
    {
      for (i = 0; i < nnutrients; ++i)
        free (inlet->nutrient_time[i]);
      free (inlet->nutrient_time);
    }
  free (inlet->nnutrient_times);
  if (inlet->species_concentration)
    {
      for (i = 0; i < nspecies; ++i)
        free (inlet->species_concentration[i]);
      free (inlet->species_concentration);
    }
  if (inlet->species_time)
    {
      for (i = 0; i < nspecies; ++i)
        free (inlet->species_time[i]);
      free (inlet->species_time);
    }
  free (inlet->nspecies_times);
#if DEBUG_INLET
  fprintf (stderr, "inlet_destroy: end\n");
#endif
}

/**
 * function to read the inlet data on a XML node.
 *
 * \return 1 on succes, 0 on error.
 */
int
inlet_open_xml (Inlet * inlet,  ///< pointer to the inlet struct data.
                xmlNode * node) ///< XML node.
{
  xmlNode *child;
  xmlChar *buffer;
  double **c, **t;
  unsigned int *n;
  const char *m;
  int e;
  unsigned int i, nn;

  // start
#if DEBUG_INLET
  fprintf (stderr, "inlet_open_xml: start\n");
#endif
  inlet_null (inlet);

  // allocating arrays
  inlet->nutrient_concentration = c
    = (double **) malloc (nnutrients * sizeof (double *));
  inlet->nutrient_time = t
    = (double **) malloc (nnutrients * sizeof (double *));
  inlet->nnutrient_times = n
    = (unsigned int *) malloc (nnutrients * sizeof (unsigned int));
  for (i = 0; i < nnutrients; ++i)
    {
      c[i] = t[i] = NULL;
      n[i] = 0;
    }
  inlet->species_concentration = c
    = (double **) malloc (nspecies * sizeof (double *));
  inlet->species_time = t = (double **) malloc (nspecies * sizeof (double *));
  inlet->nspecies_times = n
    = (unsigned int *) malloc (nspecies * sizeof (unsigned int));
  for (i = 0; i < nspecies; ++i)
    {
      c[i] = t[i] = NULL;
      n[i] = 0;
    }

  // reading inlet
  for (node = node->children; node; node = node->next)
    {
      if (!xmlStrcmp (node->name, XML_NUTRIENT))
        {
          buffer = xmlGetProp (node, XML_NAME);
          if (!buffer)
            {
              m = _("No nutrient name");
              goto exit_on_error;
            }
          i = nutrient_index ((const char *) buffer);
          xmlFree (buffer);
          if (i == nnutrients)
            {
              m = _("Unknown nutrient");
              goto exit_on_error;
            }
          c = inlet->nutrient_concentration + i;
          t = inlet->nutrient_time + i;
          n = inlet->nnutrient_times + i;
          for (child = node->children; child; child = child->next)
            {
              if (xmlStrcmp (child->name, XML_NUTRIENT))
                {
                  m = _("Bad nutrient node");
                  goto exit_on_error;
                }
              nn = *n;
              ++(*n);
              *c = realloc (*c, (*n) * sizeof (double));
              *t = realloc (*t, (*n) * sizeof (double));
              (*c)[nn] = xml_node_get_float (child, XML_CONCENTRATION, &e);
              if (!e)
                {
                  m = _("Bad nutrient concentration");
                  goto exit_on_error;
                }
              (*t)[nn] = xml_node_get_time (child, XML_TIME, &e);
              if (!e)
                {
                  m = _("Bad nutrient time");
                  goto exit_on_error;
                }
              if (nn && (*t)[nn] < (*t)[nn - 1])
                {
                  m = _("Bad nutrient times order");
                  goto exit_on_error;
                }
            }
        }
      else if (!xmlStrcmp (node->name, XML_SPECIES))
        {
          buffer = xmlGetProp (node, XML_NAME);
          if (!buffer)
            {
              m = _("No species name");
              goto exit_on_error;
            }
          i = species_index ((const char *) buffer);
          xmlFree (buffer);
          if (i == nspecies)
            {
              m = _("Unknown species");
              goto exit_on_error;
            }
          c = inlet->species_concentration + i;
          t = inlet->species_time + i;
          n = inlet->nspecies_times + i;
          for (child = node->children; child; child = child->next)
            {
              if (xmlStrcmp (child->name, XML_SPECIES))
                {
                  m = _("Bad species node");
                  goto exit_on_error;
                }
              nn = *n;
              ++(*n);
              *c = realloc (*c, (*n) * sizeof (double));
              *t = realloc (*t, (*n) * sizeof (double));
              (*c)[nn] = xml_node_get_float (child, XML_CONCENTRATION, &e);
              if (!e)
                {
                  m = _("Bad species concentration");
                  goto exit_on_error;
                }
              (*t)[nn] = xml_node_get_time (child, XML_TIME, &e);
              if (!e)
                {
                  m = _("Bad species time");
                  goto exit_on_error;
                }
              if (nn && (*t)[nn] < (*t)[nn - 1])
                {
                  m = _("Bad species times order");
                  goto exit_on_error;
                }
            }
        }
      else
        {
          m = _("Unknown node");
          goto exit_on_error;
        }
    }

  // exit on success
#if DEBUG_INLET
  fprintf (stderr, "inlet_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  inlet_error (m);

  // free memory on error
  inlet_destroy (inlet);

  // end
#if DEBUG_INLET
  fprintf (stderr, "inlet_open_xml: end\n");
#endif
  return 0;
}
