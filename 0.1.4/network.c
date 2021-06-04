/**
 * \file network.c
 * \brief source file to define the networks.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
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
#include "pipe.h"
#include "junction.h"
#include "inlet.h"
#include "outlet.h"
#include "network.h"

/**
 * function to set an error message opening an networks input file.
 */
static inline void
network_error (const char *msg) ///< error message.
{
  error_message (_("Network"), (char *) g_strdup (msg));
}

/**
 * function to init an empty network.
 */
void
network_null (Network * network)        ///< pointer to the network struct data.
{
#if DEBUG_NETWORK
  fprintf (stderr, "network_null: start\n");
#endif
  network->inlet = NULL;
  network->outlet = NULL;
  network->ninlets = network->noutlets = 0;
#if DEBUG_NETWORK
  fprintf (stderr, "network_null: end\n");
#endif
}

/**
 * function to free the memory used by the networks data.
 */
void
network_destroy (Network * network)     ///< pointer to the network struct data.
{
  unsigned int i;
#if DEBUG_NETWORK
  fprintf (stderr, "network_destroy: start\n");
#endif
  for (i = 0; i < network->noutlets; ++i)
    outlet_destroy (network->outlet + i);
  free (network->outlet);
  for (i = 0; i < network->ninlets; ++i)
    inlet_destroy (network->inlet + i);
  free (network->inlet);
  network_null (network);
#if DEBUG_NETWORK
  fprintf (stderr, "network_destroy: end\n");
#endif
}

/**
 * function to copy on a point struct the data from a Epanet node struct.
 */
static void
point_net_copy (Point * point,  ///< pointer to the point struct data.
                NetNode * node) ///< pointer to the Epanet node struct data.
{
#if DEBUG_NETWORK
  fprintf (stderr, "point_net_copy: start\n");
#endif
  point->id = node->id;
  point->x = node->x;
  point->y = node->y;
#if DEBUG_NETWORK
  fprintf (stderr, "point_net_copy: end\n");
#endif
}

/**
 * function to copy on a pipe struct the data from a Epanet pipe struct.
 */
static void
pipe_net_copy (Pipe * pipe,     ///< pointer to the pipe struct data.
               NetPipe * net_pipe)
               ///< pointer to the Epanet pipe struct data.
{
#if DEBUG_NETWORK
  fprintf (stderr, "pipe_net_copy: start\n");
#endif
  pipe->id = net_pipe->id;
  pipe->diameter = net_pipe->diameter;
  pipe->roughness = net_pipe->roughness;
#if DEBUG_NETWORK
  fprintf (stderr, "pipe_net_copy: end\n");
#endif
}

/**
 * function to read network data on a INP Epanet file.
 *
 * \return 1 on success, 0 on error.
 */
