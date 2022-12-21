/**
 * \file specimen.h
 * \brief header file to define the specimen.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef SPECIES__H
#define SPECIES__H 1

/**
 * \struct Specimen
 * \brief struct to define specimens.
 */
typedef struct
{
  Species *species;             ///< species.
  double size;                  ///< size.
  double age;                   ///< age.
  unsigned int type;            ///< type.
} Specimen;

#endif
