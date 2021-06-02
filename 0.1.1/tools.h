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

unsigned int xml_node_get_uint (xmlNode * node, const xmlChar * prop,
                                int *error);
double xml_node_get_float (xmlNode * node, const xmlChar * prop, int *error);
double xml_node_get_float_with_default (xmlNode * node, const xmlChar * prop,
                                        int *error, double def);

#endif
