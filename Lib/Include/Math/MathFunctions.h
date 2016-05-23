#pragma once

#include "Real.h"

#ifndef LONGDOUBLE

#include <Math.h>

#else

Real cosh(                const Real &x);
Real sinh(                const Real &x);
Real tanh(                const Real &x);

#endif

int    getExpo10(         double x);
bool   isNan(             double x);
bool   isPInfinity(       double x);
bool   isNInfinity(       double x);
bool   isInfinity(        double x);
double root(              double x, double y);
double cot(               double x);
double acot(              double x);
void   sincos(double &c, double &s);

typedef enum {
  RADIANS
 ,DEGREES
 ,GRADS
} TrigonometricMode;

Real csc(                 const Real &x);
Real sec(                 const Real &x);
Real acsc(                const Real &x);
Real asec(                const Real &x);
//Real acosh(               const Real &x);
//Real asinh(               const Real &x);
//Real atanh(               const Real &x);

Real sinDegrees(          const Real &x);
Real cosDegrees(          const Real &x);
Real tanDegrees(          const Real &x);
Real cotDegrees(          const Real &x);
Real asinDegrees(         const Real &x);
Real acosDegrees(         const Real &x);
Real atanDegrees(         const Real &x);
Real acotDegrees(         const Real &x);

Real cscDegrees(          const Real &x);
Real secDegrees(          const Real &x);
Real acscDegrees(         const Real &x);
Real asecDegrees(         const Real &x);

Real sinGrads(            const Real &x);
Real cosGrads(            const Real &x);
Real tanGrads(            const Real &x);
Real cotGrads(            const Real &x);
Real asinGrads(           const Real &x);
Real acosGrads(           const Real &x);
Real atanGrads(           const Real &x);
Real acotGrads(           const Real &x);

Real cscGrads(            const Real &x);
Real secGrads(            const Real &x);
Real acscGrads(           const Real &x);
Real asecGrads(           const Real &x);

Real sin(                 const Real &x, TrigonometricMode mode);
Real cos(                 const Real &x, TrigonometricMode mode);
Real tan(                 const Real &x, TrigonometricMode mode);
Real cot(                 const Real &x, TrigonometricMode mode);
Real asin(                const Real &x, TrigonometricMode mode);
Real acos(                const Real &x, TrigonometricMode mode);
Real atan(                const Real &x, TrigonometricMode mode);
Real acot(                const Real &x, TrigonometricMode mode);
Real csc(                 const Real &x, TrigonometricMode mode);
Real sec(                 const Real &x, TrigonometricMode mode);
Real acsc(                const Real &x, TrigonometricMode mode);
Real asec(                const Real &x, TrigonometricMode mode);

Real gamma(               const Real &x);
Real gauss(               const Real &x);
Real norm(                const Real &x);
Real probitFunction(      const Real &x);
Real errorFunction(       const Real &x);
Real inverseErrorFunction(const Real &x);
Real fac(                 const Real &x);
Real dsign(               const Real &x);

Real mypow(               const Real &x,      const Real &y    );
Real binomial(            const Real &x,      const Real &y    );
Real dmax(                const Real &x1,     const Real &x2   );
Real dmin(                const Real &x1,     const Real &x2   );
int  dmax(                int         x1,     int         x2   );
int  dmin(                int         x1,     int         x2   );

Real random(              const Real &lower,  const Real &upper); // use _standardRandomGenerator declared in Random.h
Real randomGaussian(      const Real &mean,   const Real &s    ); // mean and standardDeviation. use _standardRandomGenerator

void setToRandom(Real &x);

Real poly( const Real &x, int degree, const Real *coef); // poly(x)  = a[n]x^n + a[n-1]x^(n-1)+...+a[1]x + a[0]; n = degree
Real poly1(const Real &x, int degree, const Real *coef); // poly1(x) = a[0]x^n + a[1]x^(n-1)+...+a[n-1]x + a[n]; n = degree

Real ator(const char *str); 
Real ttor(const TCHAR *str); 

