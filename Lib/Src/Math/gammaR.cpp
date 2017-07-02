#include "pch.h"
#include <Math/MathFunctions.h>

/* (6,4)-Minimax-approximation of gamma in [-5.000000e-001,5.000000e-001] with max error = 6.423750e-013 */
static BYTE coefdata[] = {
   0x42,0xfa,0xb4,0x91,0xf8,0x5b,0xec,0x3f, /* 8.8622692545306614e-001 */
   0xa1,0xba,0x87,0xd9,0x8d,0xac,0xe2,0x3f, /* 5.8356373297086261e-001 */
   0x9e,0x74,0xaa,0x15,0xa2,0x08,0xd3,0x3f, /* 2.9740192523770215e-001 */
   0x4c,0x98,0x29,0x96,0xea,0x51,0xb7,0x3f, /* 9.1093694363366839e-002 */
   0xb0,0xc9,0xe8,0x96,0x0e,0x1c,0x99,0x3f, /* 2.4521091423773178e-002 */
   0xc0,0x4c,0x63,0x0b,0x76,0x6d,0x70,0x3f, /* 4.0106402504591077e-003 */
   0x80,0xf3,0x81,0x17,0xd3,0xa2,0x44,0x3f, /* 6.2976176469896339e-004 */
   0x7c,0x4b,0xba,0x0e,0x5a,0xe7,0xe3,0x3f, /* 6.2199118496424388e-001 */
   0x91,0x46,0xcd,0xf5,0xf9,0xdc,0xc3,0xbf, /* -1.5518116476100777e-001 */
   0x7a,0x0d,0xeb,0x6f,0xcc,0x8b,0xaf,0xbf, /* -6.1613453542601129e-002 */
   0x9b,0x01,0xc0,0x96,0x77,0xfc,0x8d,0x3f  /* 1.4641699111962938e-002 */
};

static double *coef = (double*)coefdata;
static double approximation(double x) {
  double sum1 = coef[6];
  double sum2 = coef[10];
  int i;
  for( i = 5; i >= 0; i-- ) sum1 = sum1 * x + coef[i];
  for( i = 9; i >  6; i-- ) sum2 = sum2 * x + coef[i];
  return sum1 / (sum2 * x + 1.0);
}

// assume 1 < x < 2
static inline double gamma1_2R(double x) {
  return approximation(x-1.5);
}

Real gamma(const Real &x) {
  if(x == floor(x)) {
    if(x < 1) {
      return 1.0/(x-floor(x)); // undefined
    }
    Real res = 1;
    for(Real p = x-1; p > 1; p-=1) {
      res *= p;
    }
    return res;
  }

  if(x > 2) {
    Real res = 1, p = x-1;
    for(; p > 2; p-=1) {
      res *= p;
    }
    return res * p * gamma1_2R(getDouble(p));
  } else if(x < 1) {
    Real res = 1, p = x;
    for(; p < 1; p+=1) {
      res /= p;
    }
    return res * gamma1_2R(getDouble(p));
  } else {
    return gamma1_2R(getDouble(x));
  }
}

Real lnGamma(const Real &x) {
  if(x <= 0) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("x=%s. Must be > 0")
                                 , toString(x).cstr()); // undefined
  }
  if(x == floor(x)) {
    Real res = 0;
    for(Real p = x-1; p > 1; p -= 1) {
      res += log(p);
    }
    return res;
  }

  if(x > 2) {
    Real res = 0, p = x - 1;
    for(; p > 2; p -= 1) {
      res += log(p);
    }
    // p = ]1; 2[
    return res + log(p) + log(gamma1_2R(getDouble(p)));
  } else { // x = ]0; 2[
    Real res = 0, p = x;
    for(; p < 1; p += 1) {
      res -= log(p);
    }
    // p = ]1; 2[
    return res + log(gamma1_2R(getDouble(p)));
  }
}

// Calculates x! = x*(x-1)*(x-2)*...*2*1, extended to real numbers by the gamma function x! = gamma(x+1)
Real fac(const Real &x) {
  return gamma(x+1);
}
