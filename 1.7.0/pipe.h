/**
 * \file pipe.h
 * \brief header file to define the network pipes.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#ifndef PIPE__H
#define PIPE__H 1

///> enum to define the dispersion model.
enum DispersionModel
{
  DISPERSION_MODEL_NONE = 0,    ///< no dispersion.
  DISPERSION_MODEL_RUTHERFORD,  ///< Rutherford's dispersion.
};

/**
 * \struct Pipe
 * \brief struct to define an irrigation pipe.
 */
typedef struct
{
  char id[MAX_LABEL_LENGTH];    ///< identifier.
  char inlet_id[MAX_LABEL_LENGTH];      ///< inlet node identifier.
  char outlet_id[MAX_LABEL_LENGTH];     ///< outlet node identifier.
  Point *inlet;                 ///< pointer to the inlet point.
  Point *outlet;                ///< pointer to the outlet point.
  Cell *cell;                   ///< array of node cells.
  Wall *wall;                   ///< array of mesh walls.
  double *C;                    ///< array of C dispersion coefficients.
  double *D;                    ///< array of D dispersion coefficients.
  double *E;                    ///< array of E dispersion coefficients.
  double *H;                    ///< array of H dispersion coefficients.
  double length;                ///< length.
  double diameter;              ///< diameter.
  double radius;                ///< radius.
  double area;                  ///< cross sectional area.
  double perimeter;             ///< cross sectional perimeter.
  double roughness;             ///< roughness length.
  double discharge;             ///< current discharge.
  double velocity;              ///< current flow velocity.
  double friction_factor;       ///< friction factor.
  double dispersion;            ///< dispersion coefficient.
  unsigned int ncells;          ///< number of node cells.
  unsigned int nwalls;          ///< number of mesh walls.
} Pipe;

extern double current_time;
extern double next_time;
extern double advection_step;
extern double dispersion_step;
extern double biological_step;
extern unsigned int dispersion_model;

void pipe_inlet_add_recirculation (Pipe * pipe, double length);
void pipe_outlet_add_recirculation (Pipe * pipe, double length);

/**
 * function to init an empty pipe.
 */
static inline void
pipe_null (Pipe *pipe)          ///< pointer to the pipe struct data.
{
#if DEBUG_PIPE
  fprintf (stderr, "pipe_null: start\n");
#endif
  pipe->cell = NULL;
  pipe->wall = NULL;
  pipe->C = pipe->D = pipe->E = pipe->H = NULL;
  pipe->nwalls = pipe->ncells = 0;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_null: end\n");
#endif
}

/**
 * function free the memory used by a pipe.
 */
static inline void
pipe_destroy (Pipe *pipe)       ///< pointer to the pipe struct data.
{
#if DEBUG_PIPE
  fprintf (stderr, "pipe_destroy: start\n");
#endif
  free (pipe->H);
  free (pipe->E);
  free (pipe->D);
  free (pipe->C);
  free (pipe->wall);
  free (pipe->cell);
#if DEBUG_PIPE
  fprintf (stderr, "pipe_destroy: end\n");
#endif
}

/**
 * function to create a mesh of cells in a pipe.
 */
static inline void
pipe_create_mesh (Pipe *pipe,   ///< pointer to the pipe struct data.
                  double cell_size)     ///< maximum cell size.
{
  Cell *cell;
  Wall *wall;
  double distance, size, area, perimeter;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_create_mesh: start\n");
#endif
  pipe->radius = 0.5 * pipe->diameter;
  pipe->perimeter = perimeter = M_PI * pipe->diameter;
  pipe->area = area = 0.5 * perimeter * pipe->radius;
  pipe->length = fmax (pipe->length, cell_size);
  n = (unsigned int) ceil (pipe->length / cell_size);
  pipe->ncells = n = (2 > n) ? 2 : n;
  pipe->nwalls = --n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_create_mesh: ncells=%u nwallw=%u\n",
           pipe->ncells, pipe->nwalls);
#endif
  distance = pipe->length / n;
  size = 0.5 * distance;
  pipe->cell = cell = (Cell *) malloc (pipe->ncells * sizeof (Cell));
  pipe->wall = wall = (Wall *) malloc (n * sizeof (Wall));
  pipe->C = (double *) malloc (n * sizeof (double));
  pipe->D = (double *) malloc (pipe->ncells * sizeof (double));
  pipe->E = (double *) malloc (n * sizeof (double));
  pipe->H = (double *) malloc (pipe->ncells * sizeof (double));
  // init cells
  cell_init (cell, 0., distance, size, area, perimeter);
  for (i = 1; i < pipe->ncells - 1; ++i)
    cell_init (cell + i, i * pipe->length / n, distance, distance, area,
               perimeter);
  cell_init (cell + i, pipe->length, distance, size, area, perimeter);
  // init walls
  for (i = 0; i < pipe->nwalls; ++i)
    wall_init (wall + i, cell + i, cell + i + 1);
  if (numerical_order > 1)
    for (i = 1; i < pipe->nwalls - 1; ++i)
      wall_init_2 (wall + i, cell + i - 1, cell + i + 2);
