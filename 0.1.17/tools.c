/**
 * \file tools.c
 * \brief source file to define tools.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <time.h>
#include <libintl.h>
#include <math.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"

char *error_msg = NULL;         ///< error message string.

/**
 * function to set an error message.
 */
void
error_message (const char *label,       ///< error label.
               char *msg)       ///< error message.
{
  g_free (error_msg);
  error_msg = (char *) g_strconcat (label, ":\n", msg, NULL);
  g_free (msg);
}

/**
 * function to get the interval index of a value on an array.
 *
 * \return array interval index,
 */
unsigned int
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
double
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

/**
 * function to get a time in format "year month day hour minute seconds" since a
 * file.
 *
 * \return time in seconds since 1970.
 */
double
read_time (FILE * file,         ///< XML node struct.
           int *error)          ///< error code (1 on success, 0 on error).
{
  struct tm t[1];
  double sec = 0.;
  time_t tt;
  int e;
  *error = 0;
  e = fscanf (file, "%d%d%d%d%d%lg", &t->tm_year, &t->tm_mon, &t->tm_mday,
              &t->tm_hour, &t->tm_min, &sec);
  if (e == 6)
    {
      t->tm_year -= 1900;
      --t->tm_mon;
      t->tm_sec = 0;
      t->tm_isdst = 0;
      tt = mktime (t);
      if (tt != -1)
        {
          *error = 1;
          sec += tt;
        }
    }
  return sec;
}

/**
 * function to get an unsigned integer number from a property of a XML node.
 *
 * \return unsigned integer number value.
 */
unsigned int
xml_node_get_uint (xmlNode * node,      ///< XML node struct.
                   const xmlChar * prop,        ///< XML node property.
                   int *error)  ///< error code (1 on success, 0 on error).
{
  char *buffer;
  unsigned int x = 0;
  *error = 0;
  buffer = (char *) xmlGetProp (node, prop);
  if (buffer)
    {
      *error = sscanf (buffer, "%u", &x);
      xmlFree (buffer);
    }
  return x;
}

/**
 * function to get an unsigned integer number from a property of a XML node or a
 * default value if the node has not the property.
 *
 * \return unsigned integer number value.
 */
unsigned int
xml_node_get_uint_with_default (xmlNode * node, ///< XML node struct.
                                const xmlChar * prop,   ///< XML node property.
                                int *error,
                                ///< error code (1 on success, 0 on error).
                                unsigned int def)       ///< default value.
{
  if (!xmlHasProp (node, prop))
    {
      *error = 1;
      return def;
    }
  return xml_node_get_uint (node, prop, error);
}

/**
 * function to get a floating number, in double format, from a property of a
 * XML node.
 *
 * \return floating number value in double format.
 */
double
xml_node_get_float (xmlNode * node,     ///< XML node struct.
                    const xmlChar * prop,       ///< XML node property.
                    int *error) ///< error code (1 on success, 0 on error).
{
  char *buffer;
  double x = 0.;
  *error = 0;
  buffer = (char *) xmlGetProp (node, prop);
  if (buffer)
    {
      *error = sscanf (buffer, "%lf", &x);
      xmlFree (buffer);
    }
  return x;
}

/**
 * function to get a floating number, in double format, from a property of a
 * XML node or a default value if the node has not the property.
 *
 * \return floating number value in double format.
 */
double
xml_node_get_float_with_default (xmlNode * node,        ///< XML node struct.
                                 const xmlChar * prop,  ///< XML node property.
                                 int *error,
                                 ///< error code (1 on success, 0 on error).
                                 double def)    ///< default value.
{
  if (!xmlHasProp (node, prop))
    {
      *error = 1;
      return def;
    }
  return xml_node_get_float (node, prop, error);
}

/**
 * function to get a time in format "year month day hour minute seconds" from a
 * property of a XML node.
 *
 * \return time in seconds since 1970.
 */
double
xml_node_get_time (xmlNode * node,      ///< XML node struct.
                   const xmlChar * prop,        ///< XML node property.
                   int *error)  ///< error code (1 on success, 0 on error).
{
  struct tm t[1];
  char *buffer;
  double sec = 0.;
  time_t tt;
  *error = 0;
  buffer = (char *) xmlGetProp (node, prop);
  if (buffer)
    {
      *error = sscanf (buffer, "%d%d%d%d%d%lg", &t->tm_year, &t->tm_mon,
                       &t->tm_mday, &t->tm_hour, &t->tm_min, &sec);
      xmlFree (buffer);
      if (*error == 6)
        {
          t->tm_year -= 1900;
          --t->tm_mon;
          t->tm_sec = 0;
          t->tm_isdst = 0;
          tt = mktime (t);
          if (tt == -1)
            *error = 0;
          else
            {
              *error = 1;
              sec += tt;
            }
        }
      else
        *error = 0;
    }
  return sec;
}

/**
 * function to calculate the distance between 2 points.
 *
 * \return distance.
 */
double
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
