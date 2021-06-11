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
 * function to set the flow properties on a wall.
 */
void
wall_set_flow (Wall * wall,     ///< pointer to the wall struct data.
               double vdt)      ///< flow velocity times time step size.
{
#if DEBUG_WALL
  fprintf (stderr, "wall_set_flow: start\n");
#endif
  wall->vdt = vdt;
#if DEBUG_WALL
  fprintf (stderr, "wall_set_flow: end\n");
#endif
}

/**
 * function to set the 1st order differences of a mesh wall struct.
 */
void
wall_set (Wall * wall)          ///< pointer to the mesh wall struct data.
{
  Cell *left, *right;
  double *n1, *s1, *n2, *s2, *dn, *ds;
  double vdt;
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
  vdt = wall->vdt;
  for (i = 0; i < nnutrients; ++i)
    dn[i] = vdt * (n2[i] - n1[i]);
  for (i = 0; i < nspecies; ++i)
    ds[i] = vdt * (s2[i] - s1[i]);
#if DEBUG_WALL
  fprintf (stderr, "wall_set: end\n");
#endif
}

/**
 * function to calculate 1st order variable increments on a wall for positive
 * velocities.
 */
void
wall_increments_p (Wall * wall)
{
  Cell *right;
  double *n, *s, *dn, *ds;
  double dx;
  unsigned int i;
#if DEBUG_WALL
  fprintf (stderr, "wall_increments_p: start\n");
#endif
  right = wall->right;
  n = right->nutrient_concentration;
  s = right->species_concentration;
  dn = wall->dn;
  ds = wall->ds;
  dx = right->size;
  for (i = 0; i < nnutrients; ++i)
    n[i] -= dn[i] / dx;
  for (i = 0; i < nspecies; ++i)
    s[i] -= ds[i] / dx;
#if DEBUG_WALL
  fprintf (stderr, "wall_increments_p: end\n");
#endif
}

/**
 * function to calculate 1st order variable increments on a wall for negative
 * velocities.
 */
void
wall_increments_n (Wall * wall)
{
  Cell *left;
  double *n, *s, *dn, *ds;
  double dx;
  unsigned int i;
#if DEBUG_WALL
  fprintf (stderr, "wall_increments_n: start\n");
#endif
  left = wall->left;
  n = left->nutrient_concentration;
  s = left->species_concentration;
  dn = wall->dn;
  ds = wall->ds;
  dx = left->size;
  for (i = 0; i < nnutrients; ++i)
    n[i] -= dn[i] / dx;
  for (i = 0; i < nspecies; ++i)
    s[i] -= ds[i] / dx;
#if DEBUG_WALL
  fprintf (stderr, "wall_increments_n: end\n");
#endif
}

#if NUMERICAL_ORDER > 1

/**
 * function to init the 2nd order data of a mesh wall struct.
 */
void
wall_init_2 (Wall * wall,       ///< pointer to the mesh wall struct data.
             Cell * left2,      ///< pointer to the left left cell struct data.
             Cell * right2)
             ///< pointer to the right right cell struct data.
{
#if DEBUG_WALL
  fprintf (stderr, "wall_init_2: start\n");
#endif
  wall->left2 = left2;
  wall->right2 = right2;
#if DEBUG_WALL
  fprintf (stderr, "wall_init_2: end\n");
#endif
}

/**
 * function to set the 2nd order differences of a mesh wall struct for positive
 * velocities.
 */
void
wall_set_2p (Wall * wall,       ///< pointer to the wall struct data.
             Wall * prev)       ///< pointer to the previous wall struct data.
{
  double *dnr, *dsr, *dnl, *dsl, *dn2, *ds2;
  double k;
  unsigned int i;
#if DEBUG_WALL
  fprintf (stderr, "wall_set_2n: start\n");
#endif
  dnl = prev->dn;
  dsl = prev->ds;
  dnr = wall->dn;
  dsr = wall->ds;
  dn2 = wall->dn2;
  ds2 = wall->ds2;
  k = 0.5 * (1. - wall->vdt / wall->size);
  for (i = 0; i < nnutrients; ++i)
    dn2[i] = k * limited_flow (dnl[i], dnr[i]);
  for (i = 0; i < nspecies; ++i)
    ds2[i] = k * limited_flow (dsl[i], dsr[i]);
#if DEBUG_WALL
  fprintf (stderr, "wall_set_2n: end\n");
#endif
}

/**
 * function to set the 2nd order differences of a mesh wall struct.
 */
void
wall_set_2n (Wall * wall,       ///< pointer to the wall struct data.
             Wall * next)       ///< pointer to the next wall struct data.
{
  double *dnr, *dsr, *dnl, *dsl, *dn2, *ds2;
  double k;
  unsigned int i;
#if DEBUG_WALL
  fprintf (stderr, "wall_set_2n: start\n");
#endif
  dnr = next->dn;
  dsr = next->ds;
  dnl = wall->dn;
  dsl = wall->ds;
  dn2 = wall->dn2;
  ds2 = wall->ds2;
  k = 0.5 * (1. + wall->vdt / wall->size);
  for (i = 0; i < nnutrients; ++i)
    dn2[i] = k * limited_flow (dnr[i], dnl[i]);
  for (i = 0; i < nspecies; ++i)
    ds2[i] = k * limited_flow (dsr[i], dsl[i]);
#if DEBUG_WALL
  fprintf (stderr, "wall_set_2n: end\n");
#endif
}

#endif