static int
network_open_inp (Network * network,    ///< pointer to the network struct data.
                  FILE * file)  ///< .INP Epanet file. 
{
  NetNode *node;
  NetJunction *junction;
  NetPipe *pipe;
  NetReservoir *reservoir;
  Point **pp;
  Point *p1, *p2;
#if PIPE_LENGTHS_SAVE
  FILE *file_log;
#endif
  int *se;
  const char *m;
  char *buffer;
  size_t n;
  ssize_t r;
  int e, error_code = 0;
  unsigned int i, id, maxid, nnodes, njunctions, npipes, nreservoirs;

#if DEBUG_NETWORK
  fprintf (stderr, "network_open_inp: start\n");
#endif

  // init buffers
  node = NULL;
  junction = NULL;
  pipe = NULL;
  reservoir = NULL;
  pp = NULL;
  se = NULL;

  // reading title lines
  buffer = NULL, n = 0;
  r = getline (&buffer, &n, file);
  if (r <= 0 || strncmp (buffer, EPANET_TITLE, 5))
    {
      m = _("Bad Epanet title");
      goto exit_on_error;
    }
  do
    {
      r = getline (&buffer, &n, file);
      if (r < 0)
        {
          m = _("Bad Epanet file");
          goto exit_on_error;
        }
    }
  while (*buffer != '[');

  // processing sections
  nnodes = njunctions = npipes = nreservoirs = 0;
  do
    {
#if DEBUG_NETWORK
      fprintf (stderr, "network_open_inp: section=%s\n", buffer);
#endif

      // end
      if (!strncmp (buffer, EPANET_END, 5))
        break;

      // coordinates
      else if (!strncmp (buffer, EPANET_COORDINATES, 5))
        {
          do
            {
              r = getline (&buffer, &n, file);
              if (r < 0)
                {
                  m = _("Bad Epanet coordinates section");
                  goto exit_on_error;
                }
              if (r <= 1 || *buffer == ';')
                continue;
              if (*buffer == '[')
                break;
              i = nnodes;
              ++nnodes;
              node = (NetNode *) realloc (node, nnodes * sizeof (NetNode));
              e = sscanf (buffer, "%u%lf%lf", &node[i].id, &node[i].x,
                          &node[i].y);
              if (e < 3)
                {
                  m = _("Bad Epanet coordinates section");
                  goto exit_on_error;
                }
            }
          while (1);
        }

      // junctions
      else if (!strncmp (buffer, EPANET_JUNCTIONS, 5))
        {
          do
            {
              r = getline (&buffer, &n, file);
              if (r < 0)
                {
                  m = _("Bad Epanet junctions section");
                  goto exit_on_error;
                }
              if (r <= 1 || *buffer == ';')
                continue;
              if (*buffer == '[')
                break;
              i = njunctions;
              ++njunctions;
              junction = (NetJunction *)
                realloc (junction, njunctions * sizeof (NetJunction));
              e = sscanf (buffer, "%u%lf%lf%*s", &junction[i].id,
                          &junction[i].elevation, &junction[i].demand);
              if (e < 3)
                {
                  m = _("Bad Epanet junctions section");
                  goto exit_on_error;
                }
            }
          while (1);
        }

      // pipes
      else if (!strncmp (buffer, EPANET_PIPES, 5))
        {
          do
            {
              r = getline (&buffer, &n, file);
              if (r < 0)
                {
                  m = _("Bad Epanet pipes section");
                  goto exit_on_error;
                }
              if (r <= 1 || *buffer == ';')
                continue;
              if (*buffer == '[')
                break;
              i = npipes;
              ++npipes;
              pipe = (NetPipe *) realloc (pipe, npipes * sizeof (NetPipe));
              e = sscanf (buffer, "%u%u%u%lf%lf%lf%*f%*s", &pipe[i].id,
                          &pipe[i].node1, &pipe[i].node2, &pipe[i].length,
                          &pipe[i].diameter, &pipe[i].roughness);
              if (e < 6)
                {
                  m = _("Bad Epanet pipes section");
                  goto exit_on_error;
                }
            }
          while (1);
        }

      // reservoirs
      else if (!strncmp (buffer, EPANET_RESERVOIRS, 5))
        {
          do
            {
              r = getline (&buffer, &n, file);
              if (r < 0)
                {
                  m = _("Bad Epanet reservoirs section");
                  goto exit_on_error;
                }
              if (r <= 1 || *buffer == ';')
                continue;
              if (*buffer == '[')
                break;
              i = nreservoirs;
              ++nreservoirs;
              reservoir = (NetReservoir *)
                realloc (reservoir, nreservoirs * sizeof (NetReservoir));
              e = sscanf (buffer, "%u%lf*s", &reservoir[i].id,
                          &reservoir[i].head);
              if (e < 2)
                {
                  m = _("Bad Epanet reservoirs section");
                  goto exit_on_error;
                }
            }
          while (1);
        }

      // unknown
      else
        {
          do
            {
              r = getline (&buffer, &n, file);
              if (r < 0)
                {
                  m = _("No Epanet end section");
                  goto exit_on_error;
                }
            }
          while (*buffer != '[');
        }
    }
  while (1);

#if DEBUG_NETWORK
  fprintf (stderr,
           "network_open_inp: nodes=%u junctions=%u pipes=%u reservoirs=%u\n",
           nnodes, njunctions, npipes, nreservoirs);
#endif

  // building graph
  maxid = 0;
  for (i = 0; i < nnodes; ++i)
    maxid = maxid > node[i].id ? maxid : node[i].id;
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_inp: maxid=%u\n", maxid);
#endif
  pp = (Point **) malloc (maxid * sizeof (Point *));
  se = (int *) malloc (maxid * sizeof (int));
  for (i = 0; i < maxid; ++i)
    {
      pp[i] = NULL;
      se[i] = 0;
    }
  network->npoints = nnodes;
  network->point = (Point *) malloc (nnodes * sizeof (Point));
  for (i = 0; i < nnodes; ++i)
    {
      point_net_copy (network->point + i, node + i);
      id = node[i].id;
      if (pp[id])
        {
          m = _("Duplicated node");
          goto exit_on_error;
        }
      pp[id] = network->point + i;
    }
  for (i = 0; i < njunctions; ++i)
    {
      id = junction[i].id;
      if (se[id])
        {
          m = _("Duplicated junction");
          goto exit_on_error;
        }
      if (!pp[id])
        {
          m = _("Unknown node indentifier on junction");
          goto exit_on_error;
        }
      se[id] = 1;
      pp[id]->z = junction[i].elevation;
    }
  for (i = 0; i < nreservoirs; ++i)
    {
      id = reservoir[i].id;
      if (se[id])
        {
          m = _("Duplicated reservoir");
          goto exit_on_error;
        }
      if (!pp[id])
        {
          m = _("Unknown node indentifier on reservoir");
          goto exit_on_error;
        }
      se[id] = 1;
      pp[id]->z = reservoir[i].head;
    }
#if PIPE_LENGTHS_SAVE
  file_log = fopen (PIPE_LENGTHS_FILE, "w");
#endif
  network->npipes = npipes;
  network->pipe = (Pipe *) malloc (npipes * sizeof (Pipe));
  for (i = 0; i < n; ++i)
    {
      pipe_net_copy (network->pipe + i, pipe + i);
      id = pipe[i].node1;
      if (!pp[id])
        {
          m = _("Unknown 1st node indentifier on pipe");
          goto exit_on_error;
        }
      if (!se[id])
        {
          m = _("No elevation of 1st node");
          goto exit_on_error;
        }
      network->pipe[i].inlet = p1 = pp[id];
      id = pipe[i].node2;
      if (!pp[id])
        {
          m = _("Unknown 2nd node indentifier on pipe");
          goto exit_on_error;
        }
      if (!se[id])
        {
          m = _("No elevation of 2nd node");
          goto exit_on_error;
        }
      network->pipe[i].outlet = p2 = pp[id];
      if (p1 == p2)
        {
          m = _("Same nodes for inlet and outlet");
          goto exit_on_error;
        }
      switch (network->pipe_length)
        {
        case PIPE_LENGTH_COORDINATES:
          network->pipe[i].length
            = distance (p1->x, p1->y, p1->z, p2->x, p2->y, p2->z);
          break;
        default:
          network->pipe[i].length = pipe[i].length;
        }
#if DEBUG_NETWORK
      fprintf (stderr, "network_open_inp: pipe length1=%lg length2=%lg\n",
               network->pipe[i].length, pipe[i].length);
#endif
#if PIPE_LENGTHS_SAVE
      fprintf (file_log, "%lg %lg %u\n", network->pipe[i].length,
               pipe[i].length, pipe[i].id);
#endif
    }
#if PIPE_LENGTHS_SAVE
  fclose (file_log);
#endif

  error_code = 1;

exit_on_error:

  // free buffers memory
  free (se);
  free (pp);
  free (reservoir);
  free (pipe);
  free (junction);
  free (node);
  free (buffer);
  fclose (file);

  // set error message on error
  if (!error_code)
    network_error (m);

  // end
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_inp: end\n");
#endif
  return error_code;
}

