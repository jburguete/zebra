/**
 * \file solute.h
 * \brief header file to define the solutes.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#ifndef SOLUTE__H
#define SOLUTE__H 1

///> enum to define the solute types.
enum SoluteType
{
  SOLUTE_TYPE_CHLORINE = 0,     ///< chlorine.
  SOLUTE_TYPE_HYDROGEN_PEROXIDE = 1,    ///< hydrogen peroxide.
  SOLUTE_TYPE_ORGANIC_MATTER = 2,       ///< organic matter.
  SOLUTE_TYPE_OXYGEN = 3,       ///< oxygen.
};

/**
 * \struct Solute
 * \brief struct to define a solute.
 */
typedef struct
{
  xmlChar *name;                ///< name.
  double solubility;            ///< solubility.
  double time_decay;            ///< time decay rate.
  double decay_temperature;     ///< temperature decay rate.
  double decay_surface;         ///< surface decay rate.
  double initial_conditions;    ///< initial conditions concentration.
  unsigned int type;            ///< type.
} Solute;

extern Solute solute[MAX_SOLUTES];

void solute_destroy ();
unsigned int solute_index (const xmlChar * name);
int solute_open_xml (const char *file_name);

#endif
