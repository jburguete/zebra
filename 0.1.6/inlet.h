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
  Cell **cell;                  ///< array of pointers to cells.
  double **nutrient_concentration;
  ///< array of pointers to array of nutrient concentrations.
  double **nutrient_time;
  ///< array of pointers to array of nutrient times.
  double **species_concentration;
  ///< array of pointers to array of species concentrations.
  double **species_time;
  ///< array of pointers to array of species times.
  unsigned int *nnutrient_times;        ///< array of numbers of nutrient times.
  unsigned int *nspecies_times; ///< array of numbers of species times.
  unsigned int id;              ///< node identifier.
  unsigned int ncells;          ///< number of cells.
} Inlet;

void inlet_destroy (Inlet * inlet);
int inlet_open_xml (Inlet * inlet, xmlNode * node, Pipe * pipe,
                    unsigned int npipes);

#endif
