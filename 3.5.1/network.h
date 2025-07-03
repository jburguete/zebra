/**
 * \file network.h
 * \brief header file to define the networks.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
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
network_update_discharges (Network *network)
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
 * function to calculate the maximum time step size allowed by a network.
 *
 * \return time step size.
 */
static inline double
network_step_size (Network *network,
                   ///< pointer to the network struct data.
                   double cfl)  ///< CFL number.
{
  Pipe *pipe;
  double dt;
  unsigned int i;
#if DEBUG_NETWORK
  fprintf (stderr, "network_step_size: start\n");
#endif
  dt = INFINITY;
  pipe = network->pipe;
  for (i = 0; i < network->npipes; ++i)
    dt = fmin (dt, pipe_step_size (pipe + i, cfl));
#if DEBUG_NETWORK
  fprintf (stderr, "network_step_size: dt=%.14lg\n", dt);
  fprintf (stderr, "network_step_size: end\n");
#endif
  return dt;
}

/**
 * function to calculate the maximum next time allowed by a network.
 *
 * \return maximum allowed next time in seconds since 1970.
 */
static inline double
network_maximum_time (Network *network,
                      ///< pointer to the network struct data.
                      double final_time)
                      ///< final time in seconds since 1970.
{
  Inlet *inlet;
  double t;
  unsigned int i;
#if DEBUG_NETWORK
  fprintf (stderr, "network_maximum_time: start\n");
#endif
  t = final_time;
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
 * function to calculate the network infestations by species.
 */
static inline void
network_infestations (Network *network)
                      ///< pointer to the network struct data.
{
  unsigned int i, npipes;
  Pipe *pipe;
#if DEBUG_NETWORK
  fprintf (stderr, "network_infestations: start\n");
#endif
  pipe = network->pipe;
  npipes = network->npipes;
  for (i = 0; i < npipes; ++i)
    pipe_infestations (pipe + i);
#if DEBUG_NETWORK
  fprintf (stderr, "network_infestations: end\n");
#endif
}

/**
 * function to set the initial conditions on a network.
 */
static inline void
network_initial (Network *network)      ///< pointer to the network struct data.
{
  double solute_concentration[MAX_SOLUTES];
  double species_concentration[MAX_SPECIES];
  Inlet *inlet;
  Pipe *pipe;
  double *c;
  unsigned int i, n;
#if DEBUG_NETWORK
  fprintf (stderr, "network_initial: start\n");
#endif
  inlet = network->inlet;
  pipe = network->pipe;
  n = network->npipes;
  for (i = 0; i < MAX_SOLUTES; ++i)
    {
      c = inlet->solute_concentration[i];
      if (c)
        solute_concentration[i] = *c;
      else
        solute_concentration[i] = 0.;
    }
  for (i = 0; i < MAX_SPECIES; ++i)
    {
      c = inlet->species_concentration[i];
      if (c)
        species_concentration[i] = *c;
      else
        species_concentration[i] = 0.;
    }
  for (i = 0; i < n; ++i)
    pipe_initial (pipe + i, solute_concentration, species_concentration);
  network_infestations (network);
#if DEBUG_NETWORK
  fprintf (stderr, "network_initial: end\n");
#endif
}

/**
 * function to perform a numerical method step on a network.
 */
static inline void
network_step (Network *network, ///< pointer to the network struct data.
              gsl_rng *rng)     ///< GSL random numbers generator.
{
  Inlet *inlet;
  Junction *junction;
  Pipe *pipe;
  double ta, taf, td, tdf, tb, tbf, dt;
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
          tdf = fmin (tbf, td + dispersion_step);
          ta = td;
          do
            {
              taf = fmin (tdf, ta + advection_step);
              dt = taf - ta;
              for (i = 0; i < npipes; ++i)
                pipe_advection_step (pipe + i, dt);
              for (i = 0; i < njunctions; ++i)
                junction_set (junction + i);
              inlet = network->inlet;
              for (i = 0; i < ninlets; ++i)
                inlet_set (inlet + i, ta, taf);
              ta = taf;
#if DEBUG_NETWORK
              fprintf (stderr, "network_step: taf=%.14lg tdf=%.14lg\n",
                       taf, tdf);
#endif
            }
          while (taf < tdf);
          dt = tdf - td;
          if (dispersion_model)
            for (i = 0; i < npipes; ++i)
              pipe_dispersion_step (pipe + i, dt);
          for (i = 0; i < npipes; ++i)
            pipe_solute_decay_step (pipe + i, dt);
          for (i = 0; i < njunctions; ++i)
            junction_set_with_outputs (junction + i, dt);
          inlet = network->inlet;
          for (i = 0; i < ninlets; ++i)
            inlet_set (inlet + i, taf, tdf);
          td = tdf;
#if DEBUG_NETWORK
          fprintf (stderr, "network_step: tdf=%.14lg tbf=%.14lg\n", tdf, tbf);
#endif
        }
      while (tdf < tbf);
      dt = tbf - tb;
      temperature_set (tb);
      for (i = 0; i < npipes; ++i)
        pipe_biological_step (pipe + i, rng, dt);
      tb = tbf;
#if DEBUG_NETWORK
      fprintf (stderr, "network_step: tbf=%.14lg next_time=%.14lg\n",
               tbf, next_time);
#endif
    }
  while (tbf < next_time);
  network_infestations (network);
#if DEBUG_NETWORK
  fprintf (stderr, "network_step: end\n");
#endif
}

/**
 * function to write the summary.
 */
static inline void
network_summary (Network *network,      ///< pointer to the network struct data.
                 FILE *file)    ///< summary file.
{
  Inlet *inlet;
  Junction *junction;
  unsigned int i;
#if DEBUG_NETWORK
  fprintf (stderr, "network_summary: start\n");
#endif
  inlet = network->inlet;
  for (i = 0; i < network->ninlets; ++i)
    inlet_summary (inlet + i, file);
  junction = network->junction;
  for (i = 0; i < network->njunctions; ++i)
    junction_summary (junction + i, file);
#if DEBUG_NETWORK
  fprintf (stderr, "network_summary: end\n");
#endif
}

#endif
