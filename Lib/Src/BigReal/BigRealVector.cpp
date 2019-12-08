#include "pch.h"
#include <Math/BigRealMatrix.h>

// Implementation of basic BigRealVector-operations

DEFINECLASSNAME(BigRealVector);

void BigRealVector::checkPrecision(UINT digits) {
  if(digits == 0) {
    throwBigRealException(_T("%s:Precision = 0 not allowed"), s_className);
  }
}

void BigRealVector::invalidDimensionError(const TCHAR *method, const BigRealVector &rhs) const {
  throwBigRealException(_T("%s::%s:Invalid dimension. Left.%s, Right.%s")
                       ,s_className, method, getDimensionString().cstr(), rhs.getDimensionString().cstr());
}

BigRealVector::BigRealVector(size_t dim, UINT digits) : VectorTemplate<BigReal>(dim), m_digits(digits) {
  checkPrecision(m_digits);
}

BigRealVector::BigRealVector(const BigRealVector &v) : VectorTemplate<BigReal>(v), m_digits(v.m_digits) {
  checkPrecision(m_digits);
}

BigRealVector::BigRealVector(const VectorTemplate<BigReal> &v, UINT digits)
  : VectorTemplate<BigReal>(v), m_digits(digits)
{
  checkPrecision(m_digits);
}

UINT BigRealVector::setPrecision(UINT digits) {
  checkPrecision(digits);
  const UINT oldDigits = m_digits;
  m_digits = digits;
  return oldDigits;
}

BigRealVector operator*(const BigReal &d, const BigRealVector &rhs) {
  const size_t n      = rhs.getDimension();
  const UINT   digits = rhs.getPrecision();

  BigRealVector result(n, digits);
  for(size_t i = 0; i < n; i++) {
    result[i] = rProd(rhs[i], d, digits);
  }
  return result;
}

BigRealVector operator*(const BigRealVector &lts, const BigReal &d ) {
  return d * lts;
}

BigRealVector operator/(const BigRealVector &lts, const BigReal &d) {
  const size_t n      = lts.getDimension();
  const UINT   digits = lts.getPrecision();

  BigRealVector result(n, digits);
  for(size_t i = 0; i < n; i++) {
    result[i] = rQuot(lts[i], d, digits);
  }
  return result;
}

BigReal operator*(const BigRealVector &lts, const BigRealVector& rhs) {
  const size_t n      = lts.getDimension();
  const UINT   digits = min(lts.getPrecision(), rhs.getPrecision());

  if(n != rhs.getDimension()) lts.invalidDimensionError(__TFUNCTION__, rhs);

  BigReal sum = 0;
  for(size_t i = 0; i < n; i++) {
    sum = rSum(sum, rProd(lts[i], rhs[i], digits), digits);
  }
  return sum;
}

BigRealVector operator+(const BigRealVector& lts, const BigRealVector& rhs) {
  const size_t n      = lts.getDimension();
  const UINT   digits = min(lts.getPrecision(), rhs.getPrecision());

  if(n != rhs.getDimension()) lts.invalidDimensionError(__TFUNCTION__, rhs);

  BigRealVector result(n, digits);
  for(size_t i = 0; i < n; i++) {
    result[i] = rSum(lts[i], rhs[i], digits);
  }
  return result;
}

BigRealVector operator-(const BigRealVector& lts, const BigRealVector& rhs) {
  const size_t n      = lts.getDimension();
  const UINT   digits = min(lts.getPrecision(), rhs.getPrecision());

  if(n != rhs.getDimension()) lts.invalidDimensionError(__TFUNCTION__, rhs);

  BigRealVector result(n, digits);
  for(size_t i = 0; i < n; i++) {
    result[i] = rDif(lts[i], rhs[i], digits);
  }
  return result;
}

BigRealVector &BigRealVector::operator*=(const BigReal &d) {
  const size_t n = getDimension();
  for(size_t i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rProd(v, d, m_digits);
  }
  return *this;
}

BigRealVector &BigRealVector::operator/=(const BigReal &d) {
  const size_t n = getDimension();
  for(size_t i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rQuot(v, d, m_digits);
  }
  return *this;
}

BigRealVector &BigRealVector::operator+=(const BigRealVector &rhs) {
  const size_t n      = getDimension();
  const UINT   digits = min(m_digits, rhs.getPrecision());

  if(n != rhs.getDimension()) invalidDimensionError(__TFUNCTION__, rhs);

  for(size_t i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rSum(v, rhs[i], digits);
  }
  setPrecision(digits);
  return *this;
}

BigRealVector &BigRealVector::operator-=(const BigRealVector &rhs) {
  const size_t n      = getDimension();
  const UINT   digits = min(m_digits, rhs.getPrecision());

  if(n != rhs.getDimension()) invalidDimensionError(__TFUNCTION__, rhs);

  for(size_t i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rDif(v, rhs[i], digits);
  }
  setPrecision(digits);
  return *this;
}

BigReal BigRealVector::length() const {
  const size_t n   = getDimension();
  BigReal      sum = 0;
  for(size_t i = 0; i < n; i++) {
    const BigReal &v = (*this)[i];
    sum = rSum(sum, rProd(v, v, m_digits), m_digits);
  }
  return rSqrt(sum, m_digits);
}
