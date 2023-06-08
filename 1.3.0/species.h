/**
 * \file species.h
 * \brief header file to define the species.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#ifndef SPECIES__H
#define SPECIES__H 1

///> enum to define the species types.
enum SpeciesType
{
  SPECIES_TYPE_ZEBRA_MUSSEL = 1,        ///< zebra mussel.
};

/**
 * \struct Species
 * \brief struct to define species.
 */
typedef struct
{
  xmlChar *name;                ///< name.
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
  double decay;                 ///< decay coefficient.
  double minimum_chlorine;      ///< minimum tolerated chlorine concentration.
  double maximum_chlorine;      ///< maximum tolerated chlorine concentration.
  double decay_chlorine;        ///< chlorine decary ratio.
  double minimum_hydrogen_peroxide;
  ///< minimum tolerated hydrogen peroxide concentration.
  double maximum_hydrogen_peroxide;
  ///< maximum tolerated hydrogen peroxide concentration.
  double decay_hydrogen_peroxide;       ///< hydrogen peroxide decary ratio.
  unsigned int type;            ///< type.
} Species;

extern Species *species;
extern GHashTable *hash_species;
extern unsigned int nspecies;
extern unsigned int flags_species;

void species_destroy ();
int species_open_xml (char *file_name);

/**
 * function to get the index of a species.
 *
 * \return species index.
 */
static inline unsigned int
species_index (const char *id)  ///< identifier label.
{
  Species *s;
  size_t i;
#if DEBUG_SPECIES
  fprintf (stderr, "species_index: start\n");
#endif
  s = (Species *) g_hash_table_lookup (hash_species, id);
  i = ((size_t) s - (size_t) species) / sizeof (Species);
#if DEBUG_SPECIES
  fprintf (stderr, "species_index: index=%lu\n", i);
  fprintf (stderr, "species_index: end\n");
#endif
  return (unsigned int) i;
}

#endif
