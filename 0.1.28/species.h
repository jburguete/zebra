/**
 * \file species.h
 * \brief header file to define the species.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef SPECIES__H
#define SPECIES__H 1

///> enum to define the species types.
enum SpeciesType
{
  SPECIES_TYPE_ZEBRA_MUSSEL,    ///< zebra mussel.
};

/**
 * \struct Species
 * \brief struct to define species.
 */
typedef struct
{
  xmlChar *name;                ///< name.
  double maximum_velocity;      ///< maximum velocity to cling.
  double cling_pipe;            ///< cling pipe coefficient.
  double cling_water;           ///< cling water coefficient.
  double minimum_oxygen;        ///< minimum oxygen concentration.
  double respiration;           ///< respiration coefficient.
  double eat;                   ///< eat coefficient.
  double grow;                  ///< grow coefficient.
  double decay;                 ///< decay coefficient.
  unsigned int type;            ///< type.
} Species;

extern Species *species;
extern unsigned int nspecies;
extern double time_step;

void species_destroy ();
int species_open_xml (char *file_name);
unsigned int species_index (const char *name);

/**
 * function to calculate growing of zebra mussel.
 */
static inline void
zebra_mussel_grow (Species * species,   ///< pointer to the species struct data.
                   double *infestation,
                   ///< zebra mussel infestation coefficient.
                   double *concentration,       ///< zebra mussel concentration.
                   double volume,       ///< cell volume.
                   double lateral_area, ///< cell lateral area.
                   double velocity,     ///< flow velocity.
                   int n,       ///< number of nutrients.
                   ...)
{
  va_list list;
  double *oxygen, *organic_matter;
  double m, w, o2, o2d, om, d;

#if DEBUG_SPECIES
  fprintf (stderr, "zebra_mussel_grow: start\n");
#endif

  // check maximum velocity
  if (velocity >= species->maximum_velocity)
    goto no_growing;

  // get oxygen and organic matter concentrations
  va_start (list, n);
  oxygen = va_arg (list, double *);
  organic_matter = va_arg (list, double *);

  // get masses
  o2d = *oxygen - species->minimum_oxygen;
  if (o2d <= 0.)                // anoxy
    goto no_growing;
  m = *infestation * lateral_area;
  w = *concentration * volume;
  o2 = *oxygen * volume;
  om = *organic_matter * volume;
  o2d *= volume;

  // cling
  d = *concentration * lateral_area * time_step;
  m += d * species->cling_pipe;
  w -= d * species->cling_water;

  // respiration
  o2 = fmax (0., o2d - species->respiration * m * time_step);

  // eating
  d = fmax (0., om - species->eat * m * time_step);
  om -= d;
  m += species->grow * d;

  // update variables
  *infestation = m / lateral_area;
  *concentration = w / volume;
  *oxygen = o2 / volume;
  *organic_matter = om / volume;

  // exit
no_growing:
#if DEBUG_SPECIES
  fprintf (stderr, "zebra_mussel_grow: start\n");
#endif
  return;
}

#endif
