#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <glib.h>
#include <json-glib/json-glib.h>

#define BUFFER_SIZE 512         ///< default buffer size.
#define DEBUG 0                 ///< 0 for no debug.
#define FLUX_LIMITER 3
///< 0: 1st order upwind,
///< 1: 2nd order upwind,
///< 2: 2nd order centred,
///< 3: 2nd order TVD with monotonized central flux limiter.
#define NUMERICAL_ORDER 2       ///< numercal order.

///> Enum to define the exit codes.
enum
{
  EXIT_CODE_SUCCESS = 0,        ///< success.
  EXIT_CODE_SYNTAX,             ///< error on syntax.
  EXIT_CODE_SIMULATION,         ///< error on simulation.
  EXIT_CODE_FACTOR,             ///< error on velocity factor.
  EXIT_CODE_DISPERSION,         ///< error on dispersion coefficient.
  EXIT_CODE_SWEEPS,             ///< error on sweeps number.
} ExitCode;

/**
 * \struct Input
 * \brief Struct to define shell inputs.
 */
typedef struct
{
  double *time;                ///< array of times.
  double *density;             ///< array of time densities.
  double position;             ///< longitudinal position.
  unsigned int n_times;        ///< number of times.
  unsigned int cell;           ///< cell index.
} Input;

/**
 * \struct Result
 * \brief Struct to define results.
 */
typedef struct
{
  double *x;                    ///< array of x data.
  double *y;                    ///< array of y data.
  unsigned int n;               ///< number of data.
} Result;

/**
 * \struct Probe
 * \brief Struct to define probes.
 */
typedef struct
{
  Result *result;               ///< array of results data.
  double position;              ///< longitudinal position.
  unsigned int n_results;       ///< number of results.
  unsigned int cell;            ///< cell index.
} Probe;

/**
 * \struct Simulation
 * \brief Struct to define the simulation.
 */
typedef struct
{
  Input *input;                 ///< array of inputs.
  Probe *probe;                 ///< array of probes.
  FILE *file_probes;            ///< probes file.
  double *x;                    ///< array of cell longitudinal coordinates.
  double *density;              ///< array of cell shell densities.
  double *dd;                   ///< array of numerical differences.
  double *dfp;
  ///< array of 1st order positive numerical flows.
  double *dfn;
  ///< array of 1st order negative numerical flows.
  double *C;                    ///< left array of numerical diffusion. 
  double *D;                    ///< diagonal array of numerical diffusion. 
  double *E;                    ///< right array of numerical diffusion. 
#if NUMERICAL_ORDER > 1
  double *df2p;
  ///< array of 2nd order positive numerical flows.
  double *df2n;
  ///< array of 2nd order negative numerical flows.
#endif
  double velocity;              ///< fluid velocity.
  double factor;                ///< factor of shell velocity.
  double dispersion;            ///< dispersion coefficient.
  double length;                ///< pipe length.
  double area;                  ///< pipe area.
  double diameter;              ///< pipe diameter.
  double dx;                    ///< cell size.
  double dt;                    ///< time step size.
  double cfl;                   ///< CFL number.
  double time;                  ///< simulation time.
  unsigned int n_cells;         ///< number of cells.
  unsigned int n_inputs;        ///< number of inputs.
  unsigned int n_probes;        ///< number of probes.
} Simulation;

char *error_message = NULL;

/**
 * Function to get the index of the nearest cell to a position.
 *
 * \return cell index.
 */
unsigned int
get_index (double x,            ///< position.
           double dx,           ///< cells size.
           unsigned int n_cells)        ///< number of cells
{
  int i;
  unsigned int n1;
  i = (int) round (x / dx);
  if (i < 0)
    return 0;
  n1 = n_cells - 1;
  if (i > (int) n1)
    return n1;
  return (unsigned int) i;
}

/**
 * Function to add an error message.
 */
void
error_add (const char *msg)     ///< error message.
{
  char *buffer;
  if (error_message)
    {
      buffer = (char *) g_strconcat (error_message, "\n", msg, NULL);
      g_free (error_message);
      error_message = buffer;
    }
  else
    error_message = g_strdup (msg);
}

#if NUMERICAL_ORDER > 1

/**
 * Flux limiter function.
 *
 * \return flux limiter value.
 */
