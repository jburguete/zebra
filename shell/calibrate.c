#include <stdio.h>
#include <stdlib.h>

int
main (int argn, char **argc)
{
  char buffer[512];
  FILE *file;
  double a, d, dmin, dmax, e;
  unsigned int i, j, n;
  if (argn != 5)
    {
      printf ("The syntax is:\n./calibrate input.json dmin dmax n\n");
      return 1;
    }
  dmin = atof (argc[2]);
  dmax = atof (argc[3]);
  n = (unsigned int) atoi (argc[4]);
  for (i = 0; i <= n; ++i)
    {
      a = i * 1. / n;
      for (j = 0; j <= n; ++j)
        {
          d = dmin + j * (dmax - dmin) / n;
          snprintf (buffer, 512, "cp %s tmp.json", argc[1]);
          system (buffer);
          snprintf (buffer, 512,
                    "sed -i 's/\\\"factor\\\": \\\"0\\\"/"
                    "\\\"factor\\\": \\\"%lg\\\"/' tmp.json", a);
          system (buffer);
          snprintf (buffer, 512,
                    "sed -i 's/\\\"dispersion\\\": \\\"0\\\"/"
                    "\\\"dispersion\\\": \\\"%lg\\\"/' tmp.json", d);
          system (buffer);
          system ("./shell tmp.json > tmp");
          file = fopen ("tmp", "r");
          fscanf (file, "%*s%*s%*s%*s%*s%lg", &e);
          fclose (file);
          printf ("%lg %lg %lg\n", a, d, e); 
        }
    }  
  return 0;
}
