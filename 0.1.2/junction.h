/**
 * \file junction.h
 * \brief header file to define the network junctions.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef JUNCTION__H
#define JUNCTION__H 1

/**
 * \struct Junction
 * \brief struct to define the junctions.
 */
typedef struct
{
  Pipe **inlet;                 ///< array of inlet pipe pointers.
  Pipe **outlet;                ///< array of outlet pipe pointers.
  Point *point;                 ///< Point pointer.
  unsigned int ninlets;         ///< number of inlet pipes.
  unsigned int noutlets;        ///< number of outlet pipes.
  unsigned int id;              ///< identifier.
} Junction;

#endif