double
flux_limiter (double upwind __attribute__((unused)),    ///< upwind flux.
              double centred __attribute__((unused)))   ///< centred flux.
{
#if FLUX_LIMITER == 0
  return 0.;
#elif FLUX_LIMITER == 1
  return upwind;
#elif FLUX_LIMITER == 2
  return centred;
#else
  double r;
  if (fabs (upwind * centred) < DBL_EPSILON)
    return 0.;
  r = centred / upwind;
  if (r <= 1. / 3.)
    return centred + centred;
  if (r >= 3.)
    return upwind + upwind;
  return 0.5 * (upwind + centred);
#endif
}

#endif

/**
 * Function to calculate an interpolation between tabular data.
 *
 * \return interpolated value.
 */
double
interpolate (double x,          ///< interpolation value.
             double *restrict xi,
///< array of interpolation tabular values.
             double *restrict yi,
///< array of interpolated tabular values.
             unsigned int n)    ///< number of tabular elements.
{
  unsigned int i1, i2, i3, n1;
  if (n < 2 || x <= xi[0])
    return yi[0];
  n1 = n - 1;
  if (x >= xi[n1])
    return yi[n1];
  i1 = 0;
  i3 = n1;
  while (i3 > i1 + 1)
    {
      i2 = (i1 + i3) / 2;
      if (x > xi[i2])
        i1 = i2;
      else
        i3 = i2;
    }
  return yi[i1] + (x - xi[i1]) * (yi[i3] - yi[i1]) / (xi[i3] - xi[i1]);
}

/**
 * Function to normalize a tabular function.
 */
