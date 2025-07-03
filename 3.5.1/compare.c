#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BUFFER_SIZE 512
#define FMT1 "%*s%*s%*s%*s%s%*f%*f%*f%*f%*f%*f%*f%lf%*f"
#define FMT2 "%*s%*s%*s%*s%s%*f%*f%*f%*f%*f%*f%*f%*f%lf"

int
main (int argn, char **argc)
{
  const char *fmt;
  char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE], buffer3[BUFFER_SIZE];
  FILE *file1, *file2;
  char *header;
  double x, xx, xxx, yy, yyy, xa1, xa2, xb1, xb2, xc1, xc2, dx, dy, dxy, ddx,
    ddy, ddxy;
  size_t n;
  int error_code;
  unsigned int na1, na2, nb1, nb2, nc1, nc2;

  // check arguments
  if (argn != 4)
    {
      printf ("The syntax is:\n./compare type file1 file2\n");
      return 1;
    }
  na1 = atoi (argc[1]);
  switch (na1)
    {
    case 1:
      fmt = FMT1;
      break;
    case 2:
      fmt = FMT2;
      break;
    default:
      printf ("Bad type\n");
      return 7;
    }

  // open input files
  file1 = fopen (argc[2], "r");
  if (!file1)
    {
      printf ("Unable to open the 1st input file\n");
      return 2;
    }
  file2 = fopen (argc[3], "r");
  if (!file2)
    {
      fclose (file2);
      printf ("Unable to open the 2nd input file\n");
      return 3;
    }

  // read headers
  header = NULL;
  if (getline (&header, &n, file1) == -1)
    {
      printf ("Bad 1st input file\n");
      error_code = 4;
      goto exit1;
    }
  if (getline (&header, &n, file2) == -1)
    {
      printf ("Bad 2nd input file\n");
      error_code = 5;
      goto exit1;
    }

  // calculating averages
  nb1 = nb2 = 0;
  xb1 = xb2 = 0.;
  if (fscanf (file1, fmt, buffer1, &x) != 2)
    {
      printf ("Bad 1st input file\n");
      error_code = 4;
      goto exit1;
    }
  na1 = nc1 = 1;
  xa1 = xc1 = x;
  if (fscanf (file2, fmt, buffer2, &x) != 2)
    {
      printf ("Bad 2nd input file\n");
      error_code = 5;
      goto exit1;
    }
  if (strcmp (buffer1, buffer2))
    {
      printf ("No correlation files\n");
      error_code = 6;
      goto exit1;
    }
  na2 = nc2 = 1;
  xa2 = xc2 = x;
  do
    {
      if (fscanf (file1, fmt, buffer3, &x) != 2)
        break;
      ++na1;
      xa1 += x;
      if (strcmp (buffer1, buffer3))
        {
          memcpy (buffer1, buffer3, strlen (buffer3) + 1);
          ++nb1;
          xc1 /= nc1;
          xb1 = xc1;
          nc1 = 1;
          xc1 = x;
        }
      else
        {
          ++nc1;
          xc1 += x;
        }
      if (fscanf (file2, fmt, buffer3, &x) != 2)
        break;
      ++na2;
      xa2 += x;
      if (strcmp (buffer2, buffer3))
        {
          memcpy (buffer2, buffer3, strlen (buffer3) + 1);
          if (strcmp (buffer1, buffer2))
            {
              printf ("No correlation files\n");
              error_code = 6;
              goto exit1;
            }
          ++nb2;
          xc2 /= nc2;
          xb2 += xc2;
          nc2 = 1;
          xc2 = x;
        }
      else
        {
          ++nc2;
          xc2 += x;
        }
    }
  while (1);
  if (!na1)
    {
      printf ("Bad 1st input file\n");
      error_code = 4;
      goto exit1;
    }
  if (!na2)
    {
      printf ("Bad 2nd input file\n");
      error_code = 5;
      goto exit1;
    }
  ++nb1;
  xc1 /= nc2;
  xb1 += xc1;
  ++nb2;
  xc2 /= nc2;
  xb2 += xc2;
  error_code = 0;
  xa1 /= na1;
  xa2 /= na2;

  // reopen the files
  rewind (file1);
  rewind (file2);

  // reading headers
  getline (&header, &n, file1);
  getline (&header, &n, file2);

  // calculating coefficients
  nb1 = nb2 = 0;
  xx = xb1 = xb2 = dx = dy = dxy = ddx = ddy = ddxy = 0.;
  fscanf (file1, fmt, buffer1, &x);
  nc1 = 1;
  xc1 = x;
  fscanf (file2, fmt, buffer2, &x);
  nc2 = 1;
  xc2 = x;
  do
    {
      if (fscanf (file1, fmt, buffer3, &x) != 2)
        break;
      xxx = x - xa1;
      ddx += xxx * xxx;
      if (strcmp (buffer1, buffer3))
        {
          memcpy (buffer1, buffer3, strlen (buffer3) + 1);
          xc1 /= nc1;
          xx = xc1 - xa1;
          dx += xx * xx;
          nc1 = 1;
          xc1 = x;
        }
      else
        {
          ++nc1;
          xc1 += x;
        }
      if (fscanf (file2, fmt, buffer3, &x) != 2)
        break;
      yyy = x - xa2;
      ddy += yyy * yyy;
      ddxy += xxx * yyy;
      if (strcmp (buffer2, buffer3))
        {
          memcpy (buffer2, buffer3, strlen (buffer3) + 1);
          xc2 /= nc2;
          yy = xc2 - xa2;
          dy += yy * yy;
          dxy += xx * yy;
          nc2 = 1;
          xc2 = x;
        }
      else
        {
          ++nc2;
          xc2 += x;
        }
    }
  while (1);
  xc1 /= nc1;
  xx = xc1 - xa1;
  dx += xx * xx;
  xc2 /= nc2;
  yy = xc2 - xa2;
  dy += yy * yy;
  dxy += xx * yy;
  printf ("I1=%.14lg%% I2=%.14lg I3=%.14lg\n", 100. * (xa2 - xa1) / xa1,
          ddxy / sqrt (ddx * ddy), dxy / sqrt (dx * dy));

exit1:
  free (header);
  fclose (file2);
  fclose (file1);
  return error_code;
}
