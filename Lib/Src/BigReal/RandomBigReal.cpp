#include "pch.h"

#ifdef IS32BIT
#define nextDigit nextInt
#else
#define nextDigit nextInt64
#endif

// 0 <= random < 1; with the specified number of decimal digits
BigReal randBigReal(size_t length, RandomGenerator *rnd, DigitPool *digitPool) {
  DigitPool       *pool = digitPool ? digitPool : &DEFAULT_DIGITPOOL;
  if(length == 0) {
    return pool->_0();
  }
  BigReal        result(pool);
  const intptr_t wholeDigits = length/LOG10_BIGREALBASE;
  intptr_t       i;

  for(i = 0; i < wholeDigits; i++) {
    result.appendDigit(rnd->nextDigit(BIGREALBASE));
  }

  if(length % LOG10_BIGREALBASE) {
    const BRDigitType s = BigReal::pow10(length % LOG10_BIGREALBASE);
    result.appendDigit(rnd->nextDigit(s) * (BIGREALBASE / s));
    i++;
  }

  result.m_expo     = -1;
  result.m_low      = -i;
  result.setPositive();
  result.trimZeroes();
  return result;
}

// Return uniform distributed random BigReal between low (incl) and high (excl)
// with length decimal digits. If digitPool == NULL, use low.digitPool
BigReal randBigReal(const BigReal &low, const BigReal &high, size_t length, RandomGenerator *rnd, DigitPool *digitPool) {
  if(low >= high) {
    throwBigRealInvalidArgumentException(__TFUNCTION__, _T("low >= high"));
  }
  DigitPool *pool = digitPool ? digitPool : low.getDigitPool();
  BigReal    r    = randBigReal(length, rnd, pool);
  BigReal    f(e(pool->_1(), BigReal::getExpo10(low) - length));
  r = sum(prod(high-low, r, f, pool), low, f, pool);
  return (r.isZero() || (r.getDecimalDigits() <= length) || (length == 0)) ? r : r.rTrunc(length);
}
