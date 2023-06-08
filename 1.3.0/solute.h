/**
 * \file solute.h
 * \brief header file to define the solutes.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#ifndef SOLUTE__H
#define SOLUTE__H 1

enum SoluteType
{
  SOLUTE_TYPE_CHLORINE = 1,     ///< chlorine.
  SOLUTE_TYPE_HYDROGEN_PEROXIDE = 1 << 1,       ///< hydrogen peroxide.
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
  double time_decay;            ///< time decay rate.
  double decay_temperature;     ///< temperature decay rate.
  double decay_surface;         ///< surface decay rate.
  double initial_conditions;    ///< initial conditions concentration.
  unsigned int type;            ///< type.
} Solute;

extern Solute *solute;
extern GHashTable *hash_solutes;
extern unsigned int nsolutes;
extern unsigned int flags_solutes;

void solute_destroy ();
int solute_open_xml (const char *file_name);

/**
 * function to get the index of a solute.
 *
 * \return solute index.
 */
static inline unsigned int
solute_index (const char *id)   ///< identifier label.
{
  Solute *s;
  size_t i;
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_index: start\n");
#endif
  s = (Solute *) g_hash_table_lookup (hash_solutes, id);
  i = ((size_t) s - (size_t) solute) / sizeof (Solute);
#if DEBUG_SOLUTE
  fprintf (stderr, "solute_index: index=%lu\n", i);
  fprintf (stderr, "solute_index: end\n");
#endif
  return (unsigned int) i;
}

#endif
