/**
 * \file point.h
 * \brief header file to define the network node points.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#ifndef POINT__H
#define POINT__H 1

/**
 * \struct Point
 * \brief struct to define a pipe point.
 */
typedef struct
{
  char id[MAX_LABEL_LENGTH];    ///< identifier.
  double x;                     ///< x-coordinate.
  double y;                     ///< y-coordinate.
  double z;                     ///< z-coordinate.
} Point;

#endif
