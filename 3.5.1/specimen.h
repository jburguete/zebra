/**
 * \file specimen.h
 * \brief header file to define the specimen.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
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
specimen_init (Specimen *specimen,
               ///< pointer to the specimen struct data.
               Species *species)        ///< pointer to the species struct data.
{
  specimen->species = species;
  specimen->size = specimen->age = 0.;
  ++species->nsettled;
}

/**
 * function to calculate specimen settlement.
 *
 */
static inline void
specimen_settlement (GList **list_specimens,
                ///< pointer ot the list of specimens.
                Species *species,       ///< pointer to the species struct data.
                gsl_rng *rng,   ///< GSL random number generator.
                double *concentration,  ///< specimen larvae concentration.
                double volume,  ///< cell volume.
                double lateral_area,    ///< cell lateral area.
                double velocity,        ///< flow velocity.
                double step,    ///< time step size.
                unsigned int recirculation)     ///< flow recirculation zone.
{
  Specimen *specimen;
  double r, settlement;
  unsigned int n;

#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_settlement: start\n");
#endif

  // check maximum velocity of flow
  if (velocity > species->maximum_velocity && !recirculation)
    goto no_settlement;

  // settlement probability
  settlement = species->settlement * (*concentration) * lateral_area * step;
//printf ("settlement=%lg %lg %lg %lg %lg\n", settlement, species->settlement,
//*concentration, lateral_area, step);
  for (n = 0; (r = gsl_rng_uniform (rng)) < settlement; ++n)
    {
#if DEBUG_SPECIMEN
      fprintf (stderr, "specimen_settlement: specimen-list=%lu\n",
               (size_t) *list_specimens);
#endif
      specimen = (Specimen *) malloc (sizeof (Specimen));
      specimen_init (specimen, species);
      *list_specimens = g_list_prepend (*list_specimens, specimen);
      settlement -= 1.;
#if DEBUG_SPECIMEN
      fprintf (stderr, "specimen_settlement: specimen-list=%lu\n",
               (size_t) *list_specimens);
#endif
    }
  *concentration = fmax (0., *concentration - n / volume);

no_settlement:

#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_settlement: end\n");
#endif

  return;
}

/**
 * function to calculate specimen growing.
 */
static inline void
specimen_grow (Specimen *specimen,
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

  // age
  specimen->age += step;

  // get solute concentrations
  organic_matter = solute + SOLUTE_TYPE_ORGANIC_MATTER;
  oxygen = solute + SOLUTE_TYPE_OXYGEN;

  // get species
  species = specimen->species;

  // check maximum velocity and flow recirculation
  if (velocity > species->maximum_velocity && !recirculation)
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
 * function to calculate specimen death.
 */
static inline int
specimen_death (Specimen *specimen,
                ///< pointer to the specimen struct data.
                gsl_rng *rng,   ///< GSL random numbers generator.
                double step,    ///< time step size.
                double *solute) ///< array of solute concentrations.
{
  Species *species;
  double oxygen, chlorine, hydrogen_peroxide, decay;

#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_death: start\n");
#endif

  // get species and solutes
  species = specimen->species;

  // anoxy
  oxygen = solute[SOLUTE_TYPE_OXYGEN];
#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_death: oxygen=%lg minimum=%lg\n",
           oxygen, species->minimum_oxygen);
#endif
  if (oxygen < species->minimum_oxygen)
    goto death;

  // check juvenile age
  if (specimen->age < species->juvenile_age)
    {

      // larva decay
      decay = species->larva_decay;
      chlorine = solute[SOLUTE_TYPE_CHLORINE] - species->larva_minimum_chlorine;
      if (chlorine > species->larva_minimum_chlorine)
        decay += species->larva_decay_chlorine * chlorine
                 / (species->larva_maximum_chlorine
                    - species->larva_minimum_chlorine);
      hydrogen_peroxide = solute[SOLUTE_TYPE_HYDROGEN_PEROXIDE]
                          - species->larva_minimum_hydrogen_peroxide;
      if (hydrogen_peroxide > species->larva_minimum_hydrogen_peroxide)
        decay += species->larva_decay_hydrogen_peroxide * hydrogen_peroxide
                 / (species->larva_maximum_hydrogen_peroxide
                    - species->larva_minimum_hydrogen_peroxide);
#if DEBUG_SPECIMEN
      fprintf (stderr, "specimen_death: larva_decay=%lg step=%lg\n",
               decay, step);
#endif
     if (gsl_rng_uniform (rng) < decay * step)
       goto death;

      // exit
#if DEBUG_SPECIMEN
      fprintf (stderr, "specimen_death: alive\n");
      fprintf (stderr, "specimen_death: end\n");
#endif
      return 0;
    }

  // adult decay
#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_death: adult_decay=%lg step=%lg\n",
           species->adult_decay, step);
#endif
  decay = species->adult_decay;
  chlorine = solute[SOLUTE_TYPE_CHLORINE] - species->adult_minimum_chlorine;
  if (chlorine > species->adult_minimum_chlorine)
    decay += species->adult_decay_chlorine * chlorine
             / (species->adult_maximum_chlorine
                - species->adult_minimum_chlorine);
  hydrogen_peroxide = solute[SOLUTE_TYPE_HYDROGEN_PEROXIDE]
                      - species->adult_minimum_hydrogen_peroxide;
  if (hydrogen_peroxide > species->adult_minimum_hydrogen_peroxide)
    decay += species->adult_decay_hydrogen_peroxide * hydrogen_peroxide
             / (species->adult_maximum_hydrogen_peroxide
                - species->adult_minimum_hydrogen_peroxide);
  if (gsl_rng_uniform (rng) < decay * step)
    goto death;

  // exit
#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_death: alive\n");
  fprintf (stderr, "specimen_death: end\n");
#endif
  return 0;

death:
  ++species->ndeath;
#if DEBUG_SPECIMEN
  fprintf (stderr, "specimen_death: death\n");
  fprintf (stderr, "specimen_death: end\n");
#endif
  return 1;
}

#endif
