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
 * \struct Network
 * \brief struct to define a network.
 */
typedef struct
{
  Point *point;                 ///< array of points.
  Pipe *pipe;                   ///< array of pipes.
  Junction *junction;           ///< array of junctions.
  Inlet *inlet;                 ///< array of inlets.
  Outlet *outlet;               ///< array of outlets.
  unsigned int npoints;         ///< number of points.
  unsigned int npipes;          ///< number of pipes.
  unsigned int njunctions;      ///< number of junctions.
  unsigned int ninlets;         ///< number of inlets.
  unsigned int noutlets;        ///< number of outlets.
  unsigned int pipe_length;     ///< type of pipe length model.
} Network;

void network_null (Network * network);
void network_destroy (Network * network);
int network_open_xml (Network * network, char *file_name);

#endif
