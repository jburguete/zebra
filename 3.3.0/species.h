/**
 * \file species.h
 * \brief header file to define the species.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
 */
#ifndef SPECIES__H
#define SPECIES__H 1

///> enum to define the species types.
enum SpeciesType
{
  SPECIES_TYPE_ZEBRA_MUSSEL = 0,        ///< zebra mussel.
};

/**
 * \struct Species
 * \brief struct to define species.
 */
typedef struct
{
  xmlChar *name;                ///< name.
  double juvenile_age;          ///< age to become juvenile.
  double maximum_size;          ///< maximum size.
  double maximum_velocity;      ///< maximum velocity to cling.
  double maximum_temperature;   ///< maximum temperature to survive.
  double minimum_temperature;   ///< minimum temperature to survive.
  double optimal_temperature;   ///< optimal temperature to survive.
  double cling;                 ///< cling pipe coefficient.
  double minimum_oxygen;        ///< minimum oxygen concentration.
  double respiration;           ///< respiration coefficient.
  double eat;                   ///< eat coefficient.
  double grow;                  ///< grow coefficient.
  double larva_decay;           ///< larva decay coefficient.
  double larva_minimum_chlorine;
  ///< minimum chlorine concentration tolerated by larvae.
  double larva_maximum_chlorine;
  ///< maximum chlorine concentration tolerated by larvae.
  double larva_decay_chlorine;  ///< chlorine decary ratio of larvae.
  double larva_minimum_hydrogen_peroxide;
  ///< minimum hydrogen_peroxide concentration tolerated by larvae.
  double larva_maximum_hydrogen_peroxide;
  ///< maximum hydrogen_peroxide concentration tolerated by larvae.
  double larva_decay_hydrogen_peroxide;
  ///< hydrogen_peroxide decary ratio of larvae.
  double adult_decay;           ///< adult decay coefficient.
  double adult_minimum_chlorine;
  ///< minimum chlorine concentration tolerated by adults.
  double adult_maximum_chlorine;
  ///< maximum chlorine concentration tolerated by adults.
  double adult_decay_chlorine;  ///< chlorine decary ratio of adults.
  double adult_minimum_hydrogen_peroxide;
  ///< minimum hydrogen_peroxide concentration tolerated by adults.
  double adult_maximum_hydrogen_peroxide;
  ///< maximum hydrogen_peroxide concentration tolerated by adults.
  double adult_decay_hydrogen_peroxide;
  unsigned int type;            ///< type.
} Species;

extern Species species[MAX_SPECIES];

void species_destroy ();
unsigned int species_index (const xmlChar * name);
int species_open_xml (char *file_name);

/**
 * function to calculate the larva decay coefficient of a species.
 *
 * \return larva decay coefficient.
 */
static inline double
species_larva_decay (Species *species,  ///
                     double *solute)    ///< array of solute concentrations.
{
  double decay, chlorine, hydrogen_peroxide;
#if DEBUG_SPECIES
  fprintf (stderr, "species_larva_decay: start\n");
#endif
  decay = species->larva_decay;
  chlorine = solute[SOLUTE_TYPE_CHLORINE] - species->larva_minimum_chlorine;
  if (chlorine > 0.)
    decay += species->larva_decay_chlorine
           * exp (8.2 * chlorine / (species->larva_maximum_chlorine
                                    - species->larva_minimum_chlorine));
  hydrogen_peroxide = solute[SOLUTE_TYPE_HYDROGEN_PEROXIDE]
                      - species->larva_minimum_hydrogen_peroxide;
  if (hydrogen_peroxide > 0.)
    decay += species->larva_decay_hydrogen_peroxide
           * exp (8.2 * hydrogen_peroxide
                  / (species->larva_maximum_hydrogen_peroxide
                     - species->larva_minimum_hydrogen_peroxide));
#if DEBUG_SPECIES
  fprintf (stderr, "species_larva_decay: end\n");
#endif
  return decay;
}

#endif
