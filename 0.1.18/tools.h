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
unsigned int array_search (double t, double *x, unsigned int n);
double array_interpolate (double t, double *x, double *y, unsigned int n);
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

#endif
