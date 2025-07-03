#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512
#define FMT "%512s"

enum ErrorCode
{
  ERROR_CODE_NONE = 0,
  ERROR_CODE_ARGUMENTS,
  ERROR_CODE_LIST_OPEN,
  ERROR_CODE_LIST_BAD,
  ERROR_CODE_LIST_MEMORY,
};

int
main (int argn, char **argc)
{
  char buffer[BUFFER_SIZE], name[BUFFER_SIZE];
  FILE *file;
  char **pipes;
  const char *id;
  double second;
  unsigned int i, year, month, day, hour, minute, npipes;
  if (argn != 9)
    {
      puts ("The syntax is:\n./results-pipes "
            "results_file list_file year month day hour minute second");
      return ERROR_CODE_ARGUMENTS;
    }
  puts ("Reading list file");
  file = fopen (argc[2], "r");
  if (!file)
    {
      puts ("Unable to open the pipe list file");
      return ERROR_CODE_LIST_OPEN;
    }
  if (fscanf (file, "%u", &npipes) != 1)
    {
      puts ("Bad pipe list file");
      return ERROR_CODE_LIST_BAD;
    }
  pipes = (char **) malloc (npipes * sizeof (char *));
  if (!pipes)
    {
      puts ("Not enough memory to open the pipe list");
      return ERROR_CODE_LIST_MEMORY;
    }
  for (i = 0; i < npipes; ++i)
    {
      pipes[i] = (char *) malloc (BUFFER_SIZE);
      if (fscanf (file, FMT, pipes[i]) != 1)
        {
          puts ("Bad pipe list file");
          return ERROR_CODE_LIST_BAD;
        }
    }
  fclose (file);
  year = atoi (argc[3]);
  month = atoi (argc[4]);
  day = atoi (argc[5]);
  hour = atoi (argc[6]);
  minute = atoi (argc[7]);
  second = atof (argc[8]);
  snprintf (name, BUFFER_SIZE, "%u-%u-%u-%u-%u-%lg",
            year, month, day, hour, minute, second);
  puts ("Doing configuration file");
  file = fopen ("r.xml", "w");
  fprintf (file, "<?xml version=\"1.0\"?>\n");
  fprintf (file, "<results>\n");
  for (i = 0; i < npipes; ++i)
    {
      id = pipes[i];
      fprintf (file, "\t<pipe id=\"%s\" time=\"%u %u %u %u %u %lg\" "
               "file=\"%s-pipe-%s-%s\"/>\n",
               id, year, month, day, hour, minute, second, argc[1], id, name);
    }
  fputs ("</results>", file);
  fclose (file);
  puts ("Executing zebra");
  snprintf (buffer, BUFFER_SIZE, "./zebra %s r.xml", argc[1]);
  system (buffer);
  puts ("Translating every file to .csv");
  snprintf (buffer, BUFFER_SIZE, "for i in %s-pipe-*-%s; "
            "do ./translate csv $i $i.csv; done", argc[1], name);
  system (buffer);
  puts ("Doing final .csv file");
  snprintf (buffer, BUFFER_SIZE, "echo \"Line\tID\tpatternID\tz\tCl\tH2O2\t"
            "OrganicMatter\tO2\tlarvae\tadults\tmass\" > %s-%s.csv",
            argc[1], name);
  system (buffer);
  snprintf (buffer, BUFFER_SIZE, "cat %s-pipe-*-%s.csv >> %s-%s.csv",
            argc[1], name, argc[1], name);
  system (buffer);
  puts ("Removing temporal files");
  snprintf (buffer, BUFFER_SIZE, "rm %s-pipe-*-%s*", argc[1], name);
  system (buffer);
  puts ("Freeing memory");
  for (i = 0; i < npipes; ++i)
    free (pipes[i]);
  free (pipes);
  return ERROR_CODE_NONE;
}
