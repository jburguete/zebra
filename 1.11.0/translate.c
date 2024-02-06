#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SIZE 10.
#define THICKNESS 0.1

enum ErrorCode
{
  ERROR_CODE_NONE,
  ERROR_CODE_ARGUMENTS,
  ERROR_CODE_TYPE,
  ERROR_CODE_CSV_ARGUMENTS,
  ERROR_CODE_TEX_ARGUMENTS,
  ERROR_CODE_INPUT_FILE,
  ERROR_CODE_OUTPUT_FILE,
  ERROR_CODE_BAD_INPUT
};

enum OutputType
{
  OUTPUT_TYPE_CSV,
  OUTPUT_TYPE_TEX
};

int
main (int argn, char **argc)
{
  const char fmtin[] = "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf";
  const char fmtcsv[] = "LINESTRING(%.3lf %.3lf,%.3lf %.3lf)\t%s-%u\t%s\t%.3lf"
    "\t%.9lg\t%.9lg\t%.9lg\t%.9lg\t%.9lg\t%.9lg\t%.9lg\n";
  double x[10], y[10];
  double xmin, xmax, ymin, ymax, zmin, zmax, d, x0, y0, x1, y1, z;
  char id[6];
  FILE *fin, *fout;
  unsigned int i, j, k, col;
  if (argn != 4 && argn != 7)
    {
      puts ("The syntax is:\n"
            "./translate type(\"csv\" or \"tex\") input_file output_file "
            "[column] [min] [max]");
      return ERROR_CODE_ARGUMENTS;
    }
  fin = fopen (argc[2], "r");
  if (!fin)
    {
      puts ("Unable to open the input file");
      return ERROR_CODE_INPUT_FILE;
    }
  fout = fopen (argc[3], "w");
  if (!fout)
    {
      puts ("Unable to open the output file");
      return ERROR_CODE_OUTPUT_FILE;
    }
  if (!strcmp (argc[1], "csv"))
    {
      if (argn != 4)
        {
          puts ("The syntax for CSV is:\n"
                "./translate csv input_file output_file");
          return ERROR_CODE_CSV_ARGUMENTS;
        }
      for (k = 0; strncmp (argc[2] + k, "-pipe-", 6); ++k);
      for (i = 0, k += 6; i < 5; ++i)
        {
          j = i + k;
          if (argc[2][j] == '-')
            break;
          id[i] = argc[2][j];
        }
      id[i] = 0;
      printf ("ID=%s\n", id);
      if (fscanf (fin, fmtin, x, x + 1, x + 2, x + 3, x + 4, x + 5, x + 6,
                  x + 7, x + 8, x + 9) != 10)
        {
          puts ("Bad input file");
          return ERROR_CODE_BAD_INPUT;
        }
      for (i = 0; fscanf (fin, fmtin, y, y + 1, y + 2, y + 3, y + 4, y + 5,
                          y + 6, y + 7, y + 8, y + 9) == 10; ++i)
        {
          fprintf (fout, fmtcsv, x[0], x[1], y[0], y[1], id, i, id, y[2], y[3],
                   y[4], y[5], y[6], y[7], y[8], y[9]);
          memcpy (x, y, 10 * sizeof (double));
        }
    }
  else if (!strcmp (argc[1], "tex"))
    {
      if (argn != 7)
        {
          puts ("The syntax for TEX is:\n"
                "./translate tex input_file output_file column min max");
          return ERROR_CODE_TEX_ARGUMENTS;
        }
      col = (unsigned int) atoi (argc[4]);
      zmin = atof (argc[5]);
      zmax = atof (argc[6]);
      if (fscanf (fin, fmtin, x, x + 1, x + 2, x + 3, x + 4, x + 5, x + 6,
                  x + 7, x + 8, x + 9) != 10)
        {
          puts ("Bad input file");
          return ERROR_CODE_BAD_INPUT;
        }
      xmin = xmax = x[0];
      ymin = ymax = y[0];
      while (fscanf (fin, fmtin, x, x + 1, x + 2, x + 3, x + 4, x + 5,
                     x + 6, x + 7, x + 8, x + 9) == 10)
        {
          xmin = fmin (xmin, x[0]);
          xmax = fmax (xmax, x[0]);
          ymin = fmin (ymin, x[1]);
          ymax = fmax (ymax, x[1]);
        }
      d = SIZE / fmax (xmax - xmin, ymax - ymin);
      rewind (fin);
      fscanf (fin, fmtin, x, x + 1, x + 2, x + 3, x + 4, x + 5, x + 6,
              x + 7, x + 8, x + 9);
      x0 = (x[0] - xmin) * d;
      y0 = (x[1] - ymin) * d;
      fprintf (fout, "\\begin{pspicture}(0,0)(%lg,%lg)\n",
               (xmax - xmin) * d, (ymax - ymin) * d);
      while (fscanf (fin, fmtin, y, y + 1, y + 2, y + 3, y + 4, y + 5,
                     y + 6, y + 7, y + 8, y + 9) == 10)
        {
          x1 = (x[0] - xmin) * d;
          y1 = (x[1] - ymin) * d;
          z = (x[col] - zmin) * THICKNESS / (zmax - zmin);
          fprintf (fout, "\t\\psline[linecolor=blue](%lg,%lg)(%lg,%lg)\n",
                   x0, y0, x1, y1);
          fprintf (fout, "\t\\psline[linewidth=%lg,linecolor=red]"
                   "(%lg,%lg)(%lg,%lg)\n", z, x0, y0, x1, y1);
          x0 = x1;
          y0 = y1;
        }
      fprintf (fout, "\\end{pspicture}\n");
    }
  else
    {
      puts ("Unknown output type");
      return ERROR_CODE_TYPE;
    }
  fclose (fout);
  fclose (fin);
  return ERROR_CODE_NONE;
}
