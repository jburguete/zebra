/**
 * \file results.h
 * \brief header file to define the results data base file.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2021-2023, Javier Burguete Tolosa.
 */
#ifndef RESULTS__H
#define RESULTS__H 1

/**
 * \struct ResultsHeader
 * \brief struct to define the results data base header.
 */
typedef struct
{
  double initial_time;          ///< initial time in seconds since 1970.
  double final_time;            ///< final time in seconds since 1970.
  double saving_step;           ///< time step size to save results.
  double cell_size;             ///< maximum cell size.
  unsigned int nsolutes;        ///< number of solutes.
  unsigned int nspecies;        ///< number of species.
  unsigned int npoints;         ///< number of points.
  unsigned int npipes;          ///< number of pipes.
} ResultsHeader;

typedef struct
{
  ResultsHeader header[1];      ///< header data.
  double *point_x;              ///< array of point x-coordinates.
  double *point_y;              ///< array of point y-coordinates.
  double *point_z;              ///< array of point z-coordinates.
  char *point_id;               ///< array of point identifiers.
  double *pipe_length;          ///< array of pipe lengths.
  char *pipe_id;                ///< array of pipe identifiers.
  char *pipe_inlet_id;          ///< array of pipe identifiers.
  char *pipe_outlet_id;         ///< array of pipe identifiers.
  unsigned int *pipe_cell;      ///< array of inlet pipe cells.
  double *variable;             ///< array of variables.
  unsigned int nvariables;      ///< number of variables.
  FILE *file;                   ///< file.
  long header_position;         ///< position after header on the file.
} Results;

void results_destroy (Results * results);
void results_set (Results * results, Network * network);
int results_open_bin (Results * results, char *file_name);
int results_open_xml (Results * results, char *file_name);

/**
 * function to init the header of a results data base.
 */
static inline void
results_header_init (ResultsHeader * header,
                     ///< pointer to the results data base header struct.
                     Network * network, ///< pointer to the network data struct.
                     double initial_time,
                     ///< initial time in seconds since 1970.
                     double final_time, ///< final time in seconds since 1970.
                     double saving_step)        ///< time step size to save results.
{
#if DEBUG_RESULTS
  fprintf (stderr, "results_header_init: start\n");
#endif
  header->initial_time = initial_time;
  header->final_time = final_time;
  header->saving_step = saving_step;
  header->cell_size = network->cell_size;
  header->nsolutes = nsolutes;
  header->nspecies = nspecies;
  header->npoints = network->npoints;
  header->npipes = network->npipes;
#if DEBUG_RESULTS
  fprintf (stderr, "results_header_init: end\n");
#endif
}

/**
 * function to init a results data base.
 */
static inline void
results_init (Results * results,
              ///< pointer to the results data base struct.
              Network * network,        ///< pointer to the network data struct.
              double initial_time,
              ///< initial time in seconds since 1970.
              double final_time,        ///< final time in seconds since 1970.
              double saving_step)       ///< time step size to save results.
{
  Pipe *pipe;
  Point *point;
  unsigned int i, j, n;
#if DEBUG_RESULTS
  fprintf (stderr, "results_init: start\n");
#endif
  results_header_init (results->header, network, initial_time, final_time,
                       saving_step);
  n = network->npoints;
  results->point_x = (double *) malloc (n * sizeof (double));
  results->point_y = (double *) malloc (n * sizeof (double));
  results->point_z = (double *) malloc (n * sizeof (double));
  results->point_id = (char *) malloc (n * MAX_LABEL_LENGTH * sizeof (char));
  point = network->point;
  for (i = 0; i < n; ++i)
    {
      memcpy (results->point_id + i * MAX_LABEL_LENGTH, point[i].id,
              MAX_LABEL_LENGTH * sizeof (char));
      results->point_x[i] = point[i].x;
      results->point_y[i] = point[i].y;
      results->point_z[i] = point[i].z;
    }
  n = network->npipes;
  results->pipe_length = (double *) malloc (n * sizeof (double));
  results->pipe_id = (char *) malloc (n * MAX_LABEL_LENGTH * sizeof (char));
  results->pipe_inlet_id
    = (char *) malloc (n * MAX_LABEL_LENGTH * sizeof (char));
  results->pipe_outlet_id
    = (char *) malloc (n * MAX_LABEL_LENGTH * sizeof (char));
  results->pipe_cell
    = (unsigned int *) malloc ((n + 1) * sizeof (unsigned int));
  pipe = network->pipe;
  results->pipe_cell[0] = j = 0;
  for (i = 0; i < n; ++i)
    {
      memcpy (results->pipe_id + i * MAX_LABEL_LENGTH, pipe[i].id,
              MAX_LABEL_LENGTH * sizeof (char));
      memcpy (results->pipe_inlet_id + i * MAX_LABEL_LENGTH, pipe[i].inlet_id,
              MAX_LABEL_LENGTH * sizeof (char));
      memcpy (results->pipe_outlet_id + i * MAX_LABEL_LENGTH, pipe[i].outlet_id,
              MAX_LABEL_LENGTH * sizeof (char));
      results->pipe_length[i] = pipe[i].length;
      results->pipe_cell[i + 1] = j += pipe[i].ncells;
    }
  results->nvariables = j *= nsolutes + 2 * nspecies;
  results->variable = (double *) malloc (j * sizeof (double));
#if DEBUG_RESULTS
  fprintf (stderr, "results_init: end\n");
#endif
}

/**
 * function to write the results data base header on a file.
 */
static inline void
results_write_header (Results * results,
                      ///< pointer to the results data base struct.
                      FILE * file)      ///< file.
{
  ResultsHeader *header;
  unsigned int n;
#if DEBUG_RESULTS
  fprintf (stderr, "results_write_header: start\n");
#endif
  header = results->header;
  fwrite (header, sizeof (ResultsHeader), 1, file);
  n = header->npoints;
  fwrite (results->point_id, sizeof (unsigned int), n, file);
  fwrite (results->point_x, sizeof (double), n, file);
  fwrite (results->point_y, sizeof (double), n, file);
  fwrite (results->point_z, sizeof (double), n, file);
  n = header->npipes;
  fwrite (results->pipe_id, sizeof (unsigned int), n, file);
  fwrite (results->pipe_inlet_id, sizeof (unsigned int), n, file);
  fwrite (results->pipe_outlet_id, sizeof (unsigned int), n, file);
  fwrite (results->pipe_length, sizeof (double), n, file);
  fwrite (results->pipe_cell, sizeof (unsigned int), n + 1, file);
#if DEBUG_RESULTS
  fprintf (stderr, "results_write_header: end\n");
#endif
}

/**
 * function to write the results data base variables on a file.
 */
static inline void
results_write_variables (Results * results,
                         ///< pointer to the results data base struct.
                         FILE * file)   ///< file.
{
#if DEBUG_RESULTS
  fprintf (stderr, "results_write_variables: start\n");
#endif
  fwrite (results->variable, sizeof (double), results->nvariables, file);
#if DEBUG_RESULTS
  fprintf (stderr, "results_write_variables: end\n");
#endif
}

#endif
