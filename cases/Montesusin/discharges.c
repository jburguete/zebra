#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512

int
main (int argn, char **argc)
{
  const unsigned int days[12]
    = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  char buffer[BUFFER_SIZE];
  FILE *fin, *fout;
  double q, concentration;
  unsigned int year, month, day, hour;
  if (argn != 7)
    {
      printf ("The syntax is:\n./discharges discharges_file "
              "year month day hour concentration\n");
      return 1;
    }
  fin = fopen (argc[1], "r");
  if (!fin)
    {
      printf ("Unable to open the input file\n");
      return 2;
    }
  year = atoi (argc[2]);
  month = atoi (argc[3]);
  day = atoi (argc[4]);
  hour = atoi (argc[5]);
  concentration = atof (argc[6]);
  snprintf (buffer, BUFFER_SIZE, "%s-c", argc[1]);
  fout = fopen (buffer, "w");
  while (fscanf (fin, "%*s%lg%*f%*f%*s", &q) == 1)
    {
      q *= concentration / -3600.;
      fprintf (fout, "%u %u %u %u 0 0 %lg\n", year, month, day, hour, q);
      ++hour;
      if (hour > 23)
        {
          hour = 0;
          ++day;
          if (day > days[month])
            {
              day = 1;
              ++month;
              if (month > 12)
                {
                  month = 1;
                  ++year;
                }
            }
        }
    }
  fclose (fout);
  fclose (fin);
  return 0;
}
