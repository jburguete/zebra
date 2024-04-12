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
  char buffer[BUFFER_SIZE];
  FILE *file;
  char **points;
  const char *id;
  unsigned int i, npoints;
  if (argn != 3)

    {
      printf ("The syntax is:\n./results-points results_file list_file\n");
      return 1;
    }
  puts ("Reading list file");
  file = fopen (argc[2], "r");
  if (!file)
    {
      puts ("Unable to open the point list file");
      return ERROR_CODE_LIST_OPEN;
    }
  if (fscanf (file, "%u", &npoints) != 1)
    {
      puts ("Bad point list file");
      return ERROR_CODE_LIST_BAD;
    }
  points = (char **) malloc (npoints * sizeof (char *));
  if (!points)
    {
      puts ("Not enough memory to open the point list");
      return ERROR_CODE_LIST_MEMORY;
    }
  for (i = 0; i < npoints; ++i)
    {
      points[i] = (char *) malloc (BUFFER_SIZE);
      if (fscanf (file, FMT, points[i]) != 1)
        {
          puts ("Bad point list file");
          return ERROR_CODE_LIST_BAD;
        }
    }
  fclose (file);
  printf ("Doing results directory\n");
  snprintf (buffer, BUFFER_SIZE, "mkdir %s.d", argc[1]);
  system (buffer);
  printf ("Doing configuration file\n");
  file = fopen ("r.xml", "w");
  fprintf (file, "<?xml version=\"1.0\"?>\n");
  fprintf (file, "<results>\n");
  for (i = 0; i < npoints; ++i)

    {
      id = points[i];
      fprintf (file, "\t<point id=\"%s\" file=\"%s.d/point-%s\"/>\n",
               id, argc[1], id);
    }
  fputs ("</results>", file);
  fclose (file);
  printf ("Executing zebra\n");
  snprintf (buffer, BUFFER_SIZE, "./zebra %s r.xml", argc[1]);
  system (buffer);
  puts ("Freeing memory");
  for (i = 0; i < npoints; ++i)
    free (points[i]);
  free (points);
  return ERROR_CODE_NONE;
}