/**
 * function to read the networks data on a XML file.
 *
 * \return 1 on succes, 0 on error.
 */
int
network_open_xml (Network * network,    ///< pointer to the network struct data.
                  char *file_name)      ///< input file name.
{
  char name[BUFFER_SIZE];
  xmlDoc *doc;
  xmlNode *node;
  xmlChar *buffer = NULL;
  FILE *file;
  const char *m;
  char *directory;

  // start
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_xml: start\n");
#endif
  network_null (network);

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
  if (xmlStrcmp (node->name, XML_NETWORK))
    {
      m = _("Bad root XML node");
      goto exit_on_error;
    }

  // reading Epanet file
  buffer = xmlGetProp (node, XML_PIPE_LENGTH);
  if (!buffer || !xmlStrcmp (buffer, XML_COORDINATES))
    network->pipe_length = PIPE_LENGTH_COORDINATES;
  else if (!xmlStrcmp (buffer, XML_LENGTH))
    network->pipe_length = PIPE_LENGTH_LENGTH;
  else
    {
      xmlFree (buffer);
      m = _("Unknown pipe length model");
      goto exit_on_error;
    }
  xmlFree (buffer);
  buffer = xmlGetProp (node, XML_EPANET);
  if (!buffer)
    {
      m = _("No Epanet file");
      goto exit_on_error;
    }
  directory = g_path_get_dirname (file_name);
  snprintf (name, BUFFER_SIZE, "%s/%s", directory, (char *) buffer);
  g_free (directory);
  xmlFree (buffer);
  file = fopen ((char *) name, "r");
  if (!file)
    {
      m = _("Unable to open the Epanet file");
      goto exit_on_error;
    }
  if (!network_open_inp (network, file))
    {
      m = error_msg;
      goto exit_on_error;
    }

  // reading inlets and outlets
  for (node = node->children; node; node = node->next)
    {
      if (!xmlStrcmp (node->name, XML_INLET))
        {
          ++network->ninlets;
          network->inlet
            = (Inlet *) realloc (network->inlet,
                                 network->ninlets * sizeof (Inlet));
          if (!inlet_open_xml (network->inlet + network->ninlets - 1, node))
            {
              m = error_msg;
              goto exit_on_error;
            }
        }
      else if (!xmlStrcmp (node->name, XML_OUTLET))
        {
          ++network->noutlets;
          network->outlet
            = (Outlet *) realloc (network->outlet,
                                  network->noutlets * sizeof (Outlet));
          if (!outlet_open_xml (network->outlet + network->noutlets - 1, node))
            {
              m = error_msg;
              goto exit_on_error;
            }
        }
      else
        {
          m = _("Bad XML node");
          goto exit_on_error;
        }
    }

  if (!network->ninlets)
    {
      m = _("No inlets");
      goto exit_on_error;
    }
  if (!network->noutlets)
    {
      m = _("No outlets");
      goto exit_on_error;
    }

  // exit on success
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  network_error (m);

  // free memory on error
  network_destroy (network);

  // end
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_xml: end\n");
#endif
  return 0;
}
