/**
 * \file specimen.h
 * \brief header file to define the specimen.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#ifndef SPECIMEN__H
#define SPECIMEN__H 1

/**
 * \struct Specimen
 * \brief struct to define specimens.
 */
typedef struct
{
  Species *species;             ///< pointer to the species struct data.
  double size;                  ///< size.
  double age;                   ///< age.
} Specimen;

extern double biological_step;

/**
 * function to init a specimen.
 */
static inline void
specimen_init (Specimen * specimen,
               ///< pointer to the specimen struct data.
               Species * species)       ///< pointer to the species struct data.
{
  specimen->species = species;
  specimen->size = specimen->age = 0.;
}

/**
 * function to calculate specimen cling.
 *
 */
static inline void
specimen_cling (GList ** list_specimens,
                ///< pointer ot the list of specimens.
                Species * species,      ///< pointer to the species struct data.
                gsl_rng * rng,  ///< GSL random number generator.
                double *concentration,  ///< specimen larvae concentration.
                double volume,  ///< cell volume.
                double lateral_area,    ///< cell lateral area.
                double velocity,        ///< flow velocity.
                double step,    ///< time step size.
                unsigned int recirculation)     ///< flow recirculation zone.
{
  Specimen *specimen;
  double r, cling;
  unsigned int n;

#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_cling: start\n");
#endif

  // check maximum velocity of flow
  if (velocity > species->maximum_velocity && !recirculation)
    goto no_cling;

  // clinging probability
  cling = species->cling * (*concentration) * lateral_area * step;
  for (n = 0; (r = gsl_rng_uniform (rng)) < cling; ++n)
    {
      specimen = (Specimen *) malloc (sizeof (Specimen));
      specimen_init (specimen, species);
      *list_specimens = g_list_prepend (*list_specimens, specimen);
      cling -= r;
    }
  *concentration = fmax (0., *concentration - n / volume);

no_cling:

#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_cling: end\n");
#endif

  return;
}

/**
 * function to calculate specimen growing.
 */
static inline void
specimen_grow (Specimen * specimen,
               ///< pointer to the specimen struct data.
               double volume,   ///< cell volume.
               double velocity, ///< flow velocity.
               double step,     ///< time step size.
               unsigned int recirculation,      ///< flow recirculation zone.
               double *solute)  ///< array of solute concentrations.
{
  Species *species;
  double *organic_matter, *oxygen;
  double v, o2, o2d, om;

#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_grow: start\n");
#endif

  // get solute concentrations
  organic_matter = solute + SOLUTE_TYPE_ORGANIC_MATTER;
  oxygen = solute + SOLUTE_TYPE_OXYGEN;

  // get species
  species = specimen->species;

  // check maximum velocity and flow recirculation
  if (velocity >= species->maximum_velocity && !recirculation)
    goto no_growing;

  // get masses
  o2d = *oxygen - species->minimum_oxygen;
  if (o2d <= 0.)                // anoxy
    goto no_growing;
  v = specimen->size * step;
  o2 = v * species->respiration;
  om = v * species->eat;

  // update variables
  *oxygen = fmax (o2d, *oxygen - o2 / volume);
  *organic_matter = fmax (0., *organic_matter - om / volume);
  specimen->size += om * species->grow;
  specimen->size = fmax (specimen->size, species->maximum_size);

  // exit
no_growing:
#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_grow: end\n");
#endif
  return;
}

/**
 * function to calculate specimen dead.
 */
static inline int
specimen_dead (Specimen * specimen,
               ///< pointer to the specimen struct data.
               gsl_rng * rng,   ///< GSL random numbers generator.
               double velocity, ///< flow velocity.
               double step,     ///< time step size.
               unsigned int recirculation,      ///< flow recirculation zone.
               double *solute)  ///< array of solute concentrations.
{
  Species *species;
  double *chlorine, *hydrogen_peroxide, *oxygen;

#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_dead: start\n");
#endif

  // get species
  species = specimen->species;

  // check maximum velocity and flow recirculation
  if (velocity > species->maximum_velocity && !recirculation)
    goto dead;

  // check solute limitants 
  chlorine = solute + SOLUTE_TYPE_CHLORINE;
  if (*chlorine > species->maximum_chlorine)
    goto dead;
  hydrogen_peroxide = solute + SOLUTE_TYPE_HYDROGEN_PEROXIDE;
  if (*hydrogen_peroxide > species->maximum_hydrogen_peroxide)
    goto dead;
  oxygen = solute + SOLUTE_TYPE_OXYGEN;
  if (*oxygen < species->minimum_oxygen)
    goto dead;

  // decay
  if (gsl_rng_uniform (rng) < species->decay * step)
    goto dead;

  // exit
#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_dead: end\n");
#endif
  return 0;

dead:
#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_dead: end\n");
#endif
  return 1;
}

#endif
