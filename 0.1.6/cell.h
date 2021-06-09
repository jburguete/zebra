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
  double nutrient_concentration[MAX_NUTRIENTS];
  ///< array of nutrient concentrations.
  double species_concentration[MAX_SPECIES];
  ///< array of species concentrations.
  double species_infestation[MAX_SPECIES];
  ///< array of species infestation rate.
  double position;              ///< longitudinal position in the pipe.
  double size;                  ///< longitudinal size.
  double distance;              ///< longitudinal distance to the next cell.
  double area;                  ///< cross sectional area.
  double perimeter;             ///< cross sectional perimeter.
  double volume;                ///< volume.
  double velocity;              ///< flow velocity.
  double discharge;             ///< flow discharge.
  double dispersion;            ///< water dispersion coefficient.
  double roughness;             ///< roughness length.
  double friction;              ///< friction factor.
} Cell;

extern double time_step;

void cell_init (Cell * cell, double position, double distance, double size,
                double area, double perimeter);
void cell_set_flow (Cell * cell, double discharge, double velocity);

#endif
