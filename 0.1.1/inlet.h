/**
 * \file inlet.h
 * \brief header file to define the inlets.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef INLET__H
#define INLET__H 1

/**
 * \struct Inlet
 * \brief struct to define the inlets.
 */
typedef struct
{
  double **nutrient_concentration;
  ///< array of pointers to array of nutrient concentrations.
  double **nutrient_time;
  ///< array of pointers to array of nutrient times.
  double **species_concentration;
  ///< array of pointers to array of species concentrations.
  double **species_time;
  ///< array of pointers to array of species times.
  unsigned int node;            ///< node identifier.
} Inlet;

extern Inlet *inlet;
extern unsigned int ninlets;

void inlet_destroy ();
int inlet_open_xml (char *file_name);

#endif
