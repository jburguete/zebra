/**
 * \file simulation.c
 * \brief source file to define the simulation.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <math.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"
#include "nutrient.h"
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
#include "simulation.h"

unsigned int numerical_order;   ///< accurate order of the numerical method.
unsigned int dispersion_model;  ///< dispersion model.

/**
 * function to set an error message opening a simulation input file.
 */
static inline void
simulation_error (const char *msg)      ///< error message.
{
  error_message (_("Simulation"), (char *) g_strdup (msg));
}

/**
 * function to open the simulation configuration file.
 *
 * \return 1 on success, 0 on error.
 */
int
simulation_open_xml (Simulation * simulation,
                     ///< pointer to the simulation struct data.
                     char *file_name)   ///< simulation configuration file name.
{
  char name[BUFFER_SIZE];
  Network *network;
  xmlDoc *doc;
  xmlNode *node;
  xmlChar *buffer = NULL;
  const char *m;
  char *directory;
  int e;

#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_open_xml: start\n");
#endif

  // init variables
  network = simulation->network;
  network_null (network);

  // open file
  directory = g_path_get_dirname (file_name);
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
  if (xmlStrcmp (node->name, XML_SIMULATION))
    {
      m = _("Bad root XML node");
      goto exit_on_error;
    }

  // reading properties
  simulation->initial_time = xml_node_get_time (node, XML_INITIAL_TIME, &e);
  if (!e)
    {
      m = _("Bad initial time");
      goto exit_on_error;
    }
  simulation->final_time = xml_node_get_time (node, XML_FINAL_TIME, &e);
  if (!e || simulation->final_time < simulation->initial_time)
    {
      m = _("Bad final time");
      goto exit_on_error;
    }
  simulation->cfl
    = xml_node_get_float_with_default (node, XML_CFL, &e, DEFAULT_CFL);
  network->cell_size = xml_node_get_float (node, XML_CELL_SIZE, &e);
  if (!e || network->cell_size <= 0.)
    {
      m = _("Bad cell size");
      goto exit_on_error;
    }
  simulation->saving_step = xml_node_get_float (node, XML_SAVING_STEP, &e);
  if (!e || simulation->saving_step <= 0.)
    {
      m = _("Bad saving time step size");
      goto exit_on_error;
    }
  buffer = xmlGetProp (node, XML_RESULTS);
  if (!buffer)
    {
      m = _("No results file");
      goto exit_on_error;
    }
  snprintf (simulation->results, BUFFER_SIZE, "%s/%s", directory,
            (char *) buffer);
  xmlFree (buffer);
  numerical_order
    = xml_node_get_uint_with_default (node, XML_NUMERICAL_ORDER, &e, 2);
  if (!e || numerical_order < 1 || numerical_order > 2)
    {
      m = _("Bad numerical order");
      goto exit_on_error;
    }
  buffer = xmlGetProp (node, XML_DISPERSION_MODEL);
  if (!buffer)
    dispersion_model = DISPERSION_MODEL_NONE;
  else if (!xmlStrcmp (buffer, XML_RUTHERFORD))
    dispersion_model = DISPERSION_MODEL_RUTHERFORD;
  else
    {
      m = _("Unknown dispersion model");
      goto exit_on_error;
    }
  xmlFree (buffer);

  // open nutrients
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_open_xml: open nutrients\n");
#endif
  buffer = xmlGetProp (node, XML_NUTRIENTS);
  if (!buffer)
    {
      m = _("No nutrients file");
      goto exit_on_error;
    }
  snprintf (name, BUFFER_SIZE, "%s/%s", directory, (char *) buffer);
  xmlFree (buffer);
  if (!nutrient_open_xml (name))
    {
      m = error_msg;
      goto exit_on_error;
    }

  // open species
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_open_xml: open species\n");
#endif
  buffer = xmlGetProp (node, XML_SPECIES);
  if (!buffer)
    {
      m = _("No species file");
      goto exit_on_error;
    }
  snprintf (name, BUFFER_SIZE, "%s/%s", directory, (char *) buffer);
  xmlFree (buffer);
  if (!species_open_xml (name))
    {
      m = error_msg;
      goto exit_on_error;
    }

  // open network
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_open_xml: open network\n");
#endif
  buffer = xmlGetProp (node, XML_NETWORK);
  if (!buffer)
    {
      m = _("No network file");
      goto exit_on_error;
    }
  if (!network_open_xml (network, directory, (char *) buffer))
    {
      m = error_msg;
      goto exit_on_error;
    }

  // free memory
  xmlFree (buffer);
  g_free (directory);
  xmlFreeDoc (doc);

#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_open_xml: end\n");
#endif
  return 1;

exit_on_error:

  // set error message
  simulation_error (m);

  // free memory
  xmlFree (buffer);
  g_free (directory);
  xmlFreeDoc (doc);

#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_open_xml: end\n");
#endif
  return 0;
}
