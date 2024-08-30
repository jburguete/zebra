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

  // check arguments
  if (argn != 2)
    {
      printf ("The syntax is:\n./chlorine-input year\n");
      return 1;
    }

  // select year
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

  // null
  snprintf (buffer, BUFFER_SIZE, "chlorine-%u-null.in", year);
  file = fopen (buffer, "w");
  fprintf (file, "%u %u %u %u %u %u 0\n",
           year, month, day, hour, minute, second);
  fclose (file);

  // constant
  snprintf (buffer, BUFFER_SIZE, "chlorine-%u-constant.in", year);
  file = fopen (buffer, "w");
  fprintf (file, "%u %u %u %u %u %u 1\n",
           year, month, day, hour, minute, second);
  fclose (file);

  // altern hours
  snprintf (buffer, BUFFER_SIZE, "chlorine-%u-hour.in", year);
  file = fopen (buffer, "w");
  fprintf (file, "%u %u %u %u %u %u 0\n",
           year, month, day, hour, minute, second);
  fprintf (file, "%u %u %u %u %u %u 1\n",
           year, month, day, hour, minute, second + 1);
  fprintf (file, "%u %u %u %u %u %u 1\n",
           year, month, day, ++hour, minute, second);
  fprintf (file, "%u %u %u %u %u %u 0\n",
           year, month, day, hour, minute, second + 1);
  do
    {
      ++hour;
      if (hour > 23)
        {
          hour = 0;
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
        }
      fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
      fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
      fprintf (file, "%u %u %u %u 0 0 1\n", year, month, day, ++hour);
      fprintf (file, "%u %u %u %u 0 1 0\n", year, month, day, hour);
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
  snprintf (buffer, BUFFER_SIZE, "chlorine-%u-hourb.in", year);
  file = fopen (buffer, "w");
  fprintf (file, "%u %u %u %u %u %u 0\n",
           year, month, day, hour, minute, second);
  fprintf (file, "%u %u %u %u %u %u 1\n",
           year, month, day, hour, minute, second + 1);
  fprintf (file, "%u %u %u %u %u %u 1\n",
           year, month, day, ++hour, minute, second);
  fprintf (file, "%u %u %u %u %u %u 0\n",
           year, month, day, hour, minute, second + 1);
  do
    {
      ++hour;
      if (hour > 23)
        {
          hour = 0;
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
        }
      if (hour % 4)
        continue;
      fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
      fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
      fprintf (file, "%u %u %u %u 0 0 1\n", year, month, day, ++hour);
      fprintf (file, "%u %u %u %u 0 1 0\n", year, month, day, hour);
    }
  while (1);
  fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
           final_day, final_hour, final_minute, final_second);
  fclose (file);

  // 12 hours
  for (hour = 0; hour < 24; ++hour)
    {
      year = initial_year;
      month = initial_month;
      day = initial_day;
      minute = initial_minute;
      second = initial_second;
      snprintf (buffer, BUFFER_SIZE, "chlorine-%u-day-12-%u.in", year, hour);
      file = fopen (buffer, "w");
      fprintf (file, "%u %u %u %u %u %u 0\n",
               year, month, day, initial_hour, minute, second);
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
          if (hour < 12)
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n",
                       year, month, day, hour + 12);
              fprintf (file, "%u %u %u %u 0 1 0\n",
                       year, month, day, hour + 12);
            }
          else
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n",
                       year, month, day + 1, hour - 12);
              fprintf (file, "%u %u %u %u 0 1 0\n",
                       year, month, day + 1, hour - 12);
            }
        }
      while (1);
      fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
               final_day, final_hour, final_minute, final_second);
      fclose (file);
    }

  // 8 hours
  for (hour = 0; hour < 24; ++hour)
    {
      year = initial_year;
      month = initial_month;
      day = initial_day;
      minute = initial_minute;
      second = initial_second;
      snprintf (buffer, BUFFER_SIZE, "chlorine-%u-day-8-%u.in", year, hour);
      file = fopen (buffer, "w");
      fprintf (file, "%u %u %u %u %u %u 0\n",
               year, month, day, initial_hour, minute, second);
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
          if (hour < 16)
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n", year, month, day, hour + 8);
              fprintf (file, "%u %u %u %u 0 1 0\n", year, month, day, hour + 8);
            }
          else
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n",
                       year, month, day + 1, hour - 16);
              fprintf (file, "%u %u %u %u 0 1 0\n",
                       year, month, day + 1, hour - 16);
            }
        }
      while (1);
      fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
               final_day, final_hour, final_minute, final_second);
      fclose (file);
    }

  // 6 hours
  for (hour = 0; hour < 24; ++hour)
    {
      year = initial_year;
      month = initial_month;
      day = initial_day;
      minute = initial_minute;
      second = initial_second;
      snprintf (buffer, BUFFER_SIZE, "chlorine-%u-day-6-%u.in", year, hour);
      file = fopen (buffer, "w");
      fprintf (file, "%u %u %u %u %u %u 0\n",
               year, month, day, initial_hour, minute, second);
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
          if (hour < 18)
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n", year, month, day, hour + 6);
              fprintf (file, "%u %u %u %u 0 1 0\n", year, month, day, hour + 6);
            }
          else
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n",
                       year, month, day + 1, hour - 18);
              fprintf (file, "%u %u %u %u 0 1 0\n",
                       year, month, day + 1, hour - 18);
            }
        }
      while (1);
      fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
               final_day, final_hour, final_minute, final_second);
      fclose (file);
    }

  // 4 hours
  for (hour = 0; hour < 24; ++hour)
    {
      year = initial_year;
      month = initial_month;
      day = initial_day;
      minute = initial_minute;
      second = initial_second;
      snprintf (buffer, BUFFER_SIZE, "chlorine-%u-day-4-%u.in", year, hour);
      file = fopen (buffer, "w");
      fprintf (file, "%u %u %u %u %u %u 0\n",
               year, month, day, initial_hour, minute, second);
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
          if (hour < 20)
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n", year, month, day, hour + 4);
              fprintf (file, "%u %u %u %u 0 1 0\n", year, month, day, hour + 4);
            }
          else
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n",
                       year, month, day + 1, hour - 20);
              fprintf (file, "%u %u %u %u 0 1 0\n",
                       year, month, day + 1, hour - 20);
            }
        }
      while (1);
      fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
               final_day, final_hour, final_minute, final_second);
      fclose (file);
    }

  // 3 hours
  for (hour = 0; hour < 24; ++hour)
    {
      year = initial_year;
      month = initial_month;
      day = initial_day;
      minute = initial_minute;
      second = initial_second;
      snprintf (buffer, BUFFER_SIZE, "chlorine-%u-day-3-%u.in", year, hour);
      file = fopen (buffer, "w");
      fprintf (file, "%u %u %u %u %u %u 0\n",
               year, month, day, initial_hour, minute, second);
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
          if (hour < 21)
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n", year, month, day, hour + 3);
              fprintf (file, "%u %u %u %u 0 1 0\n", year, month, day, hour + 3);
            }
          else
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n",
                       year, month, day + 1, hour - 21);
              fprintf (file, "%u %u %u %u 0 1 0\n",
                       year, month, day + 1, hour - 21);
            }
        }
      while (1);
      fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
               final_day, final_hour, final_minute, final_second);
      fclose (file);
    }

  // 2 hours
  for (hour = 0; hour < 24; ++hour)
    {
      year = initial_year;
      month = initial_month;
      day = initial_day;
      minute = initial_minute;
      second = initial_second;
      snprintf (buffer, BUFFER_SIZE, "chlorine-%u-day-2-%u.in", year, hour);
      file = fopen (buffer, "w");
      fprintf (file, "%u %u %u %u %u %u 0\n",
               year, month, day, initial_hour, minute, second);
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
          if (hour < 22)
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n", year, month, day, hour + 2);
              fprintf (file, "%u %u %u %u 0 1 0\n", year, month, day, hour + 2);
            }
          else
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n",
                       year, month, day + 1, hour - 22);
              fprintf (file, "%u %u %u %u 0 1 0\n",
                       year, month, day + 1, hour - 22);
            }
        }
      while (1);
      fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
               final_day, final_hour, final_minute, final_second);
      fclose (file);
    }

  // 1 hour
  for (hour = 0; hour < 24; ++hour)
    {
      year = initial_year;
      month = initial_month;
      day = initial_day;
      minute = initial_minute;
      second = initial_second;
      snprintf (buffer, BUFFER_SIZE, "chlorine-%u-day-1-%u.in", year, hour);
      file = fopen (buffer, "w");
      fprintf (file, "%u %u %u %u %u %u 0\n",
               year, month, day, initial_hour, minute, second);
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
          if (hour < 23)
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n", year, month, day, hour + 1);
              fprintf (file, "%u %u %u %u 0 1 0\n", year, month, day, hour + 1);
            }
          else
            {
              fprintf (file, "%u %u %u %u 0 0 0\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 1 1\n", year, month, day, hour);
              fprintf (file, "%u %u %u %u 0 0 1\n",
                       year, month, day + 1, hour - 23);
              fprintf (file, "%u %u %u %u 0 1 0\n",
                       year, month, day + 1, hour - 23);
            }
        }
      while (1);
      fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
               final_day, final_hour, final_minute, final_second);
      fclose (file);
    }

  // 1 day every week
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
      fprintf (file, "%u %u %u 8 0 1 1\n", year, month, day);
      fprintf (file, "%u %u %u 16 0 0 1\n", year, month, day);
      fprintf (file, "%u %u %u 16 0 1 0\n", year, month, day);
    }
  while (1);
  fprintf (file, "%u %u %u %u %u %u 0\n", final_year, final_month,
           final_day, final_hour, final_minute, final_second);
  fclose (file);

  // end
  return 0;
}
