/**
 * \file network.h
 * \brief header file to define the networks.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef NETWORK__H
#define NETWORK__H 1

///> enum to define the method to calculate the pipe lengths.
enum PipeLength
{
  PIPE_LENGTH_COORDINATES,      ///< distance between extreme nodes.
  PIPE_LENGTH_LENGTH            ///< defined length at Epanet pipes section.
};

/**
 * \struct NetNode
 * \brief struct to define an Epanet node.
 */
typedef struct
{
  double x;                     ///< x-coordinate.
  double y;                     ///< y-coordinate.
  unsigned int id;              ///< identifier.
} NetNode;

/**
 * \struct NetJunction
 * \brief struct to define an Epanet junction.
 */
typedef struct
{
  double elevation;             ///< elevation.
  double demand;                ///< demand.
  unsigned int id;              ///< identifier.
} NetJunction;

/**
 * \struct NetPipe
 * \brief struct to define an Epanet pipe.
 */
typedef struct
{
  double length;                ///< length.
  double diameter;              ///< diameter.
  double roughness;             ///< roughness.
  unsigned int node1;           ///< 1st node identifier.
  unsigned int node2;           ///< 2nd node identifier.
  unsigned int id;              ///< identifier.
} NetPipe;

/**
 * \struct NetReservoir
 * \brief struct to define an Epanet reservoir.
 */
typedef struct
{
  double head;                  ///< head.
  unsigned int id;              ///< identifier.
} NetReservoir;

/**
 * \struct NetDischarges
 * \brief struct to define a set of network discharges.
 */
typedef struct
{
  Pipe **pipe;                  ///< array of pipe struct data pointers;
  double *discharge;            ///< array of pipe discharges;
  double date;                  ///< time in seconds since 1970.
} NetDischarges;

/**
 * \struct Network
 * \brief struct to define a network.
 */
typedef struct
{
  NetDischarges *discharges;    ///< array of discharges sets.
  Point *point;                 ///< array of points.
  Pipe *pipe;                   ///< array of pipes.
  Junction *junction;           ///< array of junctions.
  Inlet *inlet;                 ///< array of inlets.
  Point **point_from_id;
  ///< array of point pointers from identifiers. 
  Pipe **pipe_from_id;
  ///< array of pipe pointers from identifiers.
  double cell_size;             ///< maximum cell size.
  unsigned int npoints;         ///< number of points.
  unsigned int npipes;          ///< number of pipes.
  unsigned int njunctions;      ///< number of junctions.
  unsigned int ninlets;         ///< number of inlets.
  unsigned int ndischarges;     ///< number of discharges sets.
  unsigned int pipe_length;     ///< type of pipe length model.
  unsigned int max_point_id;    ///< maximum point identifier.
  unsigned int max_pipe_id;     ///< maximum pipe identifier.
  unsigned int current_discharges;      ///< current discharges set index.
} Network;

void network_null (Network * network);
void network_destroy (Network * network);
int network_open_xml (Network * network, char *directory, char *file_name);
void network_set_discharges (Network * network);

/**
 * function to update the discharges on a network.
 */
static inline void
network_update_discharges (Network * network)
                           ///< pointer to the network struct data.
{
  NetDischarges *discharges;
  unsigned int i, last;
#if DEBUG_NETWORK
  fprintf (stderr, "network_update_discharges: start\n");
#endif
  discharges = network->discharges + network->current_discharges;
  last = network->ndischarges - 1;
#if DEBUG_NETWORK
  fprintf (stderr, "network_update_discharges: current=%u last=%u\n",
           network->current_discharges, last);
#endif
  i = 0;
  while (network->current_discharges < last
         && current_time >= discharges[1].date)
    {
      ++network->current_discharges;
      ++discharges;
      ++i;
    }
  if (i)
    network_set_discharges (network);
#if DEBUG_NETWORK
  fprintf (stderr, "network_update_discharges: end\n");
#endif
}

/**
 * function to calculate the maximum next time allowed by a network.
 *
 * \return maximum allowed next time in seconds since 1970.
 */
static inline double
network_maximum_time (Network * network,
                      ///< pointer to the network struct data.
                      double final_time,
                      ///< final time in seconds since 1970.
                      double cfl)       ///< CFL number.
{
  Inlet *inlet;
  Pipe *pipe;
  double t;
  unsigned int i;
#if DEBUG_NETWORK
  fprintf (stderr, "network_maximum_time: start\n");
#endif
  t = final_time;
  pipe = network->pipe;
  for (i = 0; i < network->npipes; ++i)
    t = fmin (t, pipe_maximum_time (pipe + i, cfl));
#if DEBUG_NETWORK
  fprintf (stderr, "network_maximum_time: t=%.14lg\n", t);
#endif
  if (network->current_discharges < network->ndischarges - 1)
    t = fmin (t, network->discharges[network->current_discharges + 1].date);
#if DEBUG_NETWORK
  fprintf (stderr, "network_maximum_time: t=%.14lg\n", t);
#endif
  inlet = network->inlet;
  for (i = 0; i < network->ninlets; ++i)
    t = inlet_maximum_time (inlet + i, t);
#if DEBUG_NETWORK
  fprintf (stderr, "network_maximum_time: t=%.14lg\n", t);
  fprintf (stderr, "network_maximum_time: end\n");
#endif
  return t;
}

/**
 * function to set the initial conditions on a network.
 */
static inline void
network_initial (Network * network)     ///< pointer to the network struct data.
{
  double nutrient_concentration[MAX_NUTRIENTS];
  double species_concentration[MAX_SPECIES];
  Inlet *inlet;
  Pipe *pipe;
  unsigned int i, n;
#if DEBUG_NETWORK
  fprintf (stderr, "network_initial: start\n");
#endif
  inlet = network->inlet;
  pipe = network->pipe;
  n = network->npipes;
  for (i = 0; i < nnutrients; ++i)
    nutrient_concentration[i] = *(inlet->nutrient_concentration[i]);
  for (i = 0; i < nspecies; ++i)
    species_concentration[i] = *(inlet->species_concentration[i]);
  for (i = 0; i < n; ++i)
    pipe_initial (pipe + i, nutrient_concentration, species_concentration);
#if DEBUG_NETWORK
  fprintf (stderr, "network_initial: end\n");
#endif
}

/**
 * function to perform a numerical method step on a network.
 */
static inline void
network_step (Network * network)        ///< pointer to the network struct data.
{
  Inlet *inlet;
  Junction *junction;
  Pipe *pipe;
  unsigned int i, n;
#if DEBUG_NETWORK
  fprintf (stderr, "network_step: start\n");
#endif
  pipe = network->pipe;
  n = network->npipes;
  for (i = 0; i < n; ++i)
    pipe_step (pipe + i);
  junction = network->junction;
  n = network->njunctions;
  for (i = 0; i < n; ++i)
    junction_set (junction + i);
  inlet = network->inlet;
  n = network->ninlets;
  for (i = 0; i < n; ++i)
    inlet_set (inlet + i);
#if DEBUG_NETWORK
  fprintf (stderr, "network_step: end\n");
#endif
}

#endif
