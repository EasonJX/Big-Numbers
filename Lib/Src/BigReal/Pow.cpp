#include "pch.h"

class PowConstants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;
  const ConstBigReal c3;

  PowConstants()
    :c1( 0.4)
    ,c2( 0.001)
    ,c3( 0.4)
  {
  }
};

static const PowConstants POWC;

BigReal pow(const BigReal &x, const BigReal &y, const BigReal &f) { // x^y
  VALIDATETOLERANCE(f)

  DigitPool *pool = x.getDigitPool();
#define _0 pool->_0()
#define _1 pool->_1()

  if(y.isZero()) {
    return x.isZero() ? pool->nan() : _1;
  }
  if(y == _1) {
    return x;
  }
  if(x.isZero()) {
    return y.isNegative() ? pool->pinf() : _0;
  }
  if(x.isNegative()) {
    if(isEven(y)) {
      return pow(-x,y,f);
    }
    if(isOdd(y)) {
      return -pow(-x,y,f);
    }
    return pool->nan();
  }

  BigReal a = (x > _1) ? APCpow(>,x, floor(y)+_1,pool) : APCpow(>,x,floor(y),pool);

  if(a > f) {
    const BigReal u = APCquot(<,f,a,pool);
    return exp(prod(y,ln(x, APCquot(<,APCprod(<,POWC.c1,u,pool),fabs(y),pool)),APCprod(<,POWC.c2,u,pool),pool),APCprod(<,POWC.c3,f,pool));
  } else {
    return _0;
  }
}
