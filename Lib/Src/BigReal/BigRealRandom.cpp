#include "pch.h"

#ifdef IS32BIT
#define nextDigit nextInt
#else
#define nextDigit nextInt64
#endif

// 0 <= random < 1; with at most maxDigits, decimal digits
BigReal randBigReal(size_t maxDigits, RandomGenerator &rnd, DigitPool *digitPool) {
  if(digitPool == NULL) digitPool = DEFAULT_DIGITPOOL;
  if(maxDigits == 0) {
    return digitPool->_0();
  }
  BigReal        result(digitPool);
  const intptr_t wholeDigits = maxDigits/BIGREAL_LOG10BASE;
  intptr_t       i;

  for(i = 0; i < wholeDigits; i++) {
    result.appendDigit(rnd.nextDigit(BIGREALBASE));
  }

  if(maxDigits % BIGREAL_LOG10BASE) {
    const BRDigitType s = BigReal::pow10(maxDigits % BIGREAL_LOG10BASE);
    result.appendDigit(rnd.nextDigit(s) * (BIGREALBASE / s));
    i++;
  }

  result.m_expo     = -1;
  result.m_low      = -i;
  result.setPositive();
  return result.trimZeroes();
}

// Return uniform distributed random BigReal in [from;to] with at most maxDigits decimal digits.
// If digitPool == NULL, use from.digitPool
BigReal randBigReal(const BigReal &from, const BigReal &to, size_t maxDigits, RandomGenerator &rnd, DigitPool *digitPool) {
  if(from > to) {
    throwBigRealInvalidArgumentException(__TFUNCTION__, _T("from > to"));
  }
  if(digitPool == NULL) digitPool = from.getDigitPool();
  if(!from._isfinite() || !to._isfinite()) return digitPool->nan();

  BigReal r = randBigReal(maxDigits, rnd, digitPool);
  r.clrInitDone();
  r = rSum(rProd(r, rDif(to,from,maxDigits, digitPool),maxDigits), from, maxDigits);
  return (r.isZero() || (r.getDecimalDigits() <= maxDigits) || (maxDigits == 0)) ? r : r.rTrunc(maxDigits).setInitDone();
}
