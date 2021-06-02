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
  double decay;                 ///< decay coefficient.
  double grow;                  ///< grow coefficient.
  double cling;                 ///< cling coefficient.
  double maximum_velocity;      ///< maximum velocity to cling.
} Species;

extern Species *species;        ///< array of species struct data.
extern unsigned int nspecies;   ///< number of species.

void species_destroy ();
int species_open_xml (char *file_name);

#endif
