/**
 * \file network.c
 * \brief source file to define the networks.
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
  network->point = NULL;
  network->pipe = NULL;
  network->junction = NULL;
  network->inlet = NULL;
  network->point_from_id = NULL;
  network->pipe_from_id = NULL;
  network->npoints = network->npipes = network->njunctions = network->ninlets
    = 0;
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
  free (network->pipe_from_id);
  free (network->point_from_id);
  for (i = 0; i < network->ninlets; ++i)
    inlet_destroy (network->inlet + i);
  free (network->inlet);
  free (network->junction);
  for (i = 0; i < network->npipes; ++i)
    pipe_destroy (network->pipe + i);
  free (network->pipe);
  free (network->point);
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
  pipe->inlet_id = net_pipe->node1;
  pipe->outlet_id = net_pipe->node2;
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
  Junction *pj;
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
  unsigned int i, j, id, maxid, nnodes, njunctions, npipes, nreservoirs;

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
  pj = NULL;

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
              if (r <= 2 || *buffer == ';')
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
              if (r <= 2 || *buffer == ';')
                continue;
              if (*buffer == '[')
                break;
              i = njunctions++;
              junction = (NetJunction *)
                realloc (junction, njunctions * sizeof (NetJunction));
              printf ("njunctions=%u r=%ld\n", njunctions, r);
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
              if (r <= 2 || *buffer == ';')
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
              if (r <= 2 || *buffer == ';')
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
  network->max_point_id = maxid;
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_inp: max_point_id=%u\n", maxid);
#endif
  network->point_from_id = pp
    = (Point **) malloc ((maxid + 1) * sizeof (Point *));
  se = (int *) malloc ((maxid + 1) * sizeof (int));
  pj = (Junction *) malloc ((maxid + 1) * sizeof (Junction));
  for (i = 0; i <= maxid; ++i)
    {
      pp[i] = NULL;
      se[i] = 0;
      junction_null (pj + i);
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
  for (i = 0; i < npipes; ++i)
    pipe_null (network->pipe + i);
  for (i = 0; i < npipes; ++i)
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
      junction_add_inlet (pj + id, network->pipe + i);
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
      junction_add_outlet (pj + id, network->pipe + i);
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
      pipe_create_mesh (network->pipe + i, network->cell_size);
    }
  for (i = 0; i <= maxid; ++i)
    if (pj[i].ninlets + pj[i].noutlets > 1)
      {
        j = network->njunctions;
        ++network->njunctions;
        network->junction
          = (Junction *) realloc (network->junction,
                                  network->njunctions * sizeof (Junction));
        memcpy (network->junction + j, pj + i, sizeof (Junction));
      }
#if PIPE_LENGTHS_SAVE
  fclose (file_log);
#endif
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_inp: njunctions=%u\n", network->njunctions);
#endif

  // building array of pipes from identifiers
  maxid = 0;
  for (i = 0; i < npipes; ++i)
    maxid = maxid > pipe[i].id ? maxid : pipe[i].id;
  network->max_pipe_id = maxid;
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_inp: max_pipe_id=%u\n", maxid);
#endif
  network->pipe_from_id = (Pipe **) malloc ((maxid + 1) * sizeof (Pipe *));
  for (i = 0; i <= maxid; ++i)
    network->pipe_from_id[i] = NULL;
  for (i = 0; i < npipes; ++i)
    network->pipe_from_id[pipe[i].id] = network->pipe + i;

  // success
  error_code = 1;

exit_on_error:

  // free buffers memory
  free (pj);
  free (se);
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
 * function to read an Epanet output file to set the pipe discharges.
 * 
 * \return 1 on success, 0 on error.
 */
static int
network_open_out (Network * network,    ///< pointer to the network struct data.
                  FILE * file)  ///< .OUT Epanet file.
{
  Pipe *pipe;
  char *buffer = NULL;
  double q;
  size_t n = 0;
  ssize_t r;
  int error_code = 0;
  unsigned int i, id;
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_out: start\n");
#endif
  do
    {
      r = getline (&buffer, &n, file);
      if (r < 0)
        goto exit_on_error;
      if (!strncmp (buffer, PIPE_LINE, PIPE_LENGTH))
        break;
    }
  while (1);
  if (getline (&buffer, &n, file) < 0)
    goto exit_on_error;
  for (i = 0; i < network->npipes; ++i)
    {
      if (fscanf (file, "%u%lf%*f%*f%*f", &id, &q) != 2)
        goto exit_on_error;
      if (id > network->max_pipe_id)
        goto exit_on_error;
      pipe = network->pipe_from_id[id];
      if (!pipe)
        goto exit_on_error;
      pipe_set_discharge (pipe, q);
    }
  error_code = 1;
exit_on_error:
  free (buffer);
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_out: end\n");
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
  xmlChar *buffer;
  FILE *file;
  const char *m;
  char *directory = NULL;
  int e;

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

  // setting properties
  network->cell_size = xml_node_get_float (node, XML_CELL_SIZE, &e);
  if (!e)
    {
      m = _("Bad cell size");
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

  // reading inlets
  for (node = node->children; node; node = node->next)
    {
      if (!xmlStrcmp (node->name, XML_INLET))
        {
          ++network->ninlets;
          network->inlet
            = (Inlet *) realloc (network->inlet,
                                 network->ninlets * sizeof (Inlet));
          if (!inlet_open_xml (network->inlet + network->ninlets - 1, node,
                               network->pipe, network->npipes, directory))
            {
              --network->ninlets;
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

  // exit on success
  g_free (directory);
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  network_error (m);

  // free memory on error
  g_free (directory);
  network_destroy (network);

  // end
#if DEBUG_NETWORK
  fprintf (stderr, "network_open_xml: end\n");
#endif
  return 0;
}
