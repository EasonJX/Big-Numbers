#include "stdafx.h"
#include <Math.h>
#include <float.h>
#include <Math/MathLib.h>

Real dfdx(Real x, Function &f) {
#define EPS 5e-8
  const Real x1 = (x!=0)?(x*(1-EPS)):-EPS;
  const Real x2 = (x!=0)?(x*(1+EPS)): EPS;
  const Real y1 = f(x1);
  const Real y2 = f(x2);
  return (y2-y1)/(x2-x1);
}

static BigReal lowerIncGammaTaylor(const BigReal &a, const BigReal &x, int digits) {
  if(x.isNegative()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("x=%s. Valid range:[0; inf["), toString(x).cstr());
  }
  BigReal sum = 0, ai = a, p = rQuot(BIGREAL_1,ai,digits);
  ++ai;

  for(;;) {
    BigReal lastSum = sum;
    sum = rSum(sum, p, digits);
    if(sum == lastSum) {
      break;
    }
    p = rQuot(rProd(p, x, digits), ai, digits);
    ++ai;
  }
  return rProd(rProd(rExp(-x, digits), rPow(x, a, digits), digits), sum, digits);
}

BigReal BRchiSquaredDistribution(UINT df, const BigReal &x, int digits) {
  const BigReal df2  = BigReal(df) * BIGREAL_HALF;
  const BigReal incG = lowerIncGammaTaylor(df2, x * BIGREAL_HALF, digits);
  const BigReal G    = gamma(df2, digits);
  return rQuot(incG, G, digits);
}

typedef FunctionTemplate<BigReal,BigReal> BigRealFunction;

class ChiHelpFunction : public BigRealFunction {
private:
  const UINT    m_df;
  const BigReal m_targetY;
public:
#define DIGITS 40

  ChiHelpFunction(UINT df) : m_df(df), m_targetY(BIGREAL_1 - BigReal(DBL_EPSILON)) {
  }
  BigReal operator()(const BigReal &x) {
    return rDif(BRchiSquaredDistribution(m_df,x, DIGITS), m_targetY, DIGITS);
  }
};

double findRoot(double x0, BigRealFunction &f) {
  BigReal x1 = x0;
  BigReal y1 = f(x1);
  if (y1.isZero()) {
    return x0;
  }
  int sign1 = sign(y1);
  bool found = false;
  BigReal x2 = x1, y2;
  int sign2;
  for(;;) {
    ++x2;
    y2 = f(x2);
    if(y2.isZero()) {
      return getDouble(x2);
    }
    if(sign(y2) == -sign1) {
      sign2 = -sign1;
      found = true;
      break;
    }
  }
  if(!found) {
    throwException(_T("%s:Cannot find start point"), __TFUNCTION__);
  }
  BigReal bestX, bestY;
  if(compareAbs(y1,y2) < 0) {
    bestX = x1; bestY = y1;
  } else {
    bestX = x2; bestY = y2;
  }
  for(int i = 0; i < 10; i++) {
    if(y2 == y1) break;
    const BigReal A  = rQuot(x2-x1,y2-y1,DIGITS);
    const BigReal xn = rDif(x1, y1 * A, DIGITS);
    const BigReal yn = f(xn);
    if(yn.isZero()) {
      return getDouble(xn);
    }
    if(compareAbs(yn,bestY) < 0) {
      bestX = xn; bestY = yn;
    }
    if(compareAbs(y1,y2) > 0) {
      x1 = xn; y1 = yn;
    } else {
      x2 = xn; y2 = yn;
    }
  }
  return getDouble(bestX);
}

static double findLowerX(UINT df) {
  ChiHelpFunction chih(df);
  static double x = 0;
  if(x == 0) x = df;
  const double root = findRoot(x, chih);
  x = root;
  return root;
}

double lowerXEstimate(double x) {
  double a = 9.30269;
  double b = 56.0846;
  return a*x/log(x) + b;
}

void makeLowerXPlot() {
  for(UINT df = 2; df < 10000; df++) {
    const double y  = findLowerX(df);
    const double ye = lowerXEstimate(df);
    printf("%d %le %le %le\n", df, y, ye, y/ye);
    fprintf(stderr, "%d %le %le %le\n", df, y, ye, y/ye);
  }
}

void gammaStirlingErrorPlot() {
  for (Real x = 2.1; x < 100; x += 0.1) {
    Real lg  = lnGamma(x);
    Real lsg = lnGammaStirling(x);
    Real q   = fabs((lsg - lg) / lg);
    _tprintf(_T("%s %s %s %s\n"), toString(x).cstr(), toString(lg).cstr(), toString(lsg).cstr(), toString(q).cstr());
  }
}

static void usage() {
  fprintf(stderr, "Usage:IncompleteGamma a\n");
  exit(-1);
}

void checkCast() {
  for (;;) {
    const double   x64  = inputDouble(_T("Enter double:"));
    long     l32  = (long  )x64;
    ULONG    ul32 = (ULONG )x64;
    INT64    l64  = (INT64 )x64;
    UINT64   ul64 = (UINT64)x64;
    _tprintf(_T("double64-> long32:%le -> %ld\n"  ), x64,  l32);
    _tprintf(_T("double64->ulong32:%le -> %lu\n"  ), x64, ul32);
    _tprintf(_T("double64-> long64:%le -> %I64d\n"), x64,  l64);
    _tprintf(_T("double64->ulong64:%le -> %I64u\n"), x64, ul64);
    const Double80 x80  = x64;
    l32  = getLong(  x80);
    ul32 = getUlong( x80);
    l64  = getInt64( x80);
    ul64 = getUint64(x80);
    _tprintf(_T("Double80-> long32:%s -> %ld\n"  ), toString(x80).cstr(),  l32);
    _tprintf(_T("Double80->ulong32:%s -> %lu\n"  ), toString(x80).cstr(), ul32);
    _tprintf(_T("Double80-> long64:%s -> %I64d\n"), toString(x64).cstr(),  l64);
    _tprintf(_T("Double80->ulong64:%s -> %I64u\n"), toString(x64).cstr(), ul64);
  }
}

int main(int argc, char **argv) {
  checkCast();
  gammaStirlingErrorPlot();
  return 0;
//  makeLowerXPlot();
//  return 0;

  int a;
/*
  for (double x = 0.9; x <= 30; x += 0.05) {
    printf("%le %le\n", x,C4(x));
  }
  return 0;
*/
  argv++;
  if(!*argv) usage();
  if (sscanf(*argv, "%d", &a) != 1) {
    usage();
  }
  for(Real x = 0.01; x < 25; x += 0.05) {
//    const Real c1 = chiSquaredDistribution(a,x);
//    const Real ct = chiSquaredDistributionTaylor(a,x);
//    printf("%le %le %le %le\n", getDouble(x), getDouble(c1), getDouble(ct), getDouble(c1-ct));

//    const Real lig1 = lowerIncGamma(a,x);
//    const Real ligt = lowerIncGammaTaylor(a,x);
//    printf("%le %le %le %le\n", getDouble(x), getDouble(lig1), getDouble(ligt), getDouble(lig1-ligt));

//    const Real chidens = chiSquaredDensity(a,x);
//    const Real dfa     = dfdx(a,x,chiSquaredDistributionApprox);
//    const Real df      = dfdx(a,x,chiSquaredDistribution);

//    printf("%le %le %le %le\n", getDouble(x), getDouble(chidens), getDouble(dfa), getDouble(df));
  }
  return 0;
}

