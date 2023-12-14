#include <stdio.h>
#include <string.h>

int
main ()
{
  FILE *fin, *fout;
  char buffer[512], buffer2[512];
  fin = fopen ("r", "r");
  if (!fin)
    {
      fputs ("No input file", stderr);
      return 1;
    }
  fout = fopen ("w", "w");
  while (fscanf (fin, "%512s%*s%*s%*d%*d%*f%*d%*s%s", buffer, buffer2) == 2)
    {
      if (strncmp (buffer2, ";", 512))
        {
          fputs ("Bad input file", stderr);
          break;
        }
      fprintf (fout, "\t<pipe id=\"%s\" time=\"2022 10 1 15 0 0\" file=\""
               "results-pipe-%s-2022-10-1-15-0-0\"/>\n", buffer, buffer);
    }
  fclose (fout);
  fclose (fin);
  fin = fopen ("r2", "r");
  if (!fin)
    {
      fputs ("No 2nd input file", stderr);
      return 2;
    }
  fout = fopen ("w2", "w");
  while (fscanf (fin, "%512s%*f%*d%s", buffer, buffer2) == 2)
    {
      if (strncmp (buffer2, ";", 512))
        {
          fputs ("Bad 2nd input file", stderr);
          break;
        }
      fprintf (fout, "\t<point id=\"%s\" file=\"results-point-%s\"/>\n",
               buffer, buffer);
    }
  fclose (fout);
  fclose (fin);
  return 0;
}
