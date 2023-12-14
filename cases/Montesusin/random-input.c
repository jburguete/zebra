#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_rng.h>

#define K 0.001
#define YEAR 2022

#if YEAR == 2021
#define DENSITY 6.124489796
#define FINAL_YEAR 2021
#define FINAL_MONTH 8
#define FINAL_DAY 20
#define FINAL_HOUR 0
#define FINAL_MINUTE 0
#define FINAL_SECOND 0
#define INITIAL_YEAR 2021
#define INITIAL_MONTH 7
#define INITIAL_DAY 20
#define INITIAL_HOUR 0
#define INITIAL_MINUTE 0
#define INITIAL_SECOND 0
#elif YEAR == 2022
#define DENSITY 10.861666667
#define FINAL_YEAR 2022
#define FINAL_MONTH 7
#define FINAL_DAY 4
#define FINAL_HOUR 9
#define FINAL_MINUTE 10
#define FINAL_SECOND 0
#define INITIAL_YEAR 2022
#define INITIAL_MONTH 6
#define INITIAL_DAY 14
#define INITIAL_HOUR 17
#define INITIAL_MINUTE 15
#define INITIAL_SECOND 0
#endif


double
r (gsl_rng *rng)
{
  return DENSITY / log ((1. + K) / K) / (K + gsl_rng_uniform (rng));
}

int
main (int argn, char **argc)
{
  const unsigned int days[12] =
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  gsl_rng *rng;
  FILE *file;
  unsigned long int seed;
  unsigned int year, month, day, hour, minute, second;
  if (argn != 3)
    {
      printf ("The syntax is:\n./random-input seed output_file\n");
      return 1;
    }
  seed = atol (argc[1]);
  rng = gsl_rng_alloc (gsl_rng_ranlxd2);
  gsl_rng_set (rng, seed);
  file = fopen (argc[2], "w");
  year = INITIAL_YEAR;
  month = INITIAL_MONTH;
  day = INITIAL_DAY;
  hour = INITIAL_HOUR;
  minute = INITIAL_MINUTE;
  second = INITIAL_SECOND;
  fprintf (file, "%u %u %u %u %u %u %lg\n",
           year, month, day, hour, minute, second, 0.);
  do
    {
      ++hour;
      if (hour > 23)
        {
          hour -= 24;
          ++day;
          if (day > days[month - 1])
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
      if (year == FINAL_YEAR && month == FINAL_MONTH && day == FINAL_DAY)
        break;
      fprintf (file, "%u %u %u %u %u %u %lg\n",
               year, month, day, hour, minute, second, r (rng));
    }
  while (1);
  year = FINAL_YEAR;
  month = FINAL_MONTH;
  day = FINAL_DAY;
  hour = FINAL_HOUR;
  minute = FINAL_MINUTE;
  second = FINAL_SECOND;
  fprintf (file, "%u %u %u %u %u %u %lg\n",
           year, month, day, hour, minute, second, 0.);
  gsl_rng_free (rng);
  fclose (file);
  return 0;
}
