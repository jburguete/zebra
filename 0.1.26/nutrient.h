/**
 * \file nutrient.h
 * \brief header file to define the nutrients.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef NUTRIENT__H
#define NUTRIENT__H 1

///> enum to define the nutrient type.
enum NutrientType
{
  NUTRIENT_TYPE_OXYGEN,         ///< oxygen.
  NUTRIENT_TYPE_ORGANIC_MATTER, ///< organic matter.
};

/**
 * \struct Nutrient
 * \brief struct to define a nutrient.
 */
typedef struct
{
  xmlChar *name;                ///< name.
  unsigned int type;            ///< type.
} Nutrient;

extern Nutrient *nutrient;
extern unsigned int nnutrients;

void nutrient_destroy ();
int nutrient_open_xml (char *file_name);
unsigned int nutrient_index (const char *name);

#endif
