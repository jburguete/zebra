/**
 * \file cell.h
 * \brief header file to define the calculation node cells.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef CELL__H
#define CELL__H 1

/**
 * \struct Cell
 * \brief struct to define a node cell for calculation.
 */
typedef struct
{
  double *species_density;      ///< array of species density.
  double *species_infestation;  ///< array of species infestation rate.
  double position;              ///< position in the pipe.
  double length;                ///< longitudinal length.
  double area;                  ///< cross sectional area.
  double velocity;              ///< flow velocity.
  double dispersion;            ///< water dispersion coefficient.
  double roughness;             ///< roughness length.
  double friction;              ///< friction factor.
} Cell;

#endif