#if DEBUG_PIPE
  fprintf (stderr, "pipe_create_mesh: end\n");
#endif
}

/**
 * function to set the flow in a pipe.
 */
static inline void
pipe_set_flow (Pipe *pipe,      ///< pointer to the pipe struct data.
               double discharge,        ///< flow discharge.
               double friction_factor)  ///< friction factor.
{
  Cell *cell;
  double v, d;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_set_flow: start\n");
#endif
  pipe->discharge = discharge;
  pipe->friction_factor = friction_factor;
  pipe->velocity = v = discharge / pipe->area;
  pipe->dispersion = d = fmax (WATER_VISCOSITY,
                               10. * sqrt (0.5 * friction_factor)
                               * pipe->perimeter * fabs (v));
  cell = pipe->cell;
  n = pipe->ncells;
  for (i = 0; i < n; ++i)
    cell_set_flow (cell + i, discharge, v, friction_factor, d);
#if DEBUG_PIPE
  fprintf (stderr, "pipe_set_flow: discharge=%lg velocity=%lg factor=%lg\n",
           discharge, pipe->velocity, pipe->friction_factor);
  fprintf (stderr, "pipe_set_flow: end\n");
#endif
}

/**
 * function to get the cell of a node identifier in a pipe.
 *
 * \return cell pointer on succes, NULL on error.
 */
static inline Cell *
pipe_node_cell (Pipe *pipe,     ///< pointer to the pipe struct data.
                const char *id) ///< node identifier.
{
  Cell *cell = NULL;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_node_cell: start\n");
#endif
  if (!strcmp (id, pipe->inlet_id))
    cell = pipe->cell;
  if (!strcmp (id, pipe->outlet_id))
    cell = pipe->cell + pipe->ncells - 1;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_node_cell: end\n");
#endif
  return cell;
}

/**
 * function to return the time step size allowed by a pipe.
 *
 * \return time step size.
 */
static inline double
pipe_step_size (Pipe *pipe,     ///< pointer to the pipe struct data.
                double cfl)     ///< CFL number.
{
  double dt, v;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_step_size: start\n");
#endif
  v = fabs (pipe->velocity);
  dt = cfl * pipe->cell->size / v;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_step_size: dt=%.14lg v=%lg\n", dt, v);
  fprintf (stderr, "pipe_step_size: end\n");
#endif
  return dt;
}

/**
 * function to set the initial conditions on a pipe.
 */
static inline void
pipe_initial (Pipe *pipe,       ///< pointer to the pipe struct data.
              double *solute_concentration,
              ///< array of solute concetrations.
              double *species_concentration)
              ///< array of species concetrations.
{
  Cell *cell;
  size_t sn, ss;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_initial: start\n");
#endif
  n = pipe->ncells;
  sn = MAX_SOLUTES * sizeof (double);
  ss = MAX_SPECIES * sizeof (double);
  for (i = 0; i < n; ++i)
    {
      cell = pipe->cell + i;
      memcpy (cell->solute_concentration, solute_concentration, sn);
      memcpy (cell->species_concentration, species_concentration, ss);
    }
#if DEBUG_PIPE
  fprintf (stderr, "pipe_initial: end\n");
#endif
}

/**
 * function to perform an advection step on a pipe.
 */
static inline void
pipe_advection_step (Pipe *pipe,        ///< pointer to the pipe struct data.
                     double step)       ///< time step size.
{
  Wall *wall;
  double vdt;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_advection_step: start\n");
#endif
  wall = pipe->wall;
  n = pipe->nwalls;
  vdt = pipe->velocity * step;
  for (i = 0; i < n; ++i)
    wall_set (wall + i, vdt);
  if (vdt > 0.)
    {
      if (numerical_order > 1)
        {
          for (i = 1; i < n - 1; ++i)
            wall_set_2p (wall + i, wall + i - 1);
          wall_increments_p (wall);
          for (i = 1; i < n - 1; ++i)
            {
              wall_increments_p (wall + i);
              wall_increments_2p (wall + i);
            }
          if (i < n)
            wall_increments_p (wall + i);
        }
      else
        for (i = 0; i < n; ++i)
          wall_increments_p (wall + i);
    }
  else if (vdt < 0.)
    {
      if (numerical_order > 1)
        {
          for (i = 1; i < n - 1; ++i)
            wall_set_2n (wall + i, wall + i + 1);
          wall_increments_n (wall);
          for (i = 1; i < n - 1; ++i)
            {
              wall_increments_n (wall + i);
              wall_increments_2n (wall + i);
            }
          if (i < n)
            wall_increments_n (wall + i);
        }
      else
        for (i = 0; i < n; ++i)
          wall_increments_n (wall + i);
    }
#if DEBUG_PIPE
  fprintf (stderr, "pipe_advection_step: end\n");
#endif
}

