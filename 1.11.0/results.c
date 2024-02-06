/**
 * \file results.c
 * \brief source file to define the results data base file.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "junction.h"
#include "inlet.h"
#include "network.h"
#include "results.h"

/**
 * function to set an error message opening a results data base file.
 */
static inline void
results_error (const char *msg) ///< error message.
{
  error_message (_("Results"), NULL, (char *) g_strdup (msg));
}

/**
 * function to init an empty results data base.
 */
static inline void
results_null (Results *results)
{
#if DEBUG_RESULTS
  fprintf (stderr, "results_null: start\n");
#endif
  results->point_x = results->point_y = results->point_z = results->pipe_length
    = results->variable = NULL;
  results->point_id = results->pipe_id = results->pipe_inlet_id
    = results->pipe_outlet_id = NULL;
  results->pipe_cell = NULL;
#if DEBUG_RESULTS
  fprintf (stderr, "results_null: end\n");
#endif
}

/**
 * function to free the memory used by a results data base.
 */
void
results_destroy (Results *results)
{
#if DEBUG_RESULTS
  fprintf (stderr, "results_destroy: start\n");
#endif
  free (results->variable);
  free (results->pipe_cell);
  free (results->pipe_outlet_id);
  free (results->pipe_inlet_id);
  free (results->pipe_id);
  free (results->pipe_length);
  free (results->point_z);
  free (results->point_y);
  free (results->point_x);
  free (results->point_id);
#if DEBUG_RESULTS
  fprintf (stderr, "results_destroy: end\n");
#endif
}

/**
 * function to set the variables on the results data base.
 */
void
results_set (Results *results,
             ///< pointer to the results data base struct.
             Network *network)  ///< pointer to the network data struct.
{
  Pipe *pipe;
  Cell *cell;
  double *variable;
  unsigned int i, j, k, npipes, ncells;
#if DEBUG_RESULTS
  unsigned int l;
  fprintf (stderr, "results_set: start\n");
#endif
  variable = results->variable;
  pipe = network->pipe;
  npipes = network->npipes;
  for (i = k = 0; i < npipes; ++i, ++pipe)
    {
      cell = pipe->cell;
      ncells = pipe->ncells;
      for (j = 0; j < ncells; ++j, ++cell)
        {
#if DEBUG_RESULTS
          fprintf (stderr, "results_set: pipe=%u cell=%u\n", i, j);
          for (l = 0; l < MAX_SOLUTES; ++l)
            fprintf (stderr, "results_set: solute=%u concentration=%lg\n",
                     l, cell->solute_concentration[l]);
#endif
          memcpy (variable + k, cell->solute_concentration,
                  MAX_SOLUTES * sizeof (double));
          k += MAX_SOLUTES;
#if DEBUG_RESULTS
          for (l = 0; l < MAX_SPECIES; ++l)
            fprintf (stderr, "results_set: species=%u concentration=%lg\n",
                     l, cell->species_concentration[l]);
#endif
          memcpy (variable + k, cell->species_concentration,
                  MAX_SPECIES * sizeof (double));
          k += MAX_SPECIES;
#if DEBUG_RESULTS
          for (l = 0; l < MAX_SPECIES; ++l)
            fprintf (stderr, "results_set: species=%u infestation-number=%lg\n",
                     l, cell->species_infestation_number[l]);
#endif
          memcpy (variable + k, cell->species_infestation_number,
                  MAX_SPECIES * sizeof (double));
          k += MAX_SPECIES;
#if DEBUG_RESULTS
          for (l = 0; l < MAX_SPECIES; ++l)
            fprintf (stderr, "results_set: species=%u infestation-volume=%lg\n",
                     l, cell->species_infestation_volume[l]);
#endif
          memcpy (variable + k, cell->species_infestation_volume,
                  MAX_SPECIES * sizeof (double));
          k += MAX_SPECIES;
        }
    }
#if DEBUG_RESULTS
  fprintf (stderr, "results_set: k=%u\n", k);
  fprintf (stderr, "results_set: end\n");
#endif
}

/**
 * function to open the results data base from a file.
 */
