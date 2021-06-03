/**
 * \file tools.c
 * \brief source file to define tools.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <time.h>
#include <libintl.h>
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
 * Function to get a floating number, in double format, from a property of a
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
 * \return time in seconds from 1970.
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
