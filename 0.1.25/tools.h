/**
 * \file tools.h
 * \brief header file to define useful tools.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#ifndef TOOLS__H
#define TOOLS__H 1

#define _(string) (gettext(string))     ///< gettext abbreviation.

extern char *error_msg;

void error_message (const char *label, char *msg);
void time_string (char *string, unsigned int length, double date);
double read_time (FILE * file, int *error);
unsigned int xml_node_get_uint (xmlNode * node, const xmlChar * prop,
                                int *error);
unsigned int xml_node_get_uint_with_default (xmlNode * node,
                                             const xmlChar * prop, int *error,
                                             unsigned int def);
double xml_node_get_float (xmlNode * node, const xmlChar * prop, int *error);
double xml_node_get_float_with_default (xmlNode * node, const xmlChar * prop,
                                        int *error, double def);
double xml_node_get_time (xmlNode * node, const xmlChar * prop, int *error);


/**
 * function to calculate the distance between 2 points.
 *
 * \return distance.
 */
static inline double
distance (double x1,            ///< x-coordinate from 1st point.
          double y1,            ///< y-coordinate from 1st point.
          double z1,            ///< z-coordinate from 1st point.
          double x2,            ///< x-coordinate from 2nd point.
          double y2,            ///< y-coordinate from 2nd point.
          double z2)            ///< z-coordinate from 2nd point.
{
  x2 -= x1;
  y2 -= y1;
  z2 -= z1;
  return sqrt (x2 * x2 + y2 * y2 + z2 * z2);
}

/**
 * function to limit 2nd order terms.
 *
 * \return limited flux.
 */
static inline double
flux_limited (double upwind,    ///< upwind flux.
              double centered)  ///< centered flux.
{
  double r;
  if (upwind * centered <= 0.)
    return 0.;
  r = centered / upwind;
  if (r <= 1. / 3.)
    return centered + centered;
  if (r >= 3.)
    return upwind + upwind;
  return 0.5 * (upwind + centered);
}

/**
 * function to get the interval index of a value on an array.
 *
 * \return array interval index,
 */
static inline unsigned int
array_search (double t,         ///< value to find.
              double *x,        ///< array.
              unsigned int n)   ///< number of array elements.
{
  unsigned int i, i1, i2;
  i1 = 1;
  i2 = n - 1;
  if (!i2 || t < x[i1])
    return 0;
  --i2;
  if (t >= x[i2])
    return i2;
  while (i2 - i1 > 1)
    {
      i = (i2 + i1) / 2;
      if (t > x[i])
        i1 = i;
      else
        i2 = i;
    }
  return i1;
}

/**
 * function to get the interpolated y-coordinate on an array.
 *
 * \return interpolated y-coordinate.
 */
static inline double
array_interpolate (double t,    ///< x-coordinate to interpolate.
                   double *x,   ///< array of x-coordinates.
                   double *y,   ///< array of y-coordinates.
                   unsigned int n)      ///< number of array elements.
{
  unsigned int i;
  if (t <= x[0])
    return y[0];
  i = n - 1;
  if (t >= x[i])
    return y[i];
  i = array_search (t, x, n);
  return y[i] + (y[i + 1] - y[i]) * (t - x[i]) / (x[i + 1] - x[i]);
}

#endif
