/**
 * \file inlet.c
 * \brief source file to define the inlets.
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
#include "nutrient.h"
#include "species.h"
#include "point.h"
#include "cell.h"
#include "pipe.h"
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
  inlet->cell = NULL;
  inlet->nutrient_concentration = inlet->nutrient_time
    = inlet->species_concentration = inlet->species_time = NULL;
  inlet->nnutrient_times = inlet->nspecies_times = NULL;
  inlet->ncells = 0;
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
  free (inlet->cell);
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
 * function to read the inlet data on a text file.
 *
 * \return 1 on success, 0 on error.
 */
static int
inlet_read_file (const char *name,      ///< file name.
                 double **data, ///< pointer to the array of data.
                 double **date, ///< pointer to the array of times.
                 unsigned int *n)       ///< pointer to the number of data.
{
  FILE *file;
  double t, c;
  size_t size;
  int e, error_code = 0;
  unsigned int i;
#if DEBUG_INLET
  fprintf (stderr, "inlet_read_file: start\n");
#endif
  file = fopen (name, "r");
  if (!file)
    goto exit_on_error;
  *n = 0;
  do
    {
      t = read_time (file, &e);
      if (!e)
        break;
      if (fscanf (file, "%lf", &c) != 1)
        break;
      i = *n;
      if (i && t < *date[i - 1])
        goto exit_on_error;
      ++(*n);
      size = (*n) * sizeof (double);
      *data = (double *) realloc (*data, size);
      *date = (double *) realloc (*date, size);
      *date[i] = t;
      *data[i] = c;
    }
  while (1);
  if (!*n)
    goto exit_on_error;
  error_code = 1;
exit_on_error:
  if (file)
    fclose (file);
#if DEBUG_INLET
  fprintf (stderr, "inlet_read_file: end\n");
#endif
  return error_code;
}

/**
 * function to read the inlet data on a XML node.
 *
 * \return 1 on success, 0 on error.
 */
int
inlet_open_xml (Inlet * inlet,  ///< pointer to the inlet struct data.
                xmlNode * node, ///< XML node.
                Pipe * pipe,    ///< array of pointers to pipe struct data.
                unsigned int npipes)    ///< number of pipes. 
{
  Cell *cell;
  xmlChar *buffer;
  double **c, **t;
  unsigned int *n;
  const char *m;
  int e;
  unsigned int i;

  // start
#if DEBUG_INLET
  fprintf (stderr, "inlet_open_xml: start\n");
#endif
  inlet_null (inlet);

  // setting up node
  inlet->id = xml_node_get_uint (node, XML_NODE, &e);
  if (!e)
    {
      m = _("Bad node identifier");
      goto exit_on_error;
    }
  for (i = 0; i < npipes; ++i)
    {
      cell = pipe_node_cell (pipe + i, inlet->id);
      if (cell)
        {
          ++inlet->ncells;
          inlet->cell
            = (Cell **) realloc (inlet->cell, inlet->ncells * sizeof (Cell *));
          inlet->cell[inlet->ncells - 1] = cell;
        }
    }
  if (!inlet->ncells)
    {
      m = _("The node is not assigned to a pipe");
      goto exit_on_error;
    }

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
          if (!inlet_read_file ((char *) buffer,
                                inlet->nutrient_concentration + i,
                                inlet->nutrient_time + i,
                                inlet->nnutrient_times + i))
            {
              m = _("Bad nutrient input file");
              goto exit_on_error;
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
          if (!inlet_read_file ((char *) buffer,
                                inlet->species_concentration + i,
                                inlet->species_time + i,
                                inlet->nspecies_times + i))
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
