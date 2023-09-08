#include <stdio.h>

const unsigned int days[13] =
  { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

int
main ()
{
  FILE *out;
  unsigned int i, year, month, day, hour;
  year = 2022;
  month = 5;
  day = 13;
  hour = 2;
  out = fopen ("list.xml", "w");
  for (i = 0; i < 3398; ++i, ++hour)
    {
      if (hour == 24)
        {
          hour = 0;
          ++day;
          if (day > days[month])
            {
              day = 1;
              ++month;
            }
        }
      fprintf (out, "  <discharges time=\"%u %u %u %u 0 0\" "
               "file=\"rpt_files/Montesusin_R1_%u.rpt\"/>\n",
               year, month, day, hour, i);
    }
  fclose (out);
  return 0;
}
