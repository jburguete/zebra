#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512

#define INITIAL_YEAR_2021 2021
#define INITIAL_MONTH_2021 5
#define INITIAL_DAY_2021 13
#define INITIAL_HOUR_2021 2
#define INITIAL_MINUTE_2021 0
#define INITIAL_SECOND_2021 0
#define FINAL_YEAR_2021 2021
#define FINAL_MONTH_2021 10
#define FINAL_DAY_2021 1
#define FINAL_HOUR_2021 15
#define FINAL_MINUTE_2021 0
#define FINAL_SECOND_2021 0

#define INITIAL_YEAR_2022 2022
#define INITIAL_MONTH_2022 5
#define INITIAL_DAY_2022 13
#define INITIAL_HOUR_2022 2
#define INITIAL_MINUTE_2022 0
#define INITIAL_SECOND_2022 0
#define FINAL_YEAR_2022 2022
#define FINAL_MONTH_2022 10
#define FINAL_DAY_2022 1
#define FINAL_HOUR_2022 15
#define FINAL_MINUTE_2022 0
#define FINAL_SECOND_2022 0

int
main (int argn, char **argc)
{
  const unsigned int days[12] =
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  char buffer[BUFFER_SIZE];
  FILE *file;
  unsigned int year, month, day, hour, minute, second, initial_year,
    initial_month, initial_day, initial_hour, initial_minute, initial_second,
    final_year, final_month, final_day, final_hour, final_minute, final_second;
  if (argn != 2)
    {
      printf ("The syntax is:\n./random-input year\n");
      return 1;
    }
  year = atoi (argc[1]);
  if (year == 2021)
    {
      initial_year = INITIAL_YEAR_2021;
      initial_month = INITIAL_MONTH_2021;
      initial_day = INITIAL_DAY_2021;
      initial_hour = INITIAL_HOUR_2021;
      initial_minute = INITIAL_MINUTE_2021;
      initial_second = INITIAL_SECOND_2021;
      final_year = FINAL_YEAR_2021;
      final_month = FINAL_MONTH_2021;
      final_day = FINAL_DAY_2021;
      final_hour = FINAL_HOUR_2021;
      final_minute = FINAL_MINUTE_2021;
      final_second = FINAL_SECOND_2021;
    }
  else if (year == 2022)
    {
      initial_year = INITIAL_YEAR_2022;
      initial_month = INITIAL_MONTH_2022;
      initial_day = INITIAL_DAY_2022;
      initial_hour = INITIAL_HOUR_2022;
      initial_minute = INITIAL_MINUTE_2022;
      initial_second = INITIAL_SECOND_2022;
      final_year = FINAL_YEAR_2022;
      final_month = FINAL_MONTH_2022;
      final_day = FINAL_DAY_2022;
      final_hour = FINAL_HOUR_2022;
      final_minute = FINAL_MINUTE_2022;
      final_second = FINAL_SECOND_2022;
    }
  else
    {
      printf ("Unknown year\n");
      return 2;
    }
  year = initial_year;
  month = initial_month;
  day = initial_day;
  hour = initial_hour;
  minute = initial_minute;
  second = initial_second;
  snprintf (buffer, BUFFER_SIZE, "chlorine-%u-constant.in", year);
  file = fopen (buffer, "w");
  fprintf (file, "%u %u %u %u %u %u 1\n",
           year, month, day, hour, minute, second);
  fclose (file);
  snprintf (buffer, BUFFER_SIZE, "chlorine-%u-day.in", year);
  file = fopen (buffer, "w");
  fprintf (file, "%u %u %u %u %u %u 0\n",
           year, month, day, hour, minute, second);
  do
    {
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
      if (year == final_year && month == final_month && day == final_day)
        break;
      fprintf (file, "%u %u %u 8 0 0 0\n", year, month, day);
      fprintf (file, "%u %u %u 8 1 0 1\n", year, month, day);
      fprintf (file, "%u %u %u 16 0 0 1\n", year, month, day);
      fprintf (file, "%u %u %u 16 0 1 0\n", year, month, day);
    }
  while (1);
  fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
           final_day, final_hour, final_minute, final_second);
  fclose (file);
  year = initial_year;
  month = initial_month;
  day = initial_day;
  hour = initial_hour;
  minute = initial_minute;
  second = initial_second;
  snprintf (buffer, BUFFER_SIZE, "chlorine-%u-week.in", year);
  file = fopen (buffer, "w");
  fprintf (file, "%u %u %u %u %u %u 0\n",
           year, month, day, hour, minute, second);
  do
    {
      day += 7;
      if (day > days[month - 1])
        {
          day -= days[month - 1];
          ++month;
          if (month > 12)
            {
              month = 1;
              ++year;
            }
        }
      if (year == final_year && month == final_month && day + 7 > final_day)
        break;
      fprintf (file, "%u %u %u 8 0 0 0\n", year, month, day);
      fprintf (file, "%u %u %u 8 1 0 1\n", year, month, day);
      fprintf (file, "%u %u %u 16 0 0 1\n", year, month, day);
      fprintf (file, "%u %u %u 16 0 1 0\n", year, month, day);
    }
  while (1);
  fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
           final_day, final_hour, final_minute, final_second);
  fclose (file);
  return 0;
}
