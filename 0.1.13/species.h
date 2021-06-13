/**
 * \file species.h
 * \brief header file to define the species.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef SPECIES__H
#define SPECIES__H 1

/**
 * \struct Species
 * \brief struct to define species.
 */
typedef struct
{
  const char *name;             ///< name.
  double cling;                 ///< cling coefficient.
  double eat;                   ///< eat coefficient.
  double grow;                  ///< grow coefficient.
  double decay;                 ///< decay coefficient.
  double maximum_velocity;      ///< maximum velocity to cling.
} Species;

extern Species *species;
extern unsigned int nspecies;

void species_destroy ();
int species_open_xml (char *file_name);
unsigned int species_index (const char *name);

#endif
