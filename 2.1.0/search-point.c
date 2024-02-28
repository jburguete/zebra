#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <float.h>
#include <math.h>

#define BUFFER_SIZE 512

int
main (int argn, char **argc)
{
  char buffer[BUFFER_SIZE];
  struct dirent *d;
  DIR *dir;
  FILE *fin, *fout;
  char *max_name, *min_name;
  double x, m, max, min;
  unsigned int i, n, column, ncolumns;
  if (argn != 4)
    {
      printf ("The syntax is:\n./search-point directory column ncolumns\n");
      return 1;
    }
  dir = opendir (argc[1]);
  if (!dir)
    {
      printf ("Unable to open the directory\n");
      return 2;
    }
  printf ("directory=%s\n", argc[1]);
  max = -INFINITY;
  min = INFINITY;
  max_name = min_name = NULL;
  column = atoi (argc[2]) - 1;
  ncolumns = atoi (argc[3]);
  snprintf (buffer, BUFFER_SIZE, "%s/averages", argc[1]);
  fout = fopen (buffer, "w");
  while ((d = readdir (dir)))
    {
      snprintf (buffer, BUFFER_SIZE, "%s/%s", argc[1], d->d_name);
      n = 0;
      m = 0.;
      fin = fopen (buffer, "r");
      do
        {
          for (i = 0; i < ncolumns; ++i)
            {
              if (fscanf (fin, "%lg", &x) != 1)
                goto end;
              if (i == column)
                {
                  ++n;
                  m += x;
                }
            }
        }
      while (1);
    end:
      fclose (fin);
      m /= n;
      fprintf (fout, "%s %lg\n", d->d_name, m);
      if (m > max)
        {
          max = m;
          free (max_name);
          max_name = strdup (d->d_name);
        }
      if (m < FLT_EPSILON)
        printf ("file=%s: average null\n", d->d_name);
      else if (m < min)
        {
          min = m;
          free (min_name);
          min_name = strdup (d->d_name);
        }
    }
  fclose (fout);
  closedir (dir);
  printf ("max=%lg file=%s\n", max, max_name);
  printf ("min=%lg file=%s\n", min, min_name);
  return 0;
}
