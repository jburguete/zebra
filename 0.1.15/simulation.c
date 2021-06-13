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
#include "point.h"
#include "cell.h"
#include "wall.h"
#include "pipe.h"
#include "junction.h"
#include "inlet.h"
#include "network.h"
#include "results.h"
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
  Network *network;
  xmlDoc *doc;
  xmlNode *node;
  xmlChar *buffer;
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
  if (!e)
    {
      m = _("Bad cell size");
      goto exit_on_error;
    }
  simulation->saving_step = xml_node_get_float (node, XML_SAVING_STEP, &e);
  if (!e)
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

/**
 * function to run a simulation.
 */
void
simulation_run (Simulation * simulation)
{
  Results results[1];
  Network *network;
  FILE *file;
  double initial_time, final_time, cfl, saving_time, saving_step;
#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_run: end\n");
#endif

  // initial conditions
  network = simulation->network;
  initial_time = simulation->initial_time;
  final_time = simulation->final_time;
  saving_step = simulation->saving_step;
  cfl = simulation->cfl;
  network_set_discharges (network);
  network_initial (network);
  results_init (results, network, initial_time, final_time, saving_step);
  results_set (results, network);
  file = fopen (simulation->results, "wb");
  results_write_header (results, file);

  // bucle
  for (current_time = initial_time; current_time < final_time;
       current_time = saving_time)
    {

      // time for saving results
      saving_time = fmin (current_time + saving_step, final_time);

      // inner bucle
      for (; current_time < saving_time; current_time = next_time)
        {

          // update discharges and velocities
          network_update_discharges (network);

          // calculate time step size
          next_time = network_maximum_time (network, saving_time, cfl);
          time_step = next_time - current_time;

          // perform numerical method
          network_step (network);

#if DEBUG_SIMULATION
          fprintf (stderr,
                   "simulation_run: current_time=%.14lg next_time=%.14lg "
                   "time_step=%lg\n", current_time, next_time, time_step);
#endif
        }

      // saving results
      results_set (results, network);
      results_write_variables (results, file);

    }

  // freeing
  fclose (file);
  results_destroy (results);

#if DEBUG_SIMULATION
  fprintf (stderr, "simulation_run: end\n");
#endif
}
