/**
 * \file tools.c
 * \brief source file to define tools.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"

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
  double x = 0.L;
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
