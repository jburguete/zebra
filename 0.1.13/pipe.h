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
  Wall *wall;                   ///< array of mesh walls.
  double length;                ///< length.
  double diameter;              ///< diameter.
  double area;                  ///< cross sectional area.
  double perimeter;             ///< cross sectional perimeter.
  double roughness;             ///< roughness length.
  double discharge;             ///< current discharge.
  double velocity;              ///< current flow velocity.
  unsigned int ncells;          ///< number of node cells.
  unsigned int nwalls;          ///< number of mesh walls.
  unsigned int id;              ///< identifier.
  unsigned int inlet_id;        ///< inlet node identifier.
  unsigned int outlet_id;       ///< outlet node identifier.
} Pipe;

void pipe_null (Pipe * pipe);
void pipe_destroy (Pipe * pipe);
void pipe_create_mesh (Pipe * pipe, double cell_size);
void pipe_set_discharge (Pipe * pipe, double discharge);
void pipe_set_velocity (Pipe * pipe, double velocity);
Cell *pipe_node_cell (Pipe * pipe, unsigned int id);
double pipe_maximum_time (Pipe * pipe, double cfl);
void pipe_initial (Pipe * pipe, double *nutrient_concentration,
                   double *species_concentration);
void pipe_step (Pipe * pipe);

#endif
