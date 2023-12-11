#include <stdio.h>
#include <float.h>
#include <math.h>

#define BUFFER_SIZE 512
#define D1 0.1704
#define D2 0.2262
#define D3 0.2850
#define DENSITY 997.
#define DP0 2270.59
#define DT1 0.0080
#define DT2 0.0080
#define DT3 0.0080
#define F0 0.
#define INPUT1 "dn200pn16-1"
#define INPUT2 "dn250pn10-1"
#define INPUT3 "dn300pn10-1"
#define INPUT4 "dn300pn10-2"
#define INPUT5 "dn250pn10-3"
#define INPUT6 "dn300pn10-3"
#define K_COLEBROOK_WHITE (2. * exp (1.5))
#define K_GAUCKLER_MANNING (36. * 36. / (91. * 91. * cbrt (2.)))
#define KINEMATIC_VISCOSITY 1.003e-6
#define L1 1.10
#define L2 1.10
#define L3 1.10
#define N1 21
#define N2 26
#define N3 31
#define OUTPUT1 "dn200pn16-1.txt"
#define OUTPUT2 "dn250pn10-1.txt"
#define OUTPUT3 "dn300pn10-1.txt"
#define OUTPUT4 "dn300pn10-2.txt"
#define OUTPUT5 "dn250pn10-3.txt"
#define OUTPUT6 "dn300pn10-3.txt"

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
u (double Q, double D)
{
  return Q / (900. * M_PI * D * D); 
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
  const char *INPUT[6] = { INPUT1, INPUT2, INPUT3, INPUT4, INPUT5, INPUT6 };
  const char *OUTPUT[6] =
    { OUTPUT1, OUTPUT2, OUTPUT3, OUTPUT4, OUTPUT5, OUTPUT6 };
  const double D[6] = { D1, D2, D3, D3, D2, D3 };
  const double L[6] = { L1, L2, L3, L3, L2, L3 };
  const double DT[6] = { DT1, DT2, DT3, DT3, DT2, DT3 };
  const unsigned int N[6] = { N1, N2, N3, N3, N2, N3 };
  char buffer[BUFFER_SIZE];
  double x[4];
  FILE *fin, *fout;
  double sigmat, Lt, ft, Ut, dPt, ctgm, ctcw, f0, dP0, ft0, ctgm0, ctcw0;
  unsigned int i, n;
  for (i = 4; i < 6; ++i)
    {
      fin = fopen (INPUT[i], "r");
      if (!fin)
        {
          snprintf (buffer, BUFFER_SIZE, "Unable to open the file: %s\n",
                    INPUT[i]);
          puts (buffer);
          return ERROR_CODE_UNABLE_TO_OPEN_INPUT_FILE;
        }
      for (n = 0, f0 = 0.;
           fscanf (fin, "%lf%lf%lf%lf", x, x + 1, x + 2, x + 3) == 4; ++n)
        {
          sigmat = (1. + x[0] * 0.01 * 20) * N[i] / (M_PI * D[i] * L[i]);
          Lt = x[1] * 0.001;
          Ut = u (x[2], D[i]);
          dPt = x[3] * 1000.;
          ft = f (D[i], L[i], Ut, dPt);
          if (x[0] < FLT_EPSILON)
            f0 += ft;
          else
            break;
          printf ("f=%lg\n", ft);
        }
      if (n)
        f0 /= n;
      else
        f0 = F0;
      printf ("f0=%lg\n", f0);
      fout = fopen (OUTPUT[i], "w");
      do
        {
          sigmat = (1. + x[0] * 0.01 * 20) * N[i] / (M_PI * D[i] * L[i]);
          Lt = x[1] * 0.001;
          Ut = u (x[2], D[i]);
          dPt = x[3] * 1000.;
          // dP0 = dP (D[i], L[i], Ut, f0);
          dP0 = DP0;
          ft = f (D[i], L[i], Ut, dPt);
          ctgm = ct_gauckler_manning (ft, sigmat, DT[i], Lt, D[i]);
          ctcw = ct_colebrook_white (ft, sigmat, DT[i], Lt, D[i]);
          ft0 = f (D[i], L[i], Ut, dPt - dP0);
          ctgm0 = ct_gauckler_manning (ft0, sigmat, DT[i], Lt, D[i]);
          ctcw0 = ct_colebrook_white (ft0, sigmat, DT[i], Lt, D[i]);
          fprintf (fout,
                   "%.9lg %.9lg %.9lg %.9lg %.9lg %.9lg %.9lg %.9lg %.9lg %.9lg"
                   " %.9lg %.9lg %.9lg %.9lg %lg\n",
                   D[i], L[i], sigmat, DT[i], Lt, Ut, dPt, ft, ctgm, ctcw,
                   dP0, f0, ft0, ctgm0, ctcw0);
        }
      while (fscanf (fin, "%lf%lf%lf%lf", x, x + 1, x + 2, x + 3) == 4);
      fclose (fout);
      fclose (fin);
    }
  return ERROR_CODE_NONE;
}
