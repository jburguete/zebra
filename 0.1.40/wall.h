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
  double in[MAX_NUTRIENTS];
  ///< array of nutrient concentration differences.
  double is[MAX_SPECIES];
  ///< array of species concentration differences.
  double dn[MAX_NUTRIENTS];
  ///< array of nutrient concentration advection differences.
  double ds[MAX_SPECIES];
  ///< array of species concentration advection differences.
  double dn2[MAX_NUTRIENTS];
  ///< array of 2nd order nutrient concentration advection differences.
  double ds2[MAX_SPECIES];
  ///< array of 2nd order species concentration advection differences.
  Cell *left2;                  ///< left cell.
  Cell *right2;                 ///< right cell.
  Cell *left;                   ///< left cell.
  Cell *right;                  ///< right cell.
  double size;                  ///< longitudinal size.
  double vdt;                   ///< velocity times time step size.
} Wall;

extern unsigned int numerical_order;

/**
 * function to init the data of a mesh wall struct.
 */
static inline void
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
 * function to set the 1st order differences of a mesh wall struct.
 */
static inline void
wall_set (Wall * wall,          ///< pointer to the mesh wall struct data.
          double vdt)           ///< flow velocity times time step size.
{
  Cell *left, *right;
  double *n1, *s1, *n2, *s2, *dn, *ds, *in, *is;
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
  in = wall->in;
  is = wall->is;
  dn = wall->dn;
  ds = wall->ds;
  wall->vdt = vdt;
  for (i = 0; i < nnutrients; ++i)
    {
      in[i] = n2[i] - n1[i];
      dn[i] = vdt * in[i];
    }
  for (i = 0; i < nspecies; ++i)
    {
      is[i] = s2[i] - s1[i];
      ds[i] = vdt * is[i];
    }
#if DEBUG_WALL
  fprintf (stderr, "wall_set: vdt=%lg\n", vdt);
  for (i = 0; i < nnutrients; ++i)
    fprintf (stderr, "wall_set: i=%u n2=%lg n1=%lg\n", i, n2[i], n1[i]);
  for (i = 0; i < nspecies; ++i)
    fprintf (stderr, "wall_set: i=%u s2=%lg s1=%lg\n", i, s2[i], s1[i]);
  for (i = 0; i < nnutrients; ++i)
    fprintf (stderr, "wall_set: i=%u in=%lg dn=%lg\n", i, in[i], dn[i]);
  for (i = 0; i < nspecies; ++i)
    fprintf (stderr, "wall_set: i=%u is=%lg ds=%lg\n", i, is[i], ds[i]);
  fprintf (stderr, "wall_set: end\n");
#endif
}

/**
 * function to calculate 1st order variable increments on a wall for positive
 * velocities.
 */
static inline void
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
static inline void
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

/**
 * function to init the 2nd order data of a mesh wall struct.
 */
static inline void
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
static inline void
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
    dn2[i] = k * flux_limited (dnl[i], dnr[i]);
  for (i = 0; i < nspecies; ++i)
    ds2[i] = k * flux_limited (dsl[i], dsr[i]);
#if DEBUG_WALL
  fprintf (stderr, "wall_set_2n: end\n");
#endif
}

/**
 * function to set the 2nd order differences of a mesh wall struct.
 */
static inline void
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
    dn2[i] = k * flux_limited (dnr[i], dnl[i]);
  for (i = 0; i < nspecies; ++i)
    ds2[i] = k * flux_limited (dsr[i], dsl[i]);
#if DEBUG_WALL
  fprintf (stderr, "wall_set_2n: end\n");
#endif
}

/**
 * function to calculate 2nd order variable increments on a wall for positive
 * velocities.
 */
static inline void
wall_increments_2p (Wall * wall)
{
  Cell *left, *right;
  double *n, *s, *n2, *s2, *dn2, *ds2;
  double dx, dx2;
  unsigned int i;
#if DEBUG_WALL
  fprintf (stderr, "wall_increments_2p: start\n");
#endif
  left = wall->left;
  right = wall->right;
  n = left->nutrient_concentration;
  s = left->species_concentration;
  n2 = right->nutrient_concentration;
  s2 = right->species_concentration;
  dn2 = wall->dn2;
  ds2 = wall->ds2;
  dx = left->size;
  dx2 = right->size;
  for (i = 0; i < nnutrients; ++i)
    {
      n[i] -= dn2[i] / dx;
      n2[i] += dn2[i] / dx2;
    }
  for (i = 0; i < nspecies; ++i)
    {
      s[i] -= ds2[i] / dx;
      s2[i] += ds2[i] / dx2;
    }
#if DEBUG_WALL
  fprintf (stderr, "wall_increments_2p: end\n");
#endif
}

/**
 * function to calculate 2nd order variable increments on a wall for negative
 * velocities.
 */
static inline void
wall_increments_2n (Wall * wall)
{
  Cell *left, *right;
  double *n, *s, *n2, *s2, *dn2, *ds2;
  double dx, dx2;
  unsigned int i;
#if DEBUG_WALL
  fprintf (stderr, "wall_increments_2n: start\n");
#endif
  left = wall->left;
  right = wall->right;
  n = left->nutrient_concentration;
  s = left->species_concentration;
  n2 = right->nutrient_concentration;
  s2 = right->species_concentration;
  dn2 = wall->dn2;
  ds2 = wall->ds2;
  dx = left->size;
  dx2 = right->size;
  for (i = 0; i < nnutrients; ++i)
    {
      n[i] -= dn2[i] / dx;
      n2[i] += dn2[i] / dx2;
    }
  for (i = 0; i < nspecies; ++i)
    {
      s[i] -= ds2[i] / dx;
      s2[i] += ds2[i] / dx2;
    }
#if DEBUG_WALL
  fprintf (stderr, "wall_increments_2n: end\n");
#endif
}

#endif
