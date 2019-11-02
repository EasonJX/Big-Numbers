#include "pch.h"

// Return uniform distributed random rational in range [0;1[ using rnd to generate numerator and denominator
// The denominator (den) will be uniform distributed in the range [2; maxDenominator]
// The numerator will be uniform distributed in range [0; den[
// Assume maxDenominator >= 2
BigRational randBigRational(const BigInt &maxDenominator, RandomGenerator &rnd, DigitPool *digitPool) {
  if(digitPool == NULL) digitPool = maxDenominator.getDigitPool();
  const BigInt den = randBigInt(digitPool->_2(), maxDenominator, rnd, digitPool);
  const BigInt num = randBigInt(den, rnd, digitPool);
  return BigRational(num, den);
}

// Return uniform distributed random BigRational in range [from;to] (both inclusive)
// First make the the 3 products: n1 = from.num*to.den, n2 = to.num*from.den, d = from.den*to.den
// The returned rational will have the value rn/(d*f), where rn is a random BigInt in the range [n1*f..n2*f],
// where f is a random BigInt in range [1..maxScaleFactor]
// If maxScaleFactor <= 1, no scaling is done
BigRational randBigRational(const BigRational &from, const BigRational &to, const BigInt &maxScaleFactor, RandomGenerator &rnd, DigitPool *digitPool) {
  if(digitPool == NULL) digitPool = from.getDigitPool();
  BigInt minNum = prod(from.getNumerator()  , to.getDenominator()  , digitPool);
  BigInt maxNum = prod(to.getNumerator()    , from.getDenominator(), digitPool);
  BigInt den    = prod(from.getDenominator(), to.getDenominator()  , digitPool);
  if(maxScaleFactor > BigReal::_1) {
    const BigInt scaleFactor = randBigInt(digitPool->_1(), maxScaleFactor, rnd, digitPool);
    minNum *= scaleFactor; maxNum *= scaleFactor; den *= scaleFactor;
  }
  const BigInt num = randBigInt(minNum, maxNum, rnd, digitPool);
  return BigRational(num, den);
}
