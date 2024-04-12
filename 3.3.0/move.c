#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

int
main (int argn, char **argc)
{
  char buffer[BUFFER_SIZE];
  FILE *fin, *fout;
  double x, y, w, h;
  if (argn != 5)
    {
      printf ("The syntax is:\n./move x y in.inp out.inp\n");
      return 1;
    }
  w = atof (argc[1]);
  h = atof (argc[2]);
  fin = fopen (argc[3], "r");
  if (!fin)
    {
      printf ("Unable to open the input file\n");
      return 2;
    }
  fout = fopen (argc[4], "w");
  while (fscanf (fin, "%s%lf%lf", buffer, &x, &y) == 3)
    fprintf (fout, "%s %.2lf %.2lf\n", buffer, x + w, y + h);
  fclose (fout);
  fclose (fin);
  return 0;
}
