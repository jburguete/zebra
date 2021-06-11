/**
 * \file nutrient.h
 * \brief header file to define the nutrients.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef NUTRIENT__H
#define NUTRIENT__H 1

/**
 * \struct Nutrient
 * \brief struct to define a nutrient.
 */
typedef struct
{
  const char *name;             ///< name.
} Nutrient;

extern Nutrient *nutrient;
extern unsigned int nnutrients;

void nutrient_destroy ();
int nutrient_open_xml (char *file_name);
unsigned int nutrient_index (const char *name);

#endif
