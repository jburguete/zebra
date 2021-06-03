/**
 * \file outlet.h
 * \brief header file to define the outlets.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef OUTLET__H
#define OUTLET__H 1

/**
 * \struct Outlet
 * \brief struct to define the outlets.
 */
typedef struct
{
  double *discharge;            ///< array of discharges.
  double *date;                 ///< array of times.
  unsigned int ndates;          ///< number of times.
  unsigned int id;              ///< identifier.
} Outlet;

void outlet_destroy (Outlet * outlet);
int outlet_open_xml (Outlet * outlet, xmlNode * node);

#endif
