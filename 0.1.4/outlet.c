/**
 * \file outlet.c
 * \brief source file to define the outlets.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <libintl.h>
#include <libxml/parser.h>
#include <glib.h>
#include "config.h"
#include "tools.h"
#include "nutrient.h"
#include "species.h"
#include "outlet.h"

/**
 * function to set an error message opening an outlets input file.
 */
static inline void
outlet_error (const char *msg)  ///< error message.
{
  error_message (_("Outlet"), (char *) g_strdup (msg));
}

/**
 * function to init an empty outlet.
 */
static inline void
outlet_null (Outlet * outlet)   ///< pointer to the outlet struct data.
{
#if DEBUG_OUTLET
  fprintf (stderr, "outlet_null: start\n");
#endif
  outlet->discharge = outlet->date = NULL;
  outlet->ndates = 0;
#if DEBUG_OUTLET
  fprintf (stderr, "outlet_null: end\n");
#endif
}

/**
 * function to free the memory used by the outlets data.
 */
void
outlet_destroy (Outlet * outlet)        ///< pointer to the outlet struct data.
{
#if DEBUG_OUTLET
  fprintf (stderr, "outlet_destroy: start\n");
#endif
  free (outlet->discharge);
  free (outlet->date);
#if DEBUG_OUTLET
  fprintf (stderr, "outlet_destroy: end\n");
#endif
}

/**
 * function to read the outlet data on a XML node.
 *
 * \return 1 on succes, 0 on error.
 */
int
outlet_open_xml (Outlet * outlet,       ///< pointer to the outlet struct data.
                 xmlNode * node)        ///< XML node.
{
  const char *m;
  int e;
  unsigned int n;

  // start
#if DEBUG_OUTLET
  fprintf (stderr, "outlet_open_xml: start\n");
#endif
  outlet_null (outlet);

  // reading outlet
  for (node = node->children; node; node = node->next)
    {
      if (xmlStrcmp (node->name, XML_OUTLET))
        {
          m = _("Bad XML node");
          goto exit_on_error;
        }
      n = outlet->ndates;
      ++outlet->ndates;
      outlet->discharge
        = (double *) realloc (outlet->discharge,
                              outlet->ndates * sizeof (double));
      outlet->date
        = (double *) realloc (outlet->date, outlet->ndates * sizeof (double));
      outlet->discharge[n] = xml_node_get_float (node, XML_DISCHARGE, &e);
      if (!e)
        {
          m = _("Bad discharge");
          goto exit_on_error;
        }
      outlet->date[n] = xml_node_get_time (node, XML_TIME, &e);
      if (!e)
        {
          m = _("Bad discharge");
          goto exit_on_error;
        }
    }

  // exit on success
#if DEBUG_OUTLET
  fprintf (stderr, "outlet_open_xml: end\n");
#endif
  return 1;

  // exit on error
exit_on_error:

  // set error message
  outlet_error (m);

  // free memory on error
  outlet_destroy (outlet);

  // end
#if DEBUG_OUTLET
  fprintf (stderr, "outlet_open_xml: end\n");
#endif
  return 0;
}
