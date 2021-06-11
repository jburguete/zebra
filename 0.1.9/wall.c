/**
 * \file wall.h
 * \brief source file to define the calculation mesh walls.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <libxml/parser.h>
#include "config.h"
#include "tools.h"
#include "nutrient.h"
#include "species.h"
#include "point.h"
#include "cell.h"
#include "wall.h"

/**
 * function to init the data of a mesh wall struct.
 */
void
wall_init (Wall * wall,         ///< pointer to the mesh wall struct data.
           Cell * left,         ///< pointer to the left cell struct data.
           Cell * right)        ///< pointer to the right cell struct data.
{
#if DEBUG_WALL
  fprintf (stderr, "wall_init: start\n");
#endif
  wall->left = left;
  wall->right = right;
  wall->size = left->distance;
#if DEBUG_WALL
  fprintf (stderr, "wall_init: end\n");
#endif
}

/**
 * function to set the data of a mesh wall struct.
 */
void
wall_set (Wall * wall)          ///< pointer to the mesh wall struct data.
{
  Cell *left, *right;
  double *n1, *s1, *n2, *s2, *dn, *ds;
  unsigned int i;
#if DEBUG_WALL
  fprintf (stderr, "wall_set: start\n");
#endif
  left = wall->left;
  right = wall->right;
  n1 = left->nutrient_concentration;
  s1 = left->species_concentration;
  n2 = right->nutrient_concentration;
  s2 = right->species_concentration;
  dn = wall->dn;
  ds = wall->ds;
  for (i = 0; i < nnutrients; ++i)
    dn[i] = n2[i] - n1[i];
  for (i = 0; i < nspecies; ++i)
    ds[i] = s2[i] - s1[i];
#if DEBUG_WALL
  fprintf (stderr, "wall_set: end\n");
#endif
}
