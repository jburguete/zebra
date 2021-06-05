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
  Point *inlet;                 ///< pointer to the inlet point.
  Point *outlet;                ///< pointer to the outlet point.
  Cell *cell;                   ///< array of node cells.
  double length;                ///< length.
  double diameter;              ///< diameter.
  double area;                  ///< cross sectional area.
  double perimeter;             ///< cross sectional perimeter.
  double roughness;             ///< roughness length.
  double discharge;             ///< current discharge.
  double velocity;              ///< current flow velocity.
  unsigned int ncells;          ///< number of node cells.
  unsigned int id;              ///< identifier.
} Pipe;

void pipe_null (Pipe * pipe);
void pipe_destroy (Pipe * pipe);
void pipe_create_mesh (Pipe * pipe, double cell_size);

#endif
