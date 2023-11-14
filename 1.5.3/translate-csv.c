#include <stdio.h>
#include <string.h>

enum ErrorCode
{
  ERROR_CODE_NONE,
  ERROR_CODE_ARGUMENTS,
  ERROR_CODE_INPUT_FILE,
  ERROR_CODE_OUTPUT_FILE,
  ERROR_CODE_BAD_INPUT
};

int
main (int argn, char **argc)
{
  const char fmtin[] = "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf";
  const char fmtout[] = "LINESTRING(%.9lg %.9lg,%.9lg %.9lg)\t%s-%u\t%s\t%.9lg"
    "%.9lg\t%.9lg\t%.9lg\t%.9lg\t%.9lg\t%.9lg\t%.9lg\n";
  double x[10], y[10];
  char id[6];
  FILE *fin, *fout;
  unsigned int i, j, k;
  if (argn != 3)
    {
      puts ("The syntax is:\n./translate input_file output_file");
      return ERROR_CODE_ARGUMENTS;
    }
  fin = fopen (argc[1], "r");
  if (!fin)
    {
      puts ("Unable to open the input file");
      return ERROR_CODE_INPUT_FILE;
    }
  fout = fopen (argc[2], "w");
  if (!fout)
    {
      puts ("Unable to open the output file");
      return ERROR_CODE_OUTPUT_FILE;
    }
  for (k = 0; strncmp (argc[1] + k, "results-pipe-", 13); ++k);
  for (i = 0, k += 13; i < 5; ++i)
    {
      j = i + k;
      if (argc[1][j] == '-')
        break;
      id[i] = argc[1][j];
    }
  id[i] = 0;
  printf ("ID=%s\n", id); 
  if (fscanf (fin, fmtin, x, x + 1, x + 2, x + 3, x + 4, x + 5, x + 6, x + 7,
              x + 8, x + 9) != 10)
    {
      puts ("Bad input file");
      return ERROR_CODE_BAD_INPUT;
    }
  for (i = 0; fscanf (fin, fmtin, y, y + 1, y + 2, y + 3, y + 4, y + 5, y + 6,
                      y + 7, y + 8, y + 9) == 10; ++i)
    {
      fprintf (fout, fmtout, x[0], x[1], y[0], y[1], id, i, id, y[2], y[3],
               y[4], y[5], y[6], y[7], y[8], y[9]); 
      memcpy (x, y, 10 * sizeof (double));
    }
  fclose (fout);
  fclose (fin);
  return ERROR_CODE_NONE;
}
