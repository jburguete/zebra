#include <stdio.h>
#include <stdlib.h>
#include <math.h>

enum
{
  EXIT_CODE_SUCCESS = 0,
  EXIT_CODE_ARGUMENTS,
  EXIT_CODE_INPUT_FILE,
  EXIT_CODE_OUTPUT_FILE_EXISTS,
  EXIT_CODE_OUTPUT_FILE_OPEN,
} ExitCode;

void
error_print (const char *msg)
{
  printf ("ERROR!\n%s\n", msg);
}

int
main (int argn,
      char **argc)
{
  double r[2];
  FILE *fin, *fout;
  double k1, k2, kmin, kmax;
  if (argn != 5)
    {
      error_print ("The syntax is:\n"
		   "./process2 input_file output_file k1 k2\n");
      return EXIT_CODE_ARGUMENTS;
    }
  k1 = atof (argc[3]);
  k2 = atof (argc[4]);
  fin = fopen (argc[1], "r");
  if (!fin)
    {
      error_print ("Failed to open the input file");
      return EXIT_CODE_INPUT_FILE;
    }
  fout = fopen (argc[2], "r");
  if (fout)
    {
      error_print ("Output file exists");
      fclose (fout);
      fclose (fin);
      return EXIT_CODE_OUTPUT_FILE_EXISTS;
    }
  fout = fopen (argc[2], "w");
  if (!fout)
    {
      error_print ("Failed to open the output file");
      fclose (fin);
      return EXIT_CODE_OUTPUT_FILE_OPEN;
    }
  kmin = INFINITY;
  kmax = -INFINITY;
  while (fscanf (fin, "%lf%lf", r, r + 1) == 2)
    {
      kmin = fmin (kmin, r[1]);
      kmax = fmax (kmax, r[1]);
      fprintf (fout, "%lg %lg\n", r[0], k2 + k1 * r[1]);
    }
  fclose (fout);
  fclose (fin);
  printf ("min=%lg max=%lg\n", kmin, kmax);
  return EXIT_CODE_SUCCESS;
}
