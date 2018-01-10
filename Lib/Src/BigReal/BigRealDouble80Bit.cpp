#include "pch.h"
#include <ctype.h>

void BigReal::init(const Double80 &x) {
  DEFINEMETHODNAME;

  init();

  if (isNan(x)) {
    throwBigRealInvalidArgumentException(method, _T("Double80 is Nan"));
  }
  if(!x.isZero()) {
    const int expo2 = getExpo2(x) - 63;
    if(expo2 == 0) {
      init(getSignificand(x));
    } else {
      DigitPool *pool = getDigitPool();
      const BigReal significand(getSignificand(x), pool);
      const bool isConstPool = pool->getId() == CONST_DIGITPOOL_ID;
      if(isConstPool) ConstDigitPool::releaseInstance(); // unlock it or we will get a deadlock
      const BigReal &p2 = pow2(expo2, CONVERSION_POW2DIGITCOUNT);
      if(isConstPool) ConstDigitPool::requestInstance();
      shortProductNoZeroCheck(significand, p2, 5).rRound(22);
    }
    if(x.isNegative()) {
      m_negative = true;
    }
  }
}

Double80 getDouble80(const BigReal &x) {
  DEFINEMETHODNAME;

  if(x.isZero()) {
    return Double80::zero;
  }
  if(compareAbs(x,ConstBigReal::_dbl80_max) > 0) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_dbl80_max));
  }
  if(compareAbs(x,ConstBigReal::_dbl80_min) < 0) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_dbl80_min));
  }
  return x.getDouble80NoLimitCheck();
}

Double80 BigReal::getDouble80NoLimitCheck() const {
  static const int minExpo2 = 64-16382;
  static const int maxExpo2 = 0x3fff;

  DigitPool       *pool  = getDigitPool();
  const BRExpoType ee2   = getExpo2(*this);
  const BRExpoType expo2 = 64 - ee2;
  bool             e2Overflow;
  Double80         e2, e2x;
  BigReal          xi(pool);
  if(expo2 <= minExpo2) {
    e2 = Double80::pow2(minExpo2);
    e2Overflow = false;
    xi.shortProduct(::cut(*this,21), pow2(minExpo2, CONVERSION_POW2DIGITCOUNT), BIGREAL_ZEROEXPO);  // BigReal multiplication
  } else if(expo2 >= maxExpo2) {
    e2  = Double80::pow2(maxExpo2);
    e2x = Double80::pow2((int)expo2 - maxExpo2);
    e2Overflow = true;
    xi.shortProduct(::cut(*this,21), pow2((int)expo2, CONVERSION_POW2DIGITCOUNT), BIGREAL_ZEROEXPO);  // BigReal multiplication
  } else {
    e2 = Double80::pow2((int)expo2);
    e2Overflow = false;
    xi = round(xi.shortProduct(::cut(*this,22), pow2((int)expo2, CONVERSION_POW2DIGITCOUNT), -1));  // BigReal multiplication
  }
  const Digit *p = xi.m_first;
  if(p == NULL) {
    return Double80::zero;
  }
  Double80 result     = (INT64)p->n;
  int      digitCount = LOG10_BIGREALBASE;
  for(p = p->next; p && (digitCount < 24); p = p->next, digitCount += LOG10_BIGREALBASE) {
    result *= BIGREALBASE;
    result += (INT64)p->n;
  }
  const BRExpoType e = xi.m_expo * LOG10_BIGREALBASE - digitCount + LOG10_BIGREALBASE;
  if(e > 0) {
    result *= Double80::pow10((int)e);
  } else if(e < 0) {
    result /= Double80::pow10(-(int)e);
  }
  result /= e2;
  if(e2Overflow) {
    result /= e2x;
  }
  return isNegative() ? -result : result;
}