void
normalize (double *x,           ///< array of x-coordinate data.
           double *f,           ///< array of function data.
           unsigned int n)      ///< number of data.
{
  double integral;
  unsigned int i, n1;
  if (n < 2)
    return;
  n1 = n - 1;
  for (i = 0, integral = 0.; i < n1; ++i)
    integral += 0.5 * (f[i + 1] + f[i]) * (x[i + 1] - x[i]);
  integral = 1. / integral;
printf ("integral=%lg\n", integral);
  for (i = 0; i < n; ++i)
    f[i] *= integral;
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

/**
 * Function to print an error message.
 */
static inline void
error_print ()
{
  if (error_message)
    printf ("ERROR!\n%s\n", error_message); 
}

/**
 * Function to get a floating number, in double format, from a property of a
 * JSON object.
 *
 * \return floating number value in double format.
 */
double
json_object_get_float (JsonObject *object,      ///< JSON object struct.
                       const char *prop,        ///< JSON object property.
                       unsigned int *error)
///< error code (1 on success, 0 on error).
{
  JsonNode *node;
  const char *buffer;
  double x = 0.;
  *error = 0;
  node = json_object_get_member (object, prop);
  if (json_node_get_node_type (node) == JSON_NODE_VALUE
      && json_node_get_value_type (node) == G_TYPE_STRING)
    {
      buffer = (const char *) json_node_get_string (node);
      if (!buffer || sscanf (buffer, "%lf", &x) != 1)
        return 0.;
      *error = 1;
    }
  return x;
}

/**
 * Function to get an integer number, in long int format, from a property of a
 * JSON object.
 *
 * \return integer number value in long int format.
 */
long int
json_object_get_int (JsonObject *object,        ///< JSON object struct.
                     const char *prop,  ///< JSON object property.
                     unsigned int *error)
///< error code (1 on success, 0 on error).
{
  JsonNode *node;
  const char *buffer;
  long int x = 0l;
  *error = 0;
  node = json_object_get_member (object, prop);
  if (json_node_get_node_type (node) == JSON_NODE_VALUE
      && json_node_get_value_type (node) == G_TYPE_STRING)
    {
      buffer = (const char *) json_node_get_string (node);
      if (!buffer || sscanf (buffer, "%ld", &x) != 1)
        return 0l;
      *error = 1;
    }
  return x;
}

/**
 * Function to get an array of floating numbers, in double format, from a
 * property of a JSON object.
 *
 * \return array of floating numbers value in double format on success, NULL on
 * error.
 */
double*
json_object_get_floats (JsonObject *object,     ///< JSON object struct.
                        const char *prop,       ///< JSON object property.
                        unsigned int *n_elements)
///< number of array elements on success, 0 on error.
{
  JsonNode *node;
  JsonArray *array;
  GList *list, *list_initial;
  const char *buffer;
  double *x = NULL;
  unsigned int n;
  *n_elements = 0;
  node = json_object_get_member (object, prop);
  if (json_node_get_node_type (node) != JSON_NODE_ARRAY)
    return x;
  array = json_node_get_array (node);
  list_initial = json_array_get_elements (array);
  for (n = 0, list = list_initial; list; ++n, list = list->next)
    {
      node = (JsonNode *) list->data;
      if (json_node_get_node_type (node) != JSON_NODE_VALUE
          || json_node_get_value_type (node) != G_TYPE_STRING)
        goto exit_on_error;
      buffer = (const char *) json_node_get_string (node);
      if (!buffer)
        goto exit_on_error;
      x = (double *) realloc (x, (n + 1) * sizeof (double));
      if (!x)
        goto exit_on_error;
      if (sscanf (buffer, "%lf", x + n) != 1)
        goto exit_on_error;
    }
  g_list_free (list_initial);
  *n_elements = n;
  return x;

exit_on_error:
  g_list_free (list_initial);
  free (x);
  return NULL;
}

/**
 * Function to get two arrays of floating numbers, in double format, from a
 * property of a JSON object.
 *
 * \return number of array elements.
 */
unsigned int
json_object_get_two_floats (JsonObject *object, ///< JSON object struct.
                            const char *prop,   ///< JSON object property.
                            double **x, ///< pointer to the 1st array.
                            double **y) ///< pointer to the 2nd array.
///< number of array elements on success, 0 on error.
{
  double *z, *xx, *yy;
  unsigned int i, j, n;
  *x = *y = NULL;
  z = json_object_get_floats (object, prop, &n);
  if (!z)
    return 0;
  if (n & 1)
    {
      free (z);
      return 0;
    }
  n >>= 1;
  xx = (double *) malloc (n * sizeof (double));
  yy = (double *) malloc (n * sizeof (double));
  if (!xx || !yy)
    {
      free (yy);
      free (xx);
      return 0;
    }
  for (i = j = 0; i < n; ++i)
    {
      xx[i] = z[j++];
      yy[i] = z[j++];
      if (i && xx[i] <= xx[i - 1])
        {
          free (yy);
          free (xx);
          return 0;
        }
    }
  free (z);
  *x = xx;
  *y = yy;
  return n;
}

/**
 * Function to free the data used by an Input struct.
 */
void
input_destroy (Input *input)    ///< Input struct.
{
  free (input->density);
  free (input->time);
  input->density = input->time = NULL;
}

/**
 * Function to add an error message opening an Input struct.
 */
void
input_error (const char *msg)   ///< error message.
{
  error_add ("Input:");
  error_add (msg);
}

/**
 * Function to read an input in a JSON object.
 *
 * \return 1 on success, 0 on error.
 */
static inline int
input_read (Input *input,
            JsonObject *object)
{
  unsigned int i;
  input->n_times
    = json_object_get_two_floats (object, "densities",
                                  &input->time, &input->density);
  if (!input->n_times)
    {
      input_destroy (input);
      input_error ("failed to read densities");
      return 0;
    }
  input->position = json_object_get_float (object, "position", &i);
  if (!i)
    {
      input_destroy (input);
      input_error ("failed to read position");
      return 0;
    }
#if DEBUG
  for (i = 0; i < input->n_times; ++i)
    fprintf (stderr, "input %u: time=%lg density=%lg\n",
             i, input->time[i], input->density[i]);
  fprintf (stderr, "input: position=%lg\n", input->position);
#endif
  return 1;
}

/**
 * Function to init the variables used by an input.
 */
static inline void
input_init (Input *input,       ///< Input struct.
            double dx,          ///< cells size.
            unsigned int n_cells)       ///< number of cells.
{
  input->cell = get_index (input->position, dx, n_cells);
  // input normalized
  normalize (input->time, input->density, input->n_times);
}

/**
 * Function to get the current input.
 *
 * \return current density.
 */
static inline double
input_current (Input *input,    ///< Input struct.
               double t)        ///< current time.
{
  return interpolate (t, input->time, input->density, input->n_times);
}

/**
 * Function to free the data used by a Result struct.
 */
void
result_destroy (Result *result)    ///< Result struct.
{
  free (result->y);
  free (result->x);
  result->y = result->x = NULL;
}

/**
 * Function to add an error message opening a Result struct.
 */
void
result_error (const char *msg)   ///< error message.
{
  error_add ("Result:");
  error_add (msg);
}

/**
 * Function to read a result in a JSON object.
 *
 * \return 1 on success, 0 on error.
 */
static inline int
result_read (Result *result,    ///< Result struct.
             const char *file_name)       ///< file name.
{
  double r[2];
  FILE *file;
  double *x, *y;
  unsigned int n;
#if DEBUG
  unsigned int i;
#endif
  result->x = result->y = x = y = NULL;
  file = fopen (file_name, "r");
  if (!file)
    {
      result_error ("failed to open the file");
      return 0;
    }
  for (n = 0; fscanf (file, "%lf%lf", r, r + 1) == 2; ++n)
    {
      x = (double *) realloc (x, (n + 1) * sizeof (double));
      if (!x)
        {
          result_error ("failed to allocate memory");
          goto exit_on_error;
        }
      y = (double *) realloc (y, (n + 1) * sizeof (double));
      if (!y)
        {
          result_error ("failed to allocate memory");
          goto exit_on_error;
        }
      x[n] = r[0];
      y[n] = r[1];
    }
  if (!n)
    {
      result_error ("failed to read data");
      goto exit_on_error;
    }
#if DEBUG
  for (i = 0; i < n; ++i)
    fprintf (stderr, "result %u: x=%lg y=%lg\n", i, x[i], y[i]);
#endif
  result->x = x;
  result->y = y;
  result->n = n;
  return 1;

exit_on_error:
  free (y);
  free (x);
  if (file)
    fclose (file);
  return 0;
}

/**
 * Function to init the variables used by a result.
 */
static inline void
result_init (Result *result)       ///< Result struct.
{
  // result normalized
  normalize (result->x, result->y, result->n);
}

/**
 * Function to compare a result with a tabular function.
 *
 * \return quadratic error.
 */
double
result_compare (Result *result, ///< Result struct.
                double x,       ///< x data.
                double y)       ///< y data.
{
  double e;
  e = interpolate (x, result->x, result->y, result->n) - y;
  return e;
}

/**
 * Function to free the data used by an Probe struct.
 */
void
probe_destroy (Probe *probe)    ///< Probe struct.
{
  Result *result;
  unsigned int i, n_results;
  result = probe->result;
  n_results = probe->n_results;
  for (i = 0; i < n_results; ++i)
    result_destroy (result + i);
  free (probe->result);
  probe->result = NULL;
}

/**
 * Function to add an error message opening an Probe struct.
 */
void
probe_error (const char *msg)   ///< error message.
{
  error_add ("Probe:");
  error_add (msg);
}

/**
 * Function to read an probe in a JSON object.
 *
 * \return 1 on success, 0 on error.
 */
static inline int
probe_read (Probe *probe,
            JsonObject *object)
{
  JsonNode *node;
  JsonArray *array;
  GList *list, *list_initial = NULL;
  const char *buffer;
  unsigned int n;
  probe->result = NULL;
  probe->n_results = 0;
  probe->position = json_object_get_float (object, "position", &n);
  if (!n)
    {
      probe_error ("failed to read position");
      return 0;
    }
#if DEBUG
  fprintf (stderr, "probe: position=%lg\n", probe->position);
#endif
  node = json_object_get_member (object, "results");
  if (node)
    {
      if (json_node_get_node_type (node) != JSON_NODE_ARRAY)
        {
          probe_error ("bad results format");
          return 0;
        }
      array = json_node_get_array (node);
      list_initial = json_array_get_elements (array);
      for (n = 0, list = list_initial; list; list = list->next)
        {
          node = (JsonNode *) list->data;
          if (json_node_get_node_type (node) != JSON_NODE_VALUE)
            {
              probe_error ("bad results type");
              goto exit_on_error;
            }
          buffer = (const char *) json_node_get_string (node);
          if (!buffer)
            {
              probe_error ("bad results type");
              goto exit_on_error;
            }
          probe->result
            = (Result *) realloc (probe->result, (n + 1) * sizeof (Result));
          if (!result_read (probe->result + n, buffer))
            goto exit_on_error;
          probe->n_results = ++n;
        }
      g_list_free (list_initial);
    }
#if DEBUG
  fprintf (stderr, "probe: n_results=%u\n", probe->n_results);
#endif
  return 1;

exit_on_error:
  g_list_free (list_initial);
  probe_destroy (probe);
  return 0;
}

/**
 * Function to init the variables used by an probe.
 */
static inline void
probe_init (Probe *probe,       ///< Probe struct.
            double dx,          ///< cells size.
            unsigned int n_cells)       ///< number of cells.
{
  Result *result;
  unsigned int i, n_results;
  probe->cell = get_index (probe->position, dx, n_cells);
  result = probe->result;
  n_results = probe->n_results;
  for (i = 0; i < n_results; ++i)
    result_init (result + i);
}

/**
 * Function to compare the probe results with a tabular function.
 *
 * \return quadratic error.
 */
double
probe_compare (Probe *probe,    ///< Probe struct.
               double time,     ///< time.
               double *y)       ///< array of y data.
{
  Result *result;
  double x, f, e;
  unsigned int i, n_results;
  n_results = probe->n_results;
#if DEBUG
  fprintf (stderr, "probe_compare: n_results=%u\n", n_results);
#endif
  if (!n_results)
    return 0.;
  result = probe->result;
  x = y[probe->cell];
#if DEBUG
  fprintf (stderr, "probe_compare: f=%lg\n", x);
#endif
  for (e = 0., i = 0; i < n_results; ++i)
    {
      f = result_compare (result + i, time, x);
#if DEBUG
      fprintf (stderr, "probe_compare: i=%u e=%lg\n", i, f);
#endif
      e += f * f;
    }
  return sqrt (e / (double) n_results);
}

/**
 * Function to free the memory used in a Simulation struct.
 */
static inline void
simulation_destroy (Simulation *s)      ///< Simulation struct.
{
  Input *input;
  Probe *probe;
  unsigned int i, n;
  free (s->E);
  free (s->D);
  free (s->C);
#if NUMERICAL_ORDER > 1
  free (s->df2n);
  free (s->df2p);
#endif
  free (s->dfn);
  free (s->dfp);
  free (s->dd);
  free (s->density);
  free (s->x);
  probe = s->probe;
  n = s->n_probes;
  for (i = 0; i < n; ++i)
    probe_destroy (probe + i);
  free (s->probe);
  input = s->input;
  n = s->n_inputs;
  for (i = 0; i < n; ++i)
    input_destroy (input + i);
  free (input);
  if (s->file_probes)
    fclose (s->file_probes);
}

/**
 * Function to add an error message opening a Simulation struct.
 */
void
simulation_error (const char *msg)      ///< error message.
{
  error_add ("Simulation:");
  error_add (msg);
}

/**
 * Function to read the data of a Simulation struct from a JSON file.
 *
 * \return 1 on success, 0 on error.
 */
int
simulation_read (Simulation *s,         ///< Simulation struct.
                 const char *file_name, ///< JSON input file name.
                 unsigned int save)     ///< 0 on no saving probe results.
{
  JsonParser *parser;
  JsonNode *node, *child;
  JsonObject *root, *object;
  JsonArray *array;
  GList *list, *list_initial = NULL;
  const char *buffer;
  unsigned int i;
  s->input = NULL;
  s->probe = NULL;
  s->file_probes = NULL;
  s->E = s->D = s->C = s->dfn = s->dfp = s->dd = s->density = s->x = NULL;
#if NUMERICAL_ORDER > 1
  s->df2n = s->df2p = NULL;
#endif
  s->n_inputs = s->n_probes = 0;
  parser = json_parser_new ();
  if (!json_parser_load_from_file (parser, file_name, NULL))
    {
      simulation_error ("unable to parse the input file");
      goto exit_on_error;
    }
  node = json_parser_get_root (parser);
  root = json_node_get_object (node);
  node = json_object_get_member (root, "simulation");
  if (!node)
    {
      simulation_error ("bad input file");
      goto exit_on_error;
    }
  object = json_node_get_object (node);
  child = json_object_get_member (object, "input");
  if (!child)
    {
      simulation_error ("no inputs");
      goto exit_on_error;
    }
  s->n_inputs = 1;
  s->input = (Input *) malloc (sizeof (Input));
  if (!input_read (s->input, json_node_get_object (child)))
    goto exit_on_error;
  child = json_object_get_member (object, "probes");
  if (!child)
    {
      simulation_error ("no probes");
      goto exit_on_error;
    }
  if (json_node_get_node_type (child) != JSON_NODE_ARRAY)
    {
      simulation_error ("bad probes");
      goto exit_on_error;
    }
  array = json_node_get_array (child);
  list_initial = json_array_get_elements (array);
  for (list = list_initial; list; list = list->next)
    {
      node = (JsonNode *) list->data;
      s->probe
        = (Probe *) realloc (s->probe, (s->n_probes + 1) * sizeof (Probe));
      if (!s->probe)
        {
          simulation_error ("failed to allocate probes");
          goto exit_on_error;
        }
      if (!probe_read (s->probe + s->n_probes, json_node_get_object (node)))
        goto exit_on_error;
      ++s->n_probes;
    }
  s->length = json_object_get_float (object, "length", &i);
  if (!i)
    {
      simulation_error ("failed to read pipe length");
      goto exit_on_error;
    }
  s->diameter = json_object_get_float (object, "diameter", &i);
  if (!i)
    {
      simulation_error ("failed to read pipe diameter");
      goto exit_on_error;
    }
  s->velocity = json_object_get_float (object, "velocity", &i);
  if (!i)
    {
      simulation_error ("failed to read flow velocity");
      goto exit_on_error;
    }
  s->dispersion = json_object_get_float (object, "dispersion", &i);
  if (!i)
    {
      simulation_error ("failed to read dispersion coefficient");
      goto exit_on_error;
    }
  s->factor = json_object_get_float (object, "factor", &i);
  if (!i)
    {
      simulation_error ("failed to read velocity factor");
      goto exit_on_error;
    }
  s->time = json_object_get_float (object, "time", &i);
  if (!i)
    {
      simulation_error ("failed to read simulation time");
      goto exit_on_error;
    }
  s->cfl = json_object_get_float (object, "cfl", &i);
  if (!i)
    {
      simulation_error ("failed to read CFL number");
      goto exit_on_error;
    }
  s->n_cells = (unsigned int)
    json_object_get_int (object, "cells-number", &i);
  if (!i)
    {
      simulation_error ("failed to read number of cells");
      goto exit_on_error;
    }
  buffer
    = json_node_get_string (json_object_get_member (object, "probes-file"));
  if (!buffer)
    {
      simulation_error ("no probes file");
      goto exit_on_error;
    }
  if (save)
    {
      s->file_probes = fopen (buffer, "w");
      if (!s->file_probes)
        {
          simulation_error ("failed to open the probes file");
          goto exit_on_error;
        }
    }
  g_list_free (list_initial);
  g_object_unref (parser);
  return 1;

exit_on_error:
  g_list_free (list_initial);
  g_object_unref (parser);
  simulation_destroy (s);
  return 0;
}

/**
 * Function to reset the variables used by a simulation
 */
static inline void
simulation_reset (Simulation *s)        ///< Simulation struct.
{
  double *d;
  unsigned int i, n;
  d = s->density;
  n = s->n_cells;
  for (i = 0; i < n; ++i)
    d[i] = 0.;
  s->dt = s->cfl * s->dx / fabs (s->velocity * s->factor);
}

/**
 * Function to init the variables used by a simulation.
 */
static inline void
simulation_init (Simulation *s) ///< Simulation struct.
{
  Input *input;
  Probe *probe;
  unsigned int i, n_cells, n1;
  s->area = 0.25 * M_PI * s->diameter * s->diameter;
  n_cells = s->n_cells;
  s->dx = s->length / (double) n_cells;
  s->x = (double *) malloc (n_cells * sizeof (double));
  s->density = (double *) malloc (n_cells * sizeof (double));
  n1 = n_cells - 1;
  s->dd = (double *) malloc (n1 * sizeof (double));
  s->dfp = (double *) malloc (n1 * sizeof (double));
  s->dfn = (double *) malloc (n1 * sizeof (double));
  s->C = (double *) malloc (n1 * sizeof (double));
  s->D = (double *) malloc (n_cells * sizeof (double));
  s->E = (double *) malloc (n1 * sizeof (double));
#if NUMERICAL_ORDER > 1
  --n1;
  s->df2p = (double *) malloc (n1 * sizeof (double));
  s->df2n = (double *) malloc (n1 * sizeof (double));
#endif
  for (i = 0; i < n_cells; ++i)
    s->x[i] = i * s->dx;
  input = s->input;
  for (i = 0; i < s->n_inputs; ++i)
    input_init (input + i, s->dx, s->n_cells);
  probe = s->probe;
  for (i = 0; i < s->n_probes; ++i)
    probe_init (probe + i, s->dx, s->n_cells);
  simulation_reset (s);
}

/**
 * Function to write the probes file.
 */
static void
simulation_probes_write (Simulation *s, ///< Simulation struct.
                         double t)      ///< current time.
{
  Probe *probe;
  FILE *file;
  unsigned int i;
  file = s->file_probes;
  fprintf (file, "%lg", t);
  probe = s->probe;
  for (i = 0; i < s->n_probes; ++i)
    fprintf (file, " %lg", s->density[probe[i].cell]);
  fprintf (file, "\n");
}

/**
 * Function to get the current inputs.
 */
static inline void
simulation_inputs (Simulation *s,       ///< Simulation struct.
                   double t)    ///< current time.
{
  Input *input;
  unsigned int i;
  input = s->input;
  for (i = 0; i < s->n_inputs; ++i)
    {
      s->density[input[i].cell] = input_current (input + i, t);
#if DEBUG
      fprintf (stderr, "input %u: cell=%u\n", i, input[i].cell);
#endif
    }
}

/**
 * Function to perform a simulation step.
 */
static inline void
simulation_step (Simulation *s)
{
  double *d, *dd, *dfp, *dfn, *C, *D, *E;
  double v, dx, dtdx, nudtdx, k, k2, k3;
  unsigned int i, n, n1;
#if NUMERICAL_ORDER > 1
  double *df2p, *df2n;
  unsigned int n2;
  df2p = s->df2p;
  df2n = s->df2n;
#endif
  d = s->density;
  dx = s->dx;
  dd = s->dd;
  dfp = s->dfp;
  dfn = s->dfn;
  C = s->C;
  D = s->D;
  E = s->E;
  dtdx = s->dt / dx;
  n = s->n_cells;
  n1 = n - 1;
  for (i = 0; i < n1; ++i)
    dd[i] = d[i + 1] - d[i];
  v = s->velocity * s->factor;
  if (v > 0.)
    {
      for (i = 0; i < n1; ++i)
        {
          dfp[i] = v * dd[i];
          dfn[i] = 0.;
        }
#if NUMERICAL_ORDER > 1
      k = 0.5 * (1. - v * dtdx);
      n2 = n1 - 1;
      for (i = 0; i < n2; ++i)
        {
          df2p[i] = k * flux_limiter (dfp[i], dfp[i + 1]);
          df2n[i] = 0.;
        }
#endif
    }
  else
    {
      for (i = 0; i < n1; ++i)
        {
          dfn[i] = v * dd[i];
          dfp[i] = 0.;
        }
#if NUMERICAL_ORDER > 1
      k = 0.5 * (1. + v * dtdx);
      n2 = n1 - 1;
      for (i = 0; i < n2; ++i)
        {
          df2n[i] = k * flux_limiter (dfn[i + 1], dfn[i]);
          df2p[i] = 0.;
        }
#endif
    }
  for (i = 0; i < n1; ++i)
    {
      d[i] -= dtdx * dfn[i];
      d[i + 1] -= dtdx * dfp[i];
    }
#if NUMERICAL_ORDER > 1
  for (i = 0; i < n2; ++i)
    {
      d[i + 1] -= dtdx * df2p[i];
      d[i + 2] += dtdx * df2p[i];
      d[i] += dtdx * df2n[i];
      d[i + 1] -= dtdx * df2n[i];
    }
#endif
  nudtdx = s->dispersion * dtdx;
  k = fmax (0.5 * nudtdx, nudtdx - 0.5 * dx);
  k2 = 2. * k + dx;
  k3 = nudtdx - k;
  k = -k;
  E[0] = 0.;
  for (i = 1; i < n1; ++i)
    E[i] = C[i - 1] = k;
  C[i - 1] = 0.;
  D[0] = dx;
  d[0] = d[0] * dx + k3 * dd[0];
  for (i = 1; i < n1; ++i)
    {
      D[i] = k2;
      d[i] = d[i] * dx + k3 * (dd[i] - dd[i - 1]);
    }
  D[i] = dx;
  d[i] = d[i] * dx - k3 * dd[n2];
  matrix_tridiagonal_solve (C, D, E, d, n);
}

/**
 * Function to compare the results of a simulation.
 *
 * \return root square error value.
 */
double
simulation_compare (Simulation *s,      ///< Simulation struct.
                    double t)   ///< current time.
{
  Probe *probe;
  double *y;
  double f, e;
  unsigned int i, n_probes;
  n_probes = s->n_probes;
#if DEBUG
  fprintf (stderr, "simulation_compare: n_probes=%u\n", n_probes);
#endif
  if (!n_probes)
    return 0.;
  probe = s->probe;
  y = s->density;
  for (e = 0., i = 0; i < n_probes; ++i)
    {
      f = probe_compare (probe + i, t, y);
#if DEBUG
      fprintf (stderr, "simulation_compare: i=%u e=%lg\n", i, f);
#endif
      e += f * f;
    }
  return sqrt (e / (double) n_probes);
}

/**
 * Function to run the simulation.
 */
static inline double
simulation_run (Simulation *s,  ///< Simulation struct.
                unsigned int save)      ///< 0 on no saving probe results.
{
  double t, tnext, f, e;
  unsigned long int n_steps;
#if DEBUG
  unsigned int i;
#endif
  t = e = 0.;
  n_steps = 0l;
#if DEBUG
  fprintf (stderr, "init profile t=%lg\n", t);
  for (i = 0; i < s->n_cells; ++i)
    fprintf (stderr, "%lg %lg\n", s->x[i], s->density[i]);
#endif
  simulation_inputs (s, t);
#if DEBUG
  fprintf (stderr, "inputs profile t=%lg\n", t);
  for (i = 0; i < s->n_cells; ++i)
    fprintf (stderr, "%lg %lg\n", s->x[i], s->density[i]);
#endif
  if (save)
    simulation_probes_write (s, t);
  while (t < s->time)
    {
      tnext = t + s->dt;
      if (tnext > s->time)
        {
          tnext = s->time;
          s->dt = tnext - t;
        }
      simulation_step (s);
      t = tnext;
      simulation_inputs (s, t);
      if (save)
        simulation_probes_write (s, t);
      f = simulation_compare (s, t);
      e += f * f;
      ++n_steps;
    }
  return sqrt (e / (double) n_steps);
}

/**
 * Function to set simulation calibration values.
 */
static inline void
simulation_calibration_set (Simulation *s,      ///< Simulation struct.
                            double factor,      ///< velocity factor.
                            double dispersion)  ///< dispersion coefficient.
{
  simulation_reset (s);
  s->factor = factor;
  s->dispersion = dispersion;
}

/**
 * Function to perform a calibration step.
 */
static inline void
simulation_calibration_step (Simulation *s,     ///< Simulation struct.
                             double fmin,       ///< minimum velocity factor.
                             double fmax,       ///< maximum velocity factor.
                             double dmin,
///< minimum dispersion coefficient.
                             double dmax,
///< maximum dispersion coefficient.
                             unsigned int n_sweeps)
///< number of sweeps.
{
  double e, f, d, df, dd, ebest, fbest, dbest;
  unsigned int i, j;
  ebest = INFINITY;
  fbest = fmin;
  dbest = dmin;
  df = fmax - fmin;
  dd = dmax - dmin;
  for (i = 0; i <= n_sweeps; ++i)
    {
      printf ("i=%u\n", i);
      f = fmin + (double) i * df / (double) n_sweeps;
      for (j = 0; j <= n_sweeps; ++j)
        {
          printf ("j=%u\n", j);
          d = dmin + (double) j * dd / (double) n_sweeps;
          simulation_calibration_set (s, f, d);
          e = simulation_run (s, 0);
          printf ("e=%lg\n", e);
          if (e < ebest)
            {
              ebest = e;
              fbest = f;
              dbest = d;
            }
        }
    }
  printf ("best error=%lg\n", ebest);
  printf ("best factor=%lg\n", fbest);
  printf ("best dispersion=%lg\n", dbest);
}

/**
 * Main function.
 *
 * \return 0 on success, error code on error.
 */
int
main (int argn,                 ///< number of arguments.
      char **argc)              ///< array of arguments.
{
  Simulation s;
  double e, fmin, fmax, dmin, dmax;
  unsigned int n_sweeps, exit_code = EXIT_CODE_SUCCESS;
  switch (argn)
    {
    case 2:
      if (simulation_read (&s, argc[1], 1))
        {
          simulation_init (&s);
          e = simulation_run (&s, 1);
          printf ("error=%lg\n", e);
          simulation_destroy (&s);
        }
      else
        exit_code = EXIT_CODE_SIMULATION;
      break;
    case 7:
      fmin = atof (argc[2]);
      fmax = atof (argc[3]);
      if (fmin <= 0. || fmin >= fmax)
        {
          error_add ("bad factor limits");
          return EXIT_CODE_FACTOR;
        }
      dmin = atof (argc[4]);
      dmax = atof (argc[5]);
      if (dmin < 0. || dmin > dmax)
        {
          error_add ("bad dispersion limits");
          return EXIT_CODE_DISPERSION;
        }
      n_sweeps = (unsigned int) atoi (argc[6]);
      if (n_sweeps < 2)
        {
          error_add ("bad sweeps number");
          return EXIT_CODE_SWEEPS;
        }
      if (simulation_read (&s, argc[1], 0))
        {
          simulation_init (&s);
          simulation_calibration_step (&s, fmin, fmax, dmin, dmax, n_sweeps);
          simulation_destroy (&s);
        }
      else
        exit_code = EXIT_CODE_SIMULATION;
      break;
    default:
      error_add ("the syntax is:");
      error_add ("./shell input_file.json");
      exit_code = EXIT_CODE_SYNTAX;
    }
  error_print ();
  return exit_code;
}
