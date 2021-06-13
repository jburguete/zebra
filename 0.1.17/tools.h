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
double distance (double x1, double y1, double z1, double x2, double y2,
                 double z2);

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
