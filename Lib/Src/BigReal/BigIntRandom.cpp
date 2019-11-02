#include "pch.h"
#include <Math/BigInt.h>

// Return uniform distributed random BigInt in [0..e(1,maxDigits)-1], digits generated with rnd.
// If digitPool == NULL, use DEFAULT_DIGITPOOL
// ex:maxDigits = 3:returned values in interval [0..999]
BigInt randBigInt(size_t maxDigits, RandomGenerator &rnd, DigitPool *digitPool) {
  return (BigInt&)randBigReal(maxDigits, rnd, digitPool).multPow10(maxDigits);
}

// Return uniform distributed random BigInt in [0..n-1], digits generated with rnd.
// If(!n._isnormal()) (ie, n== 0 of nan,+/-inf), nan will be returned
// If digitPool == NULL, use n.getDigitPool()
BigInt randBigInt(const BigInt &n, RandomGenerator &rnd, DigitPool *digitPool) {
  if(digitPool == NULL) digitPool = n.getDigitPool();
  if(!n._isnormal()) return BigInt(digitPool->nan());
  return randBigInt(BigReal::getExpo10(n)+1, rnd, digitPool) % n;
}

// Return uniform distributed random BigInt in [from;to], digits generated with rnd.
// If digitPool == NULL, use from.getDigitPool()
BigInt randBigInt(const BigInt &from, const BigInt &to, RandomGenerator &rnd, DigitPool *digitPool) {
  if(from > to) {
    throwBigRealInvalidArgumentException(__TFUNCTION__, _T("from > to"));
  }
  if(digitPool == NULL) digitPool = from.getDigitPool();
  if(!from._isfinite() || !to._isfinite()) return BigInt(digitPool->nan());
  const BigInt result = randBigInt(sum(dif(to, from, digitPool), BigReal::_1), rnd); // r in [0..to-from]
  return result + from;
}

