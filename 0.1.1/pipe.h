/**
 * \file pipe.h
 * \brief header file to define the network pipes.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef PIPE__H
#define PIPE__H 1

/**
 * \struct Pipe
 * \brief struct to define an irrigation pipe.
 */
typedef struct
{
  Point inlet;                  ///< inlet point.
  Point outlet;                 ///< outlet point.
  Cell *cell;                   ///< array of node cells.
  double length;                ///< length.
  double area;                  ///< cross sectional area.
  double discharge;             ///< discharge.
  double velocity;              ///< flow velocity.
  unsigned int ncells;          ///< number of node cells.
  unsigned int npoints;         ///< number of points.
  unsigned int id;              ///< identifier.
} Pipe;

#endif
