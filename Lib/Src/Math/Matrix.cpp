#include "pch.h"
#include <Math/Matrix.h>

void setToRandom(Vector &v, RandomGenerator *rnd) {
  for(size_t i = 0; i < v.getDimension(); i++) {
    setToRandom(v[i], rnd);
  }
}

void setToRandom(Matrix &a, RandomGenerator *rnd) {
  for(size_t r = 0; r < a.getRowCount(); r++) {
    for(size_t c = 0; c < a.getColumnCount(); c++) {
      setToRandom(a(r,c), rnd);
    }
  }
}

Real fabs(const Vector &v) {
  return v.length();
}

Real fabs(const Matrix &a) {
  return normf(a);
}

// Frobenius norm
Real normf(const Matrix &a) {
  Real sum = 0;
  for(size_t r = 0; r < a.getRowCount(); r++) {
    for(size_t c = 0; c < a.getColumnCount(); c++) {
      sum += sqr(a(r,c));
    }
  }
  return sqrt(sum);
}

Real norm1(const Matrix &a) {
  Real m = 0;
  for(size_t c = 0; c < a.getColumnCount(); c++) {
    Real sum = 0;
    for(size_t r = 0; r < a.getRowCount(); r++) {
      sum += fabs(a(r,c));
    }
    if(sum > m) {
      m = sum;
    }
  }
  return m;
}

Real norm00(const Matrix &a) {
  Real m = 0;
  for(size_t r = 0; r < a.getRowCount(); r++) {
    Real sum = 0;
    for(size_t c = 0; c < a.getColumnCount(); c++) {
      sum += fabs(a(r,c));
    }
    if(sum > m) {
      m = sum;
    }
  }
  return m;
}

Matrix inverse(const Matrix &a) {
  _VALIDATEISSQUAREMATRIX(a);
  LUMatrix lu(a);
  return lu.getInverse();
}

ComplexVector findEigenValues(const Matrix &a, QRTracer *tracer) {
  QRMatrix QR(a,tracer);
  return QR.getEigenValues();
}

Real det(const Matrix &a) {
  try {
    LUMatrix lu(a);
    return lu.getDeterminant();
  } catch(MathException) {
    return 0;
  }
}
