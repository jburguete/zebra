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
  double dn[MAX_NUTRIENTS];
  ///< array of nutrient concentration differences.
  double ds[MAX_SPECIES];
  ///< array of species concentration differences.
#if NUMERICAL_ORDER > 1
  double dn2[MAX_NUTRIENTS];
  ///< array of 2nd order nutrient concentration differences.
  double ds2[MAX_SPECIES];
  ///< array of 2nd order species concentration differences.
  Cell *left2;                  ///< left cell.
  Cell *right2;                 ///< right cell.
#endif
  Cell *left;                   ///< left cell.
  Cell *right;                  ///< right cell.
  double size;                  ///< longitudinal size.
  double vdt;                   ///< velocity times time step size.
} Wall;

void wall_init (Wall * wall, Cell * left, Cell * right);
void wall_set_flow (Wall * wall, double vdt);
void wall_set (Wall * wall);
void wall_increments_p (Wall * wall);
void wall_increments_n (Wall * wall);
#if NUMERICAL_ORDER > 1
void wall_init_2 (Wall * wall, Cell * left2, Cell * right2);
void wall_set_2n (Wall * wall, Wall * next);
void wall_set_2p (Wall * wall, Wall * prev);
void wall_increments_2p (Wall * wall);
void wall_increments_2n (Wall * wall);
#endif

#endif
