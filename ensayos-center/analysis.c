#include <stdio.h>
#include <float.h>
#include <math.h>

#define BUFFER_SIZE 512
#define D1 0.1704
#define D2 0.2262
#define D3 0.2850
#define DENSITY 997.
#define DT1 0.0080
#define DT2 0.0080
#define DT3 0.0080
#define INPUT1 "dn200pn16"
#define INPUT2 "dn250pn10"
#define INPUT3 "dn300pn10"
#define K_COLEBROOK_WHITE (2. * exp (1.5))
#define K_GAUCKLER_MANNING (36. * 36. / (91. * 91. * cbrt (2.)))
#define KINEMATIC_VISCOSITY 1.003e-6
#define L1 1.10
#define L2 1.10
#define L3 1.10
#define OUTPUT1 "dn200pn16.txt"
#define OUTPUT2 "dn250pn10.txt"
#define OUTPUT3 "dn300pn10.txt"

enum ErrorCode
{
  ERROR_CODE_NONE = 0,
  ERROR_CODE_UNABLE_TO_OPEN_INPUT_FILE = 1
};

static inline double
sqr (double x)
{
  return x * x;
}

static inline double
f (double D, double L, double U, double dP)
{
  return 2. * dP * D / (DENSITY * U * U * L);
}

static inline double
dP (double D, double L, double U, double f)
{
  return 0.5 * f * DENSITY * U * U * L / D;
}

static inline double
ct_gauckler_manning (double f, double sigmat, double Dt, double Lt, double D)
{
  return K_GAUCKLER_MANNING * f / (sigmat * Dt * Lt * cbrt (Lt / D));
}

static inline double
ct_colebrook_white_p (double f, double sigmat, double Dt, double Lt, double D)
{
  return 0.25 * f
         / (sigmat * Dt * Lt *
            sqr (1. - 2. * sqrt (f) * log10 (2. * exp (1.5) * Lt / D)));
}

static inline double
ct_colebrook_white_n (double f, double sigmat, double Dt, double Lt, double D)
{
  return 0.25 * f
         / (sigmat * Dt * Lt *
            sqr (1. + 2. * sqrt (f) * log10 (2. * exp (1.5) * Lt / D)));
}

static inline double
ct_colebrook_white (double f, double sigmat, double Dt, double Lt, double D)
{
  double ct;
  ct = ct_colebrook_white_p (f, sigmat, Dt, Lt, D);
  if (1. + 4. * sqrt (sigmat * ct * Dt * Lt) * log10 (2. * exp (1.5) * Lt / D)
      > 0.)
    return ct;
  return ct_colebrook_white_n (f, sigmat, Dt, Lt, D);
}

int
main ()
{
  char buffer[BUFFER_SIZE];
  double x[6];
  FILE *fin, *fout;
  double sigmat, Lt, ft, Ut, dPt, ctgm, ctcw, f0, dP0, ft0, ctgm0, ctcw0;
  unsigned int n;
  fin = fopen (INPUT1, "r");
  if (!fin)
    {
      snprintf (buffer, BUFFER_SIZE, "Unable to open the file: %s\n", INPUT1);
      return ERROR_CODE_UNABLE_TO_OPEN_INPUT_FILE;
    }
  for (n = 0, f0 = 0.;
       fscanf (fin, "%lf%lf%lf%lf%lf%lf", x, x + 1, x + 2, x + 3, x + 4, x + 5)
       == 6; ++n)
    {
      sigmat = (1. + x[0] * 0.01 * 20) * 31 / (M_PI * D1 * L1);
      Lt = x[1] * 0.001;
      Ut = x[2];
      dPt = x[5] * 1000.;
      ft = f (D1, L1, Ut, dPt);
      if (x[0] < FLT_EPSILON)
        f0 += ft;
      else
        break;
      printf ("f=%lg\n", ft);
    }
  if (n)
    f0 /= n;
  printf ("f0=%lg\n", f0);
  fout = fopen (OUTPUT1, "w");
  do
    {
      sigmat = (1. + x[0] * 0.01 * 20) * 31 / (M_PI * D1 * L1);
      Lt = x[1] * 0.001;
      Ut = x[2];
      dPt = x[5] * 1000.;
      dP0 = dP (D1, L1, Ut, f0);
      ft = f (D1, L1, Ut, dPt);
      ctgm = ct_gauckler_manning (ft, sigmat, DT1, Lt, D1);
      ctcw = ct_colebrook_white (ft, sigmat, DT1, Lt, D1);
      ft0 = f (D1, L1, Ut, dPt - dP0);
      ctgm0 = ct_gauckler_manning (ft0, sigmat, DT1, Lt, D1);
      ctcw0 = ct_colebrook_white (ft0, sigmat, DT1, Lt, D1);
      fprintf (fout,
               "%.9lg %.9lg %.9lg %.9lg %.9lg %.9lg %.9lg %.9lg %.9lg %.9lg "
               "%.9lg %.9lg %.9lg %.9lg %lg\n",
               D1, L1, sigmat, DT1, Lt, Ut, dPt, ft, ctgm, ctcw,
               dP0, f0, ft0, ctgm0, ctcw0);
    }
  while (fscanf (fin, "%lf%lf%lf%lf%lf%lf",
                 x, x + 1, x + 2, x + 3, x + 4, x + 5) == 6);
  fclose (fout);
  fclose (fin);
  return ERROR_CODE_NONE;
}