/**
 * function to calculate a turbulent dispersion step in a pipe.
 */
static inline void
pipe_dispersion_step (Pipe *pipe,       ///< pointer to the pipe struct data.
                      double step)      ///< time step size.
{
  Wall *wall;
  Cell *cell;
  double *C, *D, *E, *H;
  double dx, nudt_dx, k, k2, k3;
  unsigned int i, j;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_dispersion_step: start\n");
#endif
  wall = pipe->wall;
  cell = pipe->cell;
  C = pipe->C;
  D = pipe->D;
  E = pipe->E;
  H = pipe->H;
  dx = wall->size;
  nudt_dx = pipe->dispersion * step / dx;
  k = fmax (0.5 * nudt_dx, nudt_dx - 0.5 * dx);
  k2 = 2 * k;
  k3 = nudt_dx - k;
  k = -k;
  E[0] = 0.;
  for (i = 1; i < pipe->nwalls; ++i)
    E[i] = C[i - 1] = k;
  C[i - 1] = 0.;
  for (j = 0; j < MAX_SOLUTES; ++j)
    {
      D[0] = 1.;
      H[0] = cell[0].solute_concentration[j];
      for (i = 1; i < pipe->nwalls; ++i)
        {
          D[i] = cell[i].size + k2;
          H[i] = cell[i].solute_concentration[j] * cell[i].size
            + k3 * (wall[i].in[j] - wall[i - 1].in[j]);
        }
      D[i] = 1.;
      H[i] = cell[i].solute_concentration[j];
      matrix_tridiagonal_solve (C, D, E, H, pipe->ncells);
      for (i = 1; i < pipe->nwalls; ++i)
        cell[i].solute_concentration[j] = H[i];
    }
  for (j = 0; j < MAX_SPECIES; ++j)
    {
      D[0] = 1.;
      H[0] = cell[0].species_concentration[j];
      for (i = 1; i < pipe->nwalls; ++i)
        {
          D[i] = cell[i].size + k2;
          H[i] = cell[i].species_concentration[j] * cell[i].size
            + k3 * (wall[i].is[j] - wall[i - 1].is[j]);
        }
      D[i] = 1.;
      H[i] = cell[i].species_concentration[j];
      matrix_tridiagonal_solve (C, D, E, H, pipe->ncells);
      for (i = 1; i < pipe->nwalls; ++i)
        cell[i].species_concentration[j] = H[i];
    }
#if DEBUG_PIPE
  fprintf (stderr, "pipe_dispersion_step: end\n");
#endif
}

/**
 * function to calculate a solute decay step in a pipe.
 */
static inline void
pipe_solute_decay_step (Pipe *pipe,     ///< pointer to the pipe struct data.
                        double step)    ///< time step size.
{
  Cell *cell;
  unsigned int i, j, ncells;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_solute_decay_step: start\n");
#endif
  cell = pipe->cell;
  ncells = pipe->ncells;
  for (j = 0; j < MAX_SOLUTES; ++j)
    if (solute[j].time_decay > FLT_EPSILON)
      for (i = 0; i < ncells; ++i)
        cell[i].solute_concentration[j] *=
          fmax (0., 1. - M_LN2 * step / solute[j].time_decay);
#if DEBUG_PIPE
  fprintf (stderr, "pipe_solute_decay_step: end\n");
#endif
}

/**
 * function to perform a numerical step on a pipe.
 */
static inline void
pipe_biological_step (Pipe *pipe,       ///< pointer to the pipe struct data.
                      gsl_rng *rng,     ///< GSL random numbers generator.
                      double step)      ///< time step size.
{
  Cell *cell;
  unsigned int i, n;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_biological_step: start\n");
#endif
  cell = pipe->cell;
  n = pipe->ncells;
  for (i = 0; i < n; ++i)
    {
      cell_cling (cell + i, rng, step);
      cell_grow (cell + i, step);
      cell_death (cell + i, rng, step);
    }
#if DEBUG_PIPE
  fprintf (stderr, "pipe_biological_step: end\n");
#endif
}

/**
 * function to calculate the pipe infestations by species.
 */
static inline void
pipe_infestations (Pipe *pipe)  ///< pointer to the pipe struct data.
{
  Cell *cell;
  unsigned int i, ncells;
#if DEBUG_PIPE
  fprintf (stderr, "pipe_infestations: start\n");
#endif
  ncells = pipe->ncells;
  cell = pipe->cell;
  for (i = 0; i < ncells; ++i)
    cell_infestations (cell + i);
#if DEBUG_PIPE
  fprintf (stderr, "pipe_infestations: end\n");
#endif
}

#endif
