/**
 * \file network.h
 * \brief header file to define the networks.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
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
  char id[MAX_LABEL_LENGTH];    ///< identifier.
  double x;                     ///< x-coordinate.
  double y;                     ///< y-coordinate.
} NetNode;

/**
 * \struct NetJunction
 * \brief struct to define an Epanet junction.
 */
typedef struct
{
  char id[MAX_LABEL_LENGTH];    ///< identifier.
  double elevation;             ///< elevation.
  double demand;                ///< demand.
} NetJunction;

/**
 * \struct NetPipe
 * \brief struct to define an Epanet pipe.
 */
typedef struct
{
  char id[MAX_LABEL_LENGTH];    ///< identifier.
  char node1[MAX_LABEL_LENGTH]; ///< 1st node identifier.
  char node2[MAX_LABEL_LENGTH]; ///< 2nd node identifier.
  double length;                ///< length.
  double diameter;              ///< diameter.
  double roughness;             ///< roughness.
} NetPipe;

/**
 * \struct NetReservoir
 * \brief struct to define an Epanet reservoir.
 */
typedef struct
{
  char id[MAX_LABEL_LENGTH];    ///< identifier.
  double head;                  ///< head.
} NetReservoir;

/**
 * \struct NetDischarges
 * \brief struct to define a set of network discharges.
 */
typedef struct
{
  Pipe **pipe;                  ///< array of pipe struct data pointers.
  double *discharge;            ///< array of pipe discharges.
  double *friction_factor;      ///< array of pipe friction factors.
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
  GHashTable *hash_points;      ///< hash table of points.
  GHashTable *hash_pipes;       ///< hash table of pipes.
  double cell_size;             ///< maximum cell size.
  unsigned int npoints;         ///< number of points.
  unsigned int npipes;          ///< number of pipes.
  unsigned int njunctions;      ///< number of junctions.
  unsigned int ninlets;         ///< number of inlets.
  unsigned int ndischarges;     ///< number of discharges sets.
  unsigned int pipe_length;     ///< type of pipe length model.
  unsigned int current_discharges;      ///< current discharges set index.
} Network;

void network_null (Network * network);
void network_destroy (Network * network);
int network_open_xml (Network * network, char *directory, char *file_name);
void network_set_flow (Network * network);

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
    network_set_flow (network);
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
  double solute_concentration[MAX_SOLUTES];
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
  for (i = 0; i < nsolutes; ++i)
    solute_concentration[i] = *(inlet->solute_concentration[i]);
  for (i = 0; i < nspecies; ++i)
    species_concentration[i] = *(inlet->species_concentration[i]);
  for (i = 0; i < n; ++i)
    pipe_initial (pipe + i, solute_concentration, species_concentration);
#if DEBUG_NETWORK
  fprintf (stderr, "network_initial: end\n");
#endif
}

/**
 * function to perform a numerical method step on a network.
 */
static inline void
network_step (Network * network,        ///< pointer to the network struct data.
              gsl_rng * rng)    ///< GSL random numbers generator.
{
  Inlet *inlet;
  Junction *junction;
  Pipe *pipe;
  double ta, taf, td, tdf, tb, tbf, bdt;
  unsigned int i, npipes, njunctions, ninlets;
#if DEBUG_NETWORK
  fprintf (stderr, "network_step: start\n");
#endif
  pipe = network->pipe;
  junction = network->junction;
  npipes = network->npipes;
  njunctions = network->njunctions;
  ninlets = network->ninlets;
  tb = current_time;
  do
    {
      tbf = fmin (next_time, tb + biological_step);
      td = tb;
      do
        {
          tdf = fmin (tbf, td + diffusion_step);
          ta = td;
          do
            {
              taf = fmin (tdf, ta + advection_step);
              for (i = 0; i < npipes; ++i)
                pipe_advection_step (pipe + i, taf - ta);
              for (i = 0; i < njunctions; ++i)
                junction_set (junction + i);
              inlet = network->inlet;
              for (i = 0; i < ninlets; ++i)
                inlet_set (inlet + i, taf);
#if DEBUG_NETWORK
              fprintf (stderr, "network_step: taf=%.14lg tdf=%.14lg\n",
                       taf, tdf);
#endif
            }
          while (taf < tdf);
          if (dispersion_model)
            for (i = 0; i < npipes; ++i)
              pipe_dispersion_step (pipe + i, tdf - td);
          for (i = 0; i < njunctions; ++i)
            junction_set (junction + i);
          inlet = network->inlet;
          for (i = 0; i < ninlets; ++i)
            inlet_set (inlet + i, taf);
          td = tdf;
#if DEBUG_NETWORK
          fprintf (stderr, "network_step: tdf=%.14lg tbf=%.14lg\n", tdf, tbf);
#endif
        }
      while (tdf < tbf);
      bdt = tbf - tb;
      for (i = 0; i < npipes; ++i)
        pipe_biological_step (pipe + i, rng, bdt);
      tb = tbf;
#if DEBUG_NETWORK
      fprintf (stderr, "network_step: tbf=%.14lg next_time=%.14lg\n",
               tbf, next_time);
#endif
    }
  while (tbf < next_time);
  for (i = 0; i < npipes; ++i)
    pipe_infestations (pipe + i);
#if DEBUG_NETWORK
  fprintf (stderr, "network_step: end\n");
#endif
}

#endif
