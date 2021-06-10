/**
 * \file wall.h
 * \brief header file to define the calculation mesh walls.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef WALL__H
#define WALL__H 1

/**
 * \struct Wall
 * \brief struct to define a mesh wall for calculation.
 */
typedef struct
{
  Cell *left;                   ///< left cell.
  Cell *right;                  ///< right cell.
  double dn[MAX_NUTRIENTS];
  ///< array of nutrient concentration differences.
  double ds[MAX_SPECIES];
  ///< array of species concentration differences.
#if NUMERICAL_ORDER > 1
  double dn2[MAX_NUTRIENTS];
  ///< array of 2nd order nutrient concentration differences.
  double ds2[MAX_SPECIES];
  ///< array of 2nd order species concentration differences.
#endif
  double size;                  ///< longitudinal size.
  double velocity;              ///< flow velocity.
} Wall;

void wall_init (Wall * wall, Cell * left, Cell * right);
void wall_set (Wall * wall);

#endif
