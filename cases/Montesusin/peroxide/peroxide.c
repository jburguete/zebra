#include <stdio.h>
#include <math.h>

const char *name[12] = { "peroxide-2.5-a.txt",
"peroxide-2.5-b.txt",
"peroxide-2.5-c.txt",
"peroxide-5-a.txt",
"peroxide-5-b.txt",
"peroxide-5-c.txt",
"peroxide-10-a.txt",
"peroxide-10-b.txt",
"peroxide-10-c.txt",
"peroxide-15-a.txt",
"peroxide-15-b.txt",
"peroxide-15-c.txt" };

const double r[12] = { 2.5, 2.5, 2.5, 5., 5., 5., 10., 10., 10, 15., 15., 15. };

double
error (double tau)
{
  double x[2];
  double d, di, dn;
  FILE *file;
  unsigned int i, n;
  for (i = 0, d = 0.; i < 12; ++i)
    {
      file = fopen (name[i], "r");
      for (n = 0, di = 0.; fscanf (file, "%lf%lf", x, x + 1) == 2; ++n)
        {
          dn = x[1] - r[i] * exp (-tau * x[0]);
          di += dn * dn;
        }
      fclose (file);
      di /= r[i] * r[i];
      d += di / (double) n;
    }
  return sqrt (d);
}

int
main ()
{
  double t0, t1, tm, e0, e1, em;
  unsigned int i;
  t0 = 0.;
  e0 = error (t0);
  t1 = 1.;
  e1 = error (t1);
  for (i = 0; i < 64; ++i)
    {
      tm = 0.5 * (t0 + t1);
      em = error (tm);
      if (e0 < e1)
        {
          t1 = tm;
          e1 = em;
        }
      else
        {
          t0 = tm;
          e0 = em;
        }
      printf ("tm=%lg em=%lg\n", tm, em);
    }
  return 0;
}
