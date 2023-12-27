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
  SPECIES_TYPE_ZEBRA_MUSSEL = 0,        ///< zebra mussel.
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
  double adult_decay;           ///< adult decay coefficient.
  double larva_decay;           ///< larva decay coefficient.
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

extern Species species[MAX_SPECIES];

void species_destroy ();
unsigned int species_index (const xmlChar * name);
int species_open_xml (char *file_name);

#endif
