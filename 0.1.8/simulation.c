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
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"
#include "nutrient.h"
#include "species.h"
#include "point.h"
#include "cell.h"
#include "wall.h"
#include "pipe.h"
#include "junction.h"
#include "inlet.h"
#include "network.h"
#include "simulation.h"

/**
 * function to set an error message opening a simulation input file.
 */
static inline void
simulation_error (const char *msg)      ///< error message.
{
  error_message (_("Simulation"), (char *) g_strdup (msg));
}

/**
 * function to free the memory used by a simulation struct.
 */
void
simulation_destroy (Simulation * simulation)
                    ///< pointer to the simulation struct data.
{
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_destroy: start\n");
#endif
  network_destroy (simulation->network);
  species_destroy ();
  nutrient_destroy ();
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_destroy: end\n");
#endif
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
  xmlDoc *doc;
  xmlNode *node;
  xmlChar *buffer;
  const char *m;
  char *directory;
  int e;

#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_open_xml: start\n");
#endif
  network_null (simulation->network);

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
  if (!network_open_xml (simulation->network, directory, (char *) buffer))
    {
      m = error_msg;
      goto exit_on_error;
    }

  // free memory
  xmlFree (buffer);
  g_free (directory);

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

#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_open_xml: end\n");
#endif
  return 0;
}

/**
 * function to run a simulation.
 */
void
simulation_run (Simulation * simulation)
{
  Network *network;
  double final_time, cfl;
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_run: end\n");
#endif
  network = simulation->network;
  final_time = simulation->final_time;
  cfl = simulation->cfl;
  for (current_time = simulation->initial_time; current_time < final_time;
       current_time = next_time)
    {
      network_update_discharges (network);
      next_time = network_maximum_time (network, final_time, cfl);
      time_step = next_time - current_time;
#if DEBUG_SIMULATION
      fprintf (stderr,
               "simulation_run: current_time=%lg next_time=%lg time_step=%lg\n",
               current_time, next_time, time_step);
#endif
    }
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_run: end\n");
#endif
}