#include <stdio.h>
#include <stdlib.h>

enum
{
  EXIT_CODE_SUCCESS = 0,
  EXIT_CODE_ARGUMENTS,
  EXIT_CODE_COLUMNS,
  EXIT_CODE_MEMORY,
  EXIT_CODE_INPUT_FILE,
  EXIT_CODE_OUTPUT_FILE_EXISTS,
  EXIT_CODE_OUTPUT_FILE_OPEN,
  EXIT_CODE_PROBE_TYPE,
} ExitCode;

enum
{
  PROBE_TYPE_RESULT = 0,
  PROBE_TYPE_INPUT = 1,
} ProbeType;

void
error_print (const char *msg)
{
  printf ("ERROR!\n%s\n", msg);
}

double
integral (double *x,
          double *f,
          unsigned int n)
{
  double integral;
  unsigned int i, n1;
  if (n < 2)
    return 0.;
  n1 = n - 1;
  for (i = 0, integral = 0.; i < n1; ++i)
    integral += 0.5 * (f[i + 1] + f[i]) * (x[i + 1] - x[i]);
  return 1. / integral;
}

void
file_read (const char *file_name)
{
  double r[2];
  FILE *file;
  double *x, *y;
  unsigned int n;
  file = fopen (file_name, "r");
  if (!file)
    return;
  x = y = NULL;
  for (n = 0; fscanf (file, "%lf%lf", r, r + 1) == 2; ++n)
    {
      x = (double *) realloc (x, (n + 1) * sizeof (double));
      y = (double *) realloc (y, (n + 1) * sizeof (double));
      x[n] = r[0];
      y[n] = r[1];
    }
  printf ("integral=%lg\n", integral (x, y, n));
  free (y);
  free (x);
  fclose (file);
}

int
main (int argn,
      char **argc)
{
  FILE *fin, *fout;
  double *x;
  unsigned int i, n, c1, c2, t;
  if (argn != 7)
    {
      error_print ("The syntax is:\n"
		   "./process input_file output_file "
		   "n_columns 1st_column 2nd_column probe_type\n");
      return EXIT_CODE_ARGUMENTS;
    }
  n = (unsigned int) atoi (argc[3]);
  c1 = (unsigned int) atoi (argc[4]);
  c2 = (unsigned int) atoi (argc[5]);
  t = (unsigned int) atoi (argc[6]);
  if (!n || !c1 || !c2 || n < 2 || c1 > n || c2 > n || c1 == c2)
    {
      error_print ("Bad columns");
      return EXIT_CODE_COLUMNS;
    }
  switch (t)
    {
    case PROBE_TYPE_RESULT:
    case PROBE_TYPE_INPUT:
      break;
    default:
      error_print ("Bad probe type");
      return EXIT_CODE_PROBE_TYPE;
    }
  x = (double *) malloc (n * sizeof (double));
  if (!x)
    {
      error_print ("Failed to allocate memory");
      return EXIT_CODE_MEMORY;
    }
  fin = fopen (argc[1], "r");
  if (!fin)
    {
      error_print ("Failed to open the input file");
      free (x);
      return EXIT_CODE_INPUT_FILE;
    }
  fout = fopen (argc[2], "r");
  if (fout)
    {
      error_print ("Output file exists");
      fclose (fout);
      fclose (fin);
      free (x);
      return EXIT_CODE_OUTPUT_FILE_EXISTS;
    }
  fout = fopen (argc[2], "w");
  if (!fout)
    {
      error_print ("Failed to open the output file");
      fclose (fin);
      free (x);
      return EXIT_CODE_OUTPUT_FILE_OPEN;
    }
  --c1;
  --c2;
  while (1)
    {
      for (i = 0; i < n; ++i)
	if (fscanf (fin, "%lf", x + i) != 1)
          goto end;
      switch (t)
        {
        case PROBE_TYPE_RESULT:
          fprintf (fout, "%lg %lg\n", x[c1], x[c2]);
	  break;
        case PROBE_TYPE_INPUT:
          fprintf (fout, "\"%lg\", \"%lg\",\n", x[c1], x[c2]);
	}
    }
end:
  free (x);
  fclose (fout);
  fclose (fin);
  if (t == PROBE_TYPE_RESULT)
    file_read (argc[2]);
  return EXIT_CODE_SUCCESS;
}
