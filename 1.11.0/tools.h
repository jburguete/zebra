/**
 * \file tools.h
 * \brief header file to define useful tools.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2024, Javier Burguete Tolosa.
 */
#ifndef TOOLS__H
#define TOOLS__H 1

#define _(string) (gettext(string))     ///< gettext abbreviation.

extern char *error_msg;

void error_message (const char *label, const char *name, char *msg);
void time_string (char *string, unsigned int length, double date);
double read_time (FILE * file, int *error);
int read_file (const char *name, double **data, double **date, unsigned int *n);
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
              double centred)   ///< centred flux.
{
  double r;
#if FLUX_LIMITER == 0
  return 0;
#elif FLUX_LIMITER == 1
  return upwind;
#elif FLUX_LIMITER == 2
  return centred;
#else
  if (upwind * centred <= 0.)
    return 0.;
  r = centred / upwind;
  if (r <= 1. / 3.)
    return centred + centred;
  if (r >= 3.)
    return upwind + upwind;
  return 0.5 * (upwind + centred);
#endif
}

/**
 * function to get the interval index of a value on an array.
 *
 * \return array interval index.
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
 * function to search the interval where a number is in an array.
 * \return interval number, -1 if x<a[0] or n-1 if x>a[n-1].
 */
static inline int
array_search_extended (double x,        ///< number to search.
                       double *a,       ///< array.
                       int n)   ///< number of elements of the array.
{
  int i;
  if (x < a[0])
    i = -1;
  else if (x >= a[n - 1])
    i = n - 1;
  else
    i = array_search (x, a, n);
  return i;
}

/**
 * function to get the interpolated y-coordinate on an array.
 *
 * \return interpolated y-coordinate.
 */
static inline double
array_interpolate (double t,    ///< x-coordinate to interpolate.
                   double *restrict x,  ///< array of x-coordinates.
                   double *restrict y,  ///< array of y-coordinates.
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

/**
 * Function to integrate a tabular function in an interval.
 *
 * \return integral value.
 */
static inline double
array_integrate (double *restrict x,
                 ///< array defining the x-coordinates.
                 double *restrict y,
                 ///< array defining the y-coordinates.
                 int n,         ///< number of elements of the arrays.
                 double x1,
                 ///< left limit of the integration interval.
                 double x2)
                 ///< right limit of the integration interval.
{
  double *yy, *xx;
  double I, y1;
  int i;
  if (n == 1)
    {
      I = y[0] * (x2 - x1);
      goto exit1;
    }
  i = array_search_extended (x1, x, n);
  if (i < 0)
    {
      if (x2 <= x[0])
        {
          I = y[0] * (x2 - x1);
          goto exit1;
        }
      I = y[0] * (x[0] - x1);
      i = 0;
      x1 = x[0];
      y1 = y[0];
      xx = x;
      yy = y;
    }
  else if (i == n - 1)
    {
      I = y[i] * (x2 - x1);
      goto exit1;
    }
  else
    {
      I = 0.;
      xx = x + i;
      yy = y + i;
      y1 = yy[0] + (x1 - xx[0]) * (yy[1] - yy[0]) / (xx[1] - xx[0]);
    }
  if (x2 < xx[1])
    {
      I += 0.5 * (y1 + yy[0] + (x2 - xx[0]) * (yy[1] - yy[0]) / (xx[1] - xx[0]))
        * (x2 - x1);
      goto exit1;
    }
  I += 0.5 * (y1 + yy[1]) * (xx[1] - x1);
  if (++i == n - 1)
    {
      I += yy[1] * (x2 - xx[1]);
      goto exit1;
    }
  while (++i < n - 1 && x2 > xx[2])
    {
      ++xx, ++yy;
      I += 0.5 * (yy[0] + yy[1]) * (xx[1] - xx[0]);
    }
  if (i == n - 1)
    I += yy[2] * (x2 - xx[1]);
  else if (x2 < xx[2])
    I += yy[1] + 0.5 * (x2 - xx[1]) * (yy[2] - yy[1]) / (xx[2] - xx[1])
      * (x2 - xx[1]);
exit1:
  return I;
}

/**
 * Function to solve a linear equations system stored in a tridiagonal matrix
 * with format: \f$\left(\begin{array}{cccc|c}
 * D_0 & E_0    &         &         & H_0\\
 * C_0 & D_1    & E_1     &         & H_1\\
 *     & \ddots & \ddots  & \ddots  & \vdots\\
 *     &        & C_{n-2} & D_{n-1} & H_{n-1}
 * \end{array}\right)\f$.
 * Results are stored in the H array. It modifies D and H arrays (JBFLOAT).
 */
static inline void
matrix_tridiagonal_solve (double *restrict C,   ///< left diagonal array.
                          double *restrict D,   ///< central diagonal array.
                          double *restrict E,   ///< right diagonal array.
                          double *restrict H,   ///< final column array.
                          unsigned int n)       ///< number of matrix rows.
{
  register double k;
  register unsigned int i;
  --n;
  for (i = 0; i < n; ++i)
    {
      k = C[i] / D[i];
      D[i + 1] -= k * E[i];
      H[i + 1] -= k * H[i];
    }
  H[i] /= D[i];
  while (i--)
    H[i] = (H[i] - E[i] * H[i + 1]) / D[i];
}

#endif
