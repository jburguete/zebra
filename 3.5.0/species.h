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
  double maximum_velocity;      ///< maximum velocity to settlement.
  double maximum_temperature;   ///< maximum temperature to survive.
  double minimum_temperature;   ///< minimum temperature to survive.
  double optimal_temperature;   ///< optimal temperature to survive.
  double settlement;                 ///< settlement pipe coefficient.
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
  unsigned int nsettled;        ///< number of settled larvae.
  unsigned int ndeath;          ///< number of death mussels.
} Species;

extern Species species[MAX_SPECIES];

void species_destroy ();
unsigned int species_index (const xmlChar * name);
int species_open_xml (char *file_name);

static inline void
species_print (Species * species,
               FILE *file)
{
  fprintf (file, "name=%s\n", (char*) species->name);
  fprintf (file, "juvenile_age=%lg\n", species->juvenile_age);
  fprintf (file, "maximum_size=%lg\n", species->maximum_size);
  fprintf (file, "maximum_velocity=%lg\n", species->maximum_velocity);
  fprintf (file, "maximum_temperature=%lg\n", species->maximum_temperature);
  fprintf (file, "minimum_temperature=%lg\n", species->minimum_temperature);
  fprintf (file, "optimal_temperature=%lg\n", species->optimal_temperature);
  fprintf (file, "settlement=%lg\n", species->settlement);
  fprintf (file, "minimum_oxygen=%lg\n", species->minimum_oxygen);
  fprintf (file, "respiration=%lg\n", species->respiration);
  fprintf (file, "eat=%lg\n", species->eat);
  fprintf (file, "grow=%lg\n", species->grow);
  fprintf (file, "larva_decay=%lg\n", species->larva_decay);
  fprintf (file, "larva_minimum_chlorine=%lg\n",
           species->larva_minimum_chlorine);
  fprintf (file, "larva_maximum_chlorine=%lg\n",
           species->larva_maximum_chlorine);
  fprintf (file, "larva_decay_chlorine=%lg\n", species->larva_decay_chlorine);
  fprintf (file, "larva_minimum_hydrogen_peroxide=%lg\n",
           species->larva_minimum_hydrogen_peroxide);
  fprintf (file, "larva_maximum_hydrogen_peroxide=%lg\n",
           species->larva_maximum_hydrogen_peroxide);
  fprintf (file, "larva_decay_hydrogen_peroxide=%lg\n",
           species->larva_decay_hydrogen_peroxide);
  fprintf (file, "adult_decay=%lg\n", species->adult_decay);
  fprintf (file, "adult_minimum_chlorine=%lg\n",
           species->adult_minimum_chlorine);
  fprintf (file, "adult_maximum_chlorine=%lg\n",
           species->adult_maximum_chlorine);
  fprintf (file, "adult_decay_chlorine=%lg\n", species->adult_decay_chlorine);
  fprintf (file, "adult_minimum_hydrogen_peroxide=%lg\n",
           species->adult_minimum_hydrogen_peroxide);
  fprintf (file, "adult_maximum_hydrogen_peroxide=%lg\n",
           species->adult_maximum_hydrogen_peroxide);
  fprintf (file, "adult_decay_hydrogen_peroxide=%lg\n",
           species->adult_decay_hydrogen_peroxide);
  fprintf (file, "type=%u\n", species->type);
}

/**
 * function to calculate the larva decay coefficient of a species.
 *
 * \return larva decay coefficient.
 */
static inline double
species_larva_decay (Species *species,  ///< pointer to the Species struct data.
                     double *solute,    ///< array of solute concentrations.
                     double step)       ///< time step size.
{
  double decay, chlorine, hydrogen_peroxide;
#if DEBUG_SPECIES
  fprintf (stderr, "species_larva_decay: start\n");
#endif
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
  decay = exp (- decay * step);
#if DEBUG_SPECIES
  fprintf (stderr, "species_larva_decay: end\n");
#endif
  return decay;
}

/**
 * function to init some species variables.
 */
static inline void
species_init ()
{
  unsigned int i;
#if DEBUG_SPECIES
  fprintf (stderr, "species_init: start\n");
#endif
  for (i = 0; i < MAX_SPECIES; ++i)
    species[i].nsettled = species[i].ndeath = 0;
#if DEBUG_SPECIES
  fprintf (stderr, "species_init: end\n");
#endif
}

/**
 * function to write the species summary.
 */ 
static inline void
species_summary (FILE * file)   ///< summary file.
{
  unsigned int i;
#if DEBUG_SPECIES
  fprintf (stderr, "species_summary: start\n");
#endif
  for (i = 0; i < MAX_SPECIES; ++i)
    fprintf (file, "Species: %s\nnumber of settled: %u\nnumber of death: %u\n",
             species[i].name, species[i].nsettled, species[i].ndeath);
#if DEBUG_SPECIES
  fprintf (stderr, "species_summary: end\n");
#endif
}
#endif