int
results_open_bin (Results *results,
                  ///< pointer to the results data base struct.
                  char *file_name)      ///< file name.
{
  ResultsHeader *header;
  FILE *file;
  const char *m;
  unsigned int i, n;

#if DEBUG_RESULTS
  fprintf (stderr, "results_open_bin: start\n");
#endif

  // init pointers
  results_null (results);

  // open file
  file = fopen (file_name, "rb");
  if (!file)
    {
      m = _("Unable to open the results data base file");
      goto exit_on_error;
    }

  // read header
  header = results->header;
  if (fread (header, sizeof (ResultsHeader), 1, file) != 1)
    {
      m = _("Bad header");
      goto exit_on_error;
    }
  if (header->final_time < header->initial_time)
    {
      m = _("Bad final time");
      goto exit_on_error;
    }
  if (header->saving_step <= 0.)
    {
      m = _("Bad saving time step size");
      goto exit_on_error;
    }
  if (header->cell_size <= 0.)
    {
      m = _("Bad cell size");
      goto exit_on_error;
    }
  if (!header->nsolutes || header->nsolutes > MAX_SOLUTES)
    {
      m = _("Bad solutes number");
      goto exit_on_error;
    }
  if (!header->nspecies || header->nspecies > MAX_SPECIES)
    {
      m = _("Bad species number");
      goto exit_on_error;
    }
  if (!header->npoints)
    {
      m = _("Bad points number");
      goto exit_on_error;
    }
  if (!header->npipes)
    {
      m = _("Bad pipes number");
      goto exit_on_error;
    }

  // read arrays
  n = header->npoints;
  results->point_id = (char *) malloc (n * MAX_LABEL_LENGTH * sizeof (char));
  if (fread (results->point_id, MAX_LABEL_LENGTH * sizeof (char), n, file) != n)
    {
      m = _("Bad point identifiers");
      goto exit_on_error;
    }
  results->point_x = (double *) malloc (n * sizeof (double));
  if (fread (results->point_x, sizeof (double), n, file) != n)
    {
      m = _("Bad point x-coordinates");
      goto exit_on_error;
    }
  results->point_y = (double *) malloc (n * sizeof (double));
  if (fread (results->point_y, sizeof (double), n, file) != n)
    {
      m = _("Bad point y-coordinates");
      goto exit_on_error;
    }
  results->point_z = (double *) malloc (n * sizeof (double));
  if (fread (results->point_z, sizeof (double), n, file) != n)
    {
      m = _("Bad point z-coordinates");
      goto exit_on_error;
    }
  n = header->npipes;
  results->pipe_id = (char *) malloc (n * MAX_LABEL_LENGTH * sizeof (char));
  if (fread (results->pipe_id, MAX_LABEL_LENGTH * sizeof (char), n, file) != n)
    {
      m = _("Bad pipe identifiers");
      goto exit_on_error;
    }
  results->pipe_inlet_id
    = (char *) malloc (n * MAX_LABEL_LENGTH * sizeof (char));
  if (fread (results->pipe_inlet_id, MAX_LABEL_LENGTH * sizeof (char), n, file)
      != n)
    {
      m = _("Bad pipe inlet identifiers");
      goto exit_on_error;
    }
  results->pipe_outlet_id
    = (char *) malloc (n * MAX_LABEL_LENGTH * sizeof (char));
  if (fread (results->pipe_outlet_id, MAX_LABEL_LENGTH * sizeof (char), n, file)
      != n)
    {
      m = _("Bad pipe outlet identifiers");
      goto exit_on_error;
    }
  results->pipe_length = (double *) malloc (n * sizeof (double));
  if (fread (results->pipe_length, sizeof (double), n, file) != n)
    {
      m = _("Bad pipe lengths");
      goto exit_on_error;
    }
  for (i = 0; i < n; ++i)
    if (results->pipe_length[i] <= 0.)
      {
        m = _("Bad pipe length");
        goto exit_on_error;
      }
  ++n;
  results->pipe_cell = (unsigned int *) malloc (n * sizeof (unsigned int));
  if (fread (results->pipe_cell, sizeof (unsigned int), n, file) != n)
    {
      m = _("Bad cell numbers");
      goto exit_on_error;
    }

  // check identifiers

  // saving data
  results->nvariables = (header->nsolutes + 3 * header->nspecies)
    * results->pipe_cell[header->npipes];
  results->header_position = ftell (file);
  results->file = file;

  // exit on success
#if DEBUG_RESULTS
  fprintf (stderr, "results_open_bin: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  results_error (m);

  // freeing
  if (file)
    fclose (file);
  results_destroy (results);

#if DEBUG_RESULTS
  fprintf (stderr, "results_open_bin: end\n");
#endif
  return 0;
}

/**
 * function to save results from the data base with a configuration XML file.
 */
int
results_open_xml (Results *results,
                  ///< pointer to the results data base struct.
                  char *file_name)      ///< configuration XML file name.
{
  char name[BUFFER_SIZE];
  char id[MAX_LABEL_LENGTH];
  double *variable = NULL;
  ResultsHeader *header;
  xmlDoc *doc;
  xmlNode *node;
  xmlChar *buffer;
  FILE *file;
  char *directory;
  const char *m;
  double t, x, y, z, x0, y0, z0, dx, dy, dz;
  int e;
  unsigned int i, j, k, l, n, ncells;

#if DEBUG_RESULTS
  fprintf (stderr, "results_open_xml: start\n");
#endif

  // init results file
  file = NULL;

  // open file
  directory = g_path_get_dirname (file_name);
  doc = xmlParseFile (file_name);
  if (!doc)
    {
      m = _("Bad configuration file");
      goto exit_on_error;
    }

  // open root XML node
  node = xmlDocGetRootElement (doc);
  if (!node)
    {
      m = _("No root XML node");
      goto exit_on_error;
    }
  if (xmlStrcmp (node->name, XML_RESULTS))
    {
      m = _("Bad root XML node");
      goto exit_on_error;
    }

  header = results->header;
  n = header->nsolutes + 3 * header->nspecies;

  for (node = node->children; node; node = node->next)
    {
      // locate on the data base file
      fseek (results->file, results->header_position, SEEK_SET);

      // point evolution file
      if (!xmlStrcmp (node->name, XML_POINT))
        {
          buffer = xmlGetProp (node, XML_ID);
          if (!buffer || !xmlStrlen (buffer)
              || xmlStrlen (buffer) >= MAX_LABEL_LENGTH)
            {
              m = _("Bad point identifier");
              xmlFree (buffer);
              goto exit_on_error;
            }
          strncpy (id, (const char *) buffer, MAX_LABEL_LENGTH * sizeof (char));
          xmlFree (buffer);
#if DEBUG_RESULTS
          fprintf (stderr, "results_open_xml: point id=%s\n", id);
#endif
          for (i = 0; i < header->npipes; ++i)
            {
              if (!strcmp (id, results->pipe_inlet_id + i * MAX_LABEL_LENGTH))
                {
                  fseek (results->file,
                         n * results->pipe_cell[i] * sizeof (double), SEEK_CUR);
                  break;
                }
              if (!strcmp (id, results->pipe_outlet_id + i * MAX_LABEL_LENGTH))
                {
                  fseek (results->file,
                         n * results->pipe_cell[i + 1] * sizeof (double),
                         SEEK_CUR);
                  break;
                }
            }
          if (i == header->npipes)
            {
#if DEBUG_RESULTS
              fprintf (stderr, "results_open_xml: point id=%s\n", id);
#endif
              m = _("Unknown point identifier");
              goto exit_on_error;
            }
          buffer = xmlGetProp (node, XML_FILE);
          if (!buffer)
            {
              m = _("No point file");
              goto exit_on_error;
            }
          snprintf (name, BUFFER_SIZE, "%s/%s", directory, (char *) buffer);
          xmlFree (buffer);
          file = fopen (name, "w");
          if (!file)
            {
              m = _("Bad point file");
              goto exit_on_error;
            }
          variable = (double *) realloc (variable, n * sizeof (double));
          for (i = 0; fread (variable, sizeof (double), n, results->file) == n;
               ++i)
            {
              t = fmin (header->final_time,
                        header->initial_time + i * header->saving_step);
              time_string (name, BUFFER_SIZE, t);
              fprintf (file, "%.14lg %s", t - header->initial_time, name);
              for (j = 0; j < n; ++j)
                fprintf (file, " %.9lg", variable[j]);
              fprintf (file, "\n");
              if (fseek (results->file,
                         (results->nvariables - n) * sizeof (double), SEEK_CUR)
                  < 0)
                break;
            }
        }

      // pipe profile
      else if (!xmlStrcmp (node->name, XML_PIPE))
        {
          buffer = xmlGetProp (node, XML_ID);
          if (!buffer || !xmlStrlen (buffer)
              || xmlStrlen (buffer) >= MAX_LABEL_LENGTH)
            {
              m = _("Bad pipe identifier");
              xmlFree (buffer);
              goto exit_on_error;
            }
          strncpy (id, (const char *) buffer, MAX_LABEL_LENGTH * sizeof (char));
          xmlFree (buffer);
          for (i = 0; i < header->npipes; ++i)
            if (!strcmp (id, results->pipe_id + i * MAX_LABEL_LENGTH))
              break;
          if (i == header->npipes)
            {
#if DEBUG_RESULTS
              fprintf (stderr, "results_open_xml: point id=%s\n", id);
#endif
              m = _("Unknown pipe identifier");
              goto exit_on_error;
            }
          t = xml_node_get_time (node, XML_TIME, &e);
          if (!e || t < header->initial_time || t > header->final_time)
            {
              m = _("Bad pipe time");
              goto exit_on_error;
            }
          buffer = xmlGetProp (node, XML_FILE);
          if (!buffer)
            {
              m = _("No pipe file");
              goto exit_on_error;
            }
          snprintf (name, BUFFER_SIZE, "%s/%s", directory, (char *) buffer);
          xmlFree (buffer);
          file = fopen (name, "w");
          if (!file)
            {
              m = _("Bad pipe file");
              goto exit_on_error;
            }
          j = ceil ((t - header->initial_time) / header->saving_step);
          fseek (results->file,
                 (j * results->nvariables + results->pipe_cell[i] * n)
                 * sizeof (double), SEEK_CUR);
          ncells = results->pipe_cell[i + 1] - results->pipe_cell[i];
          variable
            = (double *) realloc (variable, ncells * n * sizeof (double));
          if (fread (variable, n * sizeof (double), ncells, results->file)
              != ncells)
            {
              m = _("Bad data base");
              goto exit_on_error;
            }
          strncpy (id, results->pipe_inlet_id + i * MAX_LABEL_LENGTH,
                   MAX_LABEL_LENGTH * sizeof (char));
          for (j = 0; j < header->npoints; ++j)
            if (!strcmp (id, results->point_id + j * MAX_LABEL_LENGTH))
              break;
          x0 = results->point_x[j];
          y0 = results->point_y[j];
          z0 = results->point_z[j];
          strncpy (id, results->pipe_outlet_id + i * MAX_LABEL_LENGTH,
                   MAX_LABEL_LENGTH * sizeof (char));
          for (j = 0; j < header->npoints; ++j)
            if (!strcmp (id, results->point_id + j * MAX_LABEL_LENGTH))
              {
                dx = (results->point_x[j] - x0) / (ncells - 1);
                dy = (results->point_y[j] - y0) / (ncells - 1);
                dz = (results->point_z[j] - z0) / (ncells - 1);
                break;
              }
          if (j == header->npoints)
            {
              fprintf (stderr, "ID=%s\n", id);
              m = _("Bad point identifier");
              goto exit_on_error;
            }
          for (j = l = 0; j < ncells; ++j)
            {
              x = x0 + j * dx;
              y = y0 + j * dy;
              z = z0 + j * dz;
              fprintf (file, "%.3lf %.3lf %.3lf", x, y, z);
              for (k = 0; k < n; ++k, ++l)
                fprintf (file, " %.9lg", variable[l]);
              fprintf (file, "\n");
            }
        }

      // error
      else
        {
          m = _("Bad XML node");
          goto exit_on_error;
        }
    }

  // free memory
#if DEBUG_RESULTS
  fprintf (stderr, "results_open_xml: free memory\n");
#endif
  free (variable);
  xmlFreeDoc (doc);
  g_free (directory);
  fclose (file);

  // exit on success
#if DEBUG_RESULTS
  fprintf (stderr, "results_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
#if DEBUG_RESULTS
  fprintf (stderr, "results_open_xml: exit on error (%s)\n", m);
#endif
  results_error (m);

  // free memory
#if DEBUG_RESULTS
  fprintf (stderr, "results_open_xml: free memory\n");
#endif
  free (variable);
  xmlFreeDoc (doc);
  g_free (directory);
  if (file)
    fclose (file);

#if DEBUG_RESULTS
  fprintf (stderr, "results_open_xml: end\n");
#endif
  return 0;
}
