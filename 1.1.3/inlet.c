/**
 * \file inlet.c
 * \brief source file to define the inlets.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <math.h>
#include <libxml/parser.h>
#include <glib.h>
#include <gsl/gsl_rng.h>
#include "config.h"
#include "tools.h"
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
inlet_error (Inlet * inlet,     ///< pointer to the inlet struct data.
             const char *msg)   ///< error message.
{
  error_message (_("Inlet"), inlet->id, (char *) g_strdup (msg));
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
  inlet->solute_concentration = inlet->solute_time
    = inlet->species_concentration = inlet->species_time = NULL;
  inlet->nsolute_times = inlet->nspecies_times = NULL;
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
inlet_destroy (Inlet * inlet)   ///< pointer to the inlet struct data.
{
  unsigned int i;
#if DEBUG_INLET
  fprintf (stderr, "inlet_destroy: start\n");
#endif
  free (inlet->cell);
  if (inlet->solute_concentration)
    {
      for (i = 0; i < nsolutes; ++i)
        free (inlet->solute_concentration[i]);
      free (inlet->solute_concentration);
    }
  if (inlet->solute_time)
    {
      for (i = 0; i < nsolutes; ++i)
        free (inlet->solute_time[i]);
      free (inlet->solute_time);
    }
  free (inlet->nsolute_times);
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
  inlet_null (inlet);
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
  double *tt, *cc;
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
  tt = cc = NULL;
  do
    {
      t = read_time (file, &e);
      if (!e)
        break;
      if (fscanf (file, "%lf", &c) != 1)
        break;
      i = *n;
      if (i && t < tt[i - 1])
        goto exit_on_error;
      ++(*n);
      size = (*n) * sizeof (double);
      cc = (double *) realloc (cc, size);
      cc[i] = c;
      tt = (double *) realloc (tt, size);
      tt[i] = t;
    }
  while (1);
  *data = cc;
  *date = tt;
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
                unsigned int npipes,    ///< number of pipes. 
                char *directory)        ///< directory string.
{
  char name[BUFFER_SIZE];
  Cell *cell;
  xmlChar *buffer;
  double **c, **t;
  unsigned int *n;
  const char *m;
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

  for (i = 0; i < npipes; ++i)
    {
      cell = pipe_node_cell (pipe + i, inlet->id);
      if (cell)
        {
          j = inlet->ncells++;
          inlet->cell
            = (Cell **) realloc (inlet->cell, inlet->ncells * sizeof (Cell *));
          inlet->cell[j] = cell;
        }
    }
  if (!inlet->ncells)
    {
      m = _("The node is not assigned to a pipe");
      goto exit_on_error;
    }

  // allocating arrays
  inlet->solute_concentration = c
    = (double **) malloc (nsolutes * sizeof (double *));
  inlet->solute_time = t = (double **) malloc (nsolutes * sizeof (double *));
  inlet->nsolute_times = n
    = (unsigned int *) malloc (nsolutes * sizeof (unsigned int));
  for (i = 0; i < nsolutes; ++i)
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
      if (!xmlStrcmp (node->name, XML_SOLUTE))
        {
          xmlFree (buffer);
          buffer = xmlGetProp (node, XML_NAME);
          if (!buffer)
            {
              m = _("No solute name");
              goto exit_on_error;
            }
          i = solute_index ((const char *) buffer);
          if (i == nsolutes)
            {
              m = _("Unknown solute");
              goto exit_on_error;
            }
          xmlFree (buffer);
          buffer = xmlGetProp (node, XML_FILE);
          if (!buffer)
            {
              m = _("No solute file");
              goto exit_on_error;
            }
          snprintf (name, BUFFER_SIZE, "%s/%s", directory, (char *) buffer);
          if (!inlet_read_file (name,
                                inlet->solute_concentration + i,
                                inlet->solute_time + i,
                                inlet->nsolute_times + i))
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
          i = species_index ((const char *) buffer);
          if (i == nspecies)
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
          if (!inlet_read_file (name,
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
