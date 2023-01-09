/**
 * \file solute.h
 * \brief header file to define the solutes.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2022, Javier Burguete Tolosa.
 */
#ifndef SOLUTE__H
#define SOLUTE__H 1

///> enum to define the solute type.
enum SoluteType
{
  SOLUTE_TYPE_CHLORINE = 1,     ///< chlorine.
  SOLUTE_TYPE_HYDROGEN_PEROXIDE = 1 << 1,     ///< hydrogen peroxide.
  SOLUTE_TYPE_ORGANIC_MATTER = 1 << 2,  ///< organic matter.
  SOLUTE_TYPE_OXYGEN = 1 << 3,  ///< oxygen.
};

/**
 * \struct Solute
 * \brief struct to define a solute.
 */
typedef struct
{
  xmlChar *name;                ///< name.
  double decay;                 ///< decay rate.
  unsigned int type;            ///< type.
} Solute;

extern Solute *solute;
extern unsigned int nsolutes;

void solute_destroy ();
int solute_open_xml (char *file_name);
unsigned int solute_index (const char *name);

#endif
