#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512
#define NPIPES 146

unsigned int pipes[NPIPES] = {
  1,
  10,
  100,
  102,
  103,
  104,
  105,
  106,
  107,
  11,
  112,
  113,
  117,
  118,
  119,
  12,
  120,
  122,
  123,
  126,
  127,
  128,
  131,
  132,
  134,
  136,
  137,
  138,
  139,
  14,
  142,
  143,
  144,
  145,
  148,
  149,
  15,
  150,
  151,
  156,
  16,
  160,
  162,
  171,
  172,
  175,
  181,
  183,
  184,
  185,
  19,
  2,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  3,
  30,
  31,
  32,
  33,
  34,
  36,
  37,
  38,
  4,
  40,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  48,
  5,
  50,
  51,
  52,
  53,
  54,
  56,
  57,
  58,
  59,
  6,
  60,
  61,
  62,
  63,
  64,
  65,
  66,
  67,
  68,
  69,
  7,
  70,
  71,
  72,
  73,
  74,
  76,
  77,
  78,
  79,
  8,
  80,
  81,
  82,
  83,
  84,
  86,
  87,
  88,
  89,
  9,
  90,
  92,
  93,
  94,
  95,
  96,
  97,
  98,
  99,
  55,
  101,
  108,
  109,
  49,
  85,
  91,
  110,
  111,
  17,
  18,
  39,
  75,
  114
};

int
main (int argn, char **argc)
{
  char buffer[BUFFER_SIZE], name[BUFFER_SIZE];
  FILE *file;
  double second;
  unsigned int i, id, year, month, day, hour, minute;
  if (argn != 7)
    {
      printf ("The syntax is:\n./results year month day hour minute second\n");
      return 1;
    }
  year = atoi (argc[1]);
  month = atoi (argc[2]);
  day = atoi (argc[3]);
  hour = atoi (argc[4]);
  minute = atoi (argc[5]);
  second = atof (argc[6]);
  snprintf (name, BUFFER_SIZE, "%u-%u-%u-%u-%u-%lg",
            year, month, day, hour, minute, second);
  file = fopen ("r.xml", "w");
  fprintf (file, "<?xml version=\"1.0\"?>\n");
  fprintf (file, "<results>\n");
  for (i = 0; i < NPIPES; ++i)
    {
      id = pipes[i];
      fprintf (file, "\t<pipe id=\"%u\" time=\"%u %u %u %u %u %lg\" "
               "file=\"results-pipe-%u-%s\"/>\n",
               id, year, month, day, hour, minute, second, id, name);
    }
  fputs ("</results>", file);
  fclose (file);
  system ("./zebra results.bin r.xml");
  snprintf (buffer, BUFFER_SIZE, "for i in results-pipe-*-%s; "
            "do ./translate csv $i $i.csv; done", name);
  system (buffer);
  snprintf (buffer, BUFFER_SIZE, "echo \"Line\tID\tpatternID\tz\tCl\tH2O2\t"
            "OrganicMatter\tO2\tlarvae\tadults\tmass\" > results-%s.csv", name);
  system (buffer);
  snprintf (buffer, BUFFER_SIZE, "cat results-pipe-*-%s.csv >> results-%s.csv",
            name, name);
  system (buffer);
  snprintf (buffer, BUFFER_SIZE, "rm results-pipe-*-%s*", name);
  system (buffer);
  return 0;
}
