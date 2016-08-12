#include "pch.h"
#include <limits.h>
#include <Math/Rational.h>

const TCHAR *thisFile              = _T(__FILE__);
DEFINECLASSNAME(Rational);

#define SAFEPROD(a,b) Rational::safeProd(a,b,__LINE__)

Rational::Rational() : m_numerator(0), m_denominator(1) {
}

Rational::Rational(const __int64 &numerator, const __int64 &denominator) {
  init(numerator, denominator);
}

Rational::Rational(const __int64 &numerator, int denominator) {
  init(numerator, denominator);
}

Rational::Rational(int numerator, int denominator) {
  init(numerator, denominator);
}

Rational::Rational(int numerator, const __int64 &denominator) {
  init(numerator, denominator);
}

Rational::Rational(const __int64 &n) : m_numerator(n), m_denominator(1) {
}

Rational::Rational(int n) : m_numerator(n), m_denominator(1) {
}

Rational::Rational(unsigned int n) : m_numerator(n), m_denominator(1) {
}

// This algorithm is borrowed from gimpzoommodel.c in LIBGIMP:
// http://svn.gnome.org/viewcvs/gimp/trunk/libgimpwidgets/gimpzoommodel.c
//
// See also http://www.virtualdub.org/blog/pivot/entry.php?id=81
// for a discussion of calculating continued fractions by convergeants.
Rational::Rational(double d, unsigned int maxND) {
  DEFINEMETHODNAME;
  static const TCHAR *invalidDoubleMsg = _T("Value %le cannot be contained in a Rational with maxND=%lu");

  bool   positive;
  double da;
  if(d > 0) {
    positive = true;
    da = d;
  } else if(d < 0) {
    positive = false;
    da = -d;
  } else {
    m_numerator   = 0;
    m_denominator = 1;
    return;
  }

  __int64 p0 = 1, q0 = 0;
  __int64 p1 = (__int64)floor(da);
  __int64 q1 = 1;
  __int64 p2, q2;

  double frac = da - p1;
  while(frac) {
    frac = 1.0 / frac;
    const double next = floor(frac);
    p2 = (__int64)(next * p1 + p0);
    q2 = (__int64)(next * q1 + q0);

    // Limit the numerator and denominator to be maxND or less
    if((p2 > maxND) || (q2 > maxND)) {
      break;
    }

    // remember the last two fractions
    p0 = p1; q0 = q1;
    p1 = p2; q1 = q2;

    frac -= next;
  }

  if((da > maxND) || (da < 1.0 / maxND)) { // hard upper and lower bounds for ratio
    throwInvalidArgumentException(method, invalidDoubleMsg, d, maxND);
  }

  assert(((double)p1 / q1 <= maxND) && ((double)q1 / p1 <= maxND));
  assert(findGCD(p1,q1) == 1);

  m_numerator   = positive ? p1 : -p1;
  m_denominator = q1;
}

Rational::Rational(const Double80 &d80, unsigned __int64 maxND) {
  DEFINEMETHODNAME;
  static const TCHAR *invalidDoubleMsg = _T("Value %s cannot be contained in a Rational with maxND=%I64u");

  bool   positive;
  Double80 da;
  if(d80.isPositive()) {
    positive = true;
    da = d80;
  } else if(d80.isNegative()) {
    positive = false;
    da = -d80;
  } else {
    m_numerator   = 0;
    m_denominator = 1;
    return;
  }

  unsigned __int64 p0 = 1, q0 = 0;
  unsigned __int64 p1 = getInt64(floor(da));
  unsigned __int64 q1 = 1;
  unsigned __int64 p2, q2;

  Double80 frac = da - p1;
  while(!frac.isZero()) {
    frac = Double80::one / frac;
    const Double80 next = floor(frac);
    p2 = getInt64(next * p1 + p0);
    q2 = getInt64(next * q1 + q0);

    // Limit the numerator and denominator to be maxND or less
    if((p2 > maxND) || (q2 > maxND)) {
      break;
    }

    // remember the last two fractions
    p0 = p1; q0 = q1;
    p1 = p2; q1 = q2;

    frac -= next;
  }

  if((da > maxND) || (da < Double80::one / maxND)) { // hard upper and lower bounds for ratio
    throwInvalidArgumentException(method, invalidDoubleMsg, d80.toString().cstr(), maxND);
  }

  assert(((Double80)p1 / q1 <= maxND) && ((Double80)q1 / p1 <= maxND));
  assert(findGCD(p1,q1) == 1);

  m_numerator   = positive ? p1 : -(__int64)p1;
  m_denominator = q1;
}

Rational::Rational(const String &s) {
  init(s);
}

Rational::Rational(const TCHAR *s) {
  init(s);
}

#ifdef UNICODE
Rational::Rational(const char *s) {
  init(s);
}
#endif

void Rational::init(const String &s) {
  DEFINEMETHODNAME;
  String tmp(s);
  const int slash = (int)tmp.find(_T('/'));
  if(slash == 0) {
    throwInvalidArgumentException(method, _T("s=%s"), s.cstr());
  }
  if(slash < 0) {
    if(_stscanf(s.cstr(), _T("%I64d"), &m_numerator) != 1) {
      throwInvalidArgumentException(method, _T("s=%s"), s.cstr());
    }
    m_denominator = 1;
  } else {
    __int64 n, d;
    if((_stscanf(s.cstr()        , _T("%I64d"), &n) != 1)
    || (_stscanf(s.cstr()+slash+1, _T("%I64d"), &d) != 1)) {
      throwInvalidArgumentException(method, _T("s=%s"), s.cstr());
    }
    init(n, d);
  }
}

void Rational::init(const __int64 &numerator, const __int64 &denominator) {
  if(denominator == 0) {
    throwInvalidArgumentException(_T("init"), _T("Denominator is zero"));
  } else if((numerator < -_I64_MAX) || (numerator   > _I64_MAX)) {
    throwInvalidArgumentException(_T("init"), _T("Numerator(=%I64d) out of range [%I64d..%I64d]"), numerator, -_I64_MAX, _I64_MAX);
  } else if((denominator < -_I64_MAX) || (denominator > _I64_MAX)) {
    throwInvalidArgumentException(_T("init"), _T("Denominator(=%I64d) out of range [%I64d..%I64d]"), denominator, -_I64_MAX, _I64_MAX);
  }

  if(numerator == 0) { // zero always 0/1
    m_numerator   = 0;
    m_denominator = 1;
  } else {
    const __int64 gcd = findGCD(__int64(abs(numerator)),__int64(abs(denominator)));
    m_numerator   = numerator   / gcd;
    m_denominator = denominator / gcd;
    if(denominator < 0) { // Negative numbers are represented with negative numerator and positive denominator
      m_numerator   = -m_numerator;
      m_denominator = -m_denominator;
    }
  }
}

Rational operator+(const Rational &l, const Rational &r) {
  if(l.m_denominator == r.m_denominator) {                                                     // l.d == r.d. just add l.n and r.n
    return Rational(l.m_numerator + r.m_numerator, l.m_denominator);
  } else if((l.m_denominator > r.m_denominator) && (l.m_denominator % r.m_denominator == 0)) { // l.d = n * r.d. extend r with n and use l.d as denominator
    const __int64 n = l.m_denominator / r.m_denominator;
    return Rational(l.m_numerator + SAFEPROD(n,r.m_numerator), l.m_denominator);
  } else if((l.m_denominator < r.m_denominator) && (r.m_denominator % l.m_denominator == 0)) { // r.d = n * l.d. extend l with n and use r.d as denominator
    const __int64 n = r.m_denominator / l.m_denominator;
    return Rational(SAFEPROD(n,l.m_numerator) + r.m_numerator, r.m_denominator);
  } else {                                                                                     // Extend both and use l.d * r.d as denominator
    return Rational(SAFEPROD(l.m_numerator,r.m_denominator) + SAFEPROD(r.m_numerator,l.m_denominator), SAFEPROD(l.m_denominator,r.m_denominator));
  }
}

Rational operator-(const Rational &l, const Rational &r) {
  if(l.m_denominator == r.m_denominator) {                                                     // l.d == r.d. just subtract r.n from l.n
    return Rational(l.m_numerator - r.m_numerator, l.m_denominator);
  } else if((l.m_denominator > r.m_denominator) && (l.m_denominator % r.m_denominator == 0)) { // l.d = n * r.d. extend r with n and use l.d as denominator
    const __int64 n = l.m_denominator / r.m_denominator;
    return Rational(l.m_numerator - SAFEPROD(n,r.m_numerator), l.m_denominator);
  } else if((l.m_denominator < r.m_denominator) && (r.m_denominator % l.m_denominator == 0)) { // r.d = n * l.d. extend l with n and use r.d as denominator
    const __int64 n = r.m_denominator / l.m_denominator;
    return Rational(SAFEPROD(n,l.m_numerator) - r.m_numerator, r.m_denominator);
  } else {                                                                                     // Extend both and use l.d * r.d as denominator
    return Rational(SAFEPROD(l.m_numerator,r.m_denominator) - SAFEPROD(r.m_numerator,l.m_denominator), SAFEPROD(l.m_denominator,r.m_denominator));
  }
}

Rational operator-(const Rational &r) {
  return Rational(-r.m_numerator, r.m_denominator);
}

Rational operator*(const Rational &l, const Rational &r) {
  return Rational(SAFEPROD(l.m_numerator,r.m_numerator), SAFEPROD(l.m_denominator,r.m_denominator));
}

void Rational::throwDivisionbyZeroException(const TCHAR *method) { // static
  throwInvalidArgumentException(method, _T("Division by zero"));
}

Rational operator/(const Rational &l, const Rational &r) {
  if(r.isZero()) {
    Rational::throwDivisionbyZeroException(_T("operator/"));
  }
  return Rational(SAFEPROD(l.m_numerator,r.m_denominator), SAFEPROD(l.m_denominator,r.m_numerator));
}

Rational operator%(const Rational &l, const Rational &r) {
  if(r.isZero()) {
    Rational::throwDivisionbyZeroException(_T("operator%"));
  }
  const Rational q = l / r;
  const __int64  n = getInt64(q);
  return l - r * n;
}

Rational &Rational::operator*=(const Rational &r) {
  return *this = Rational(SAFEPROD(m_numerator,r.m_numerator), SAFEPROD(m_denominator, r.m_denominator));
}

Rational &Rational::operator/=(const Rational &r) {
  if(r.isZero()) {
    Rational::throwDivisionbyZeroException(_T("operator/="));
  }
  return *this = Rational(SAFEPROD(m_numerator,r.m_denominator), SAFEPROD(m_denominator, r.m_numerator));
}

Rational &Rational::operator%=(const Rational &r) {
  if(r.isZero()) {
    Rational::throwDivisionbyZeroException(_T("operator%="));
  }
  return *this = *this % r;
}

Rational fabs(const Rational &r) {
  Rational result;
  result.m_numerator   = abs(r.m_numerator);
  result.m_denominator = r.m_denominator;
  return result;
}

#define EVEN(n) (((n)&0x1) == 0)

__int64 Rational::pow(__int64 n, int y) { // static assume y >= 0
  __int64 p = 1;
  while(y > 0) {
    if(EVEN(y)) {
      n = SAFEPROD(n,n);
      y /= 2;
    } else {
      p = SAFEPROD(p,n);
      y--;
    }
  }
  return p;
}

Rational pow(const Rational &r, int e) {
  DEFINEMETHODNAME;
  if(e == 0) {
    return Rational(1);
  } else if(e < 0) {
    if(r.isZero()) {
      throwMethodInvalidArgumentException(Rational::s_className, method, _T("r == 0, e=%d"), e);
    }
    return Rational(Rational::pow(r.m_denominator,-e), Rational::pow(r.m_numerator,-e));
  } else {
    return Rational(Rational::pow(r.m_numerator,e), Rational::pow(r.m_denominator,e));
  }
}

Rational reciprocal(const Rational &r) {
  if(r.isZero()) {
    Rational::throwDivisionbyZeroException(_T("reciprocal"));
  }
  return Rational(r.m_denominator, r.m_numerator);
}

int rationalCmp(const Rational &r1, const Rational &r2) {
  const int sign1 = sign(r1.m_numerator);
  const int c     = sign1 - sign(r2.m_numerator);
  if(c != 0) return c;
  if(abs(r1.m_numerator) > (UINT)UINT_MAX
  || abs(r2.m_numerator) > (UINT)UINT_MAX
  || (unsigned __int64)r1.m_denominator > (UINT)UINT_MAX
  || (unsigned __int64)r2.m_denominator > (UINT)UINT_MAX) {
    return sign(getDouble(r1) - getDouble(r2));
  } else {
    const unsigned __int64 p1 = (unsigned __int64)abs(r1.m_numerator) * r2.m_denominator;
    const unsigned __int64 p2 = (unsigned __int64)abs(r2.m_numerator) * r1.m_denominator;
    return (p1 > p2) ? sign1 : (p1 < p2) ? -sign1 : 0;
  }
}

bool Rational::operator<( const Rational &r) const {
  return rationalCmp(*this, r) < 0;
}

bool Rational::operator>( const Rational &r) const {
  return rationalCmp(*this, r) > 0;
}

bool Rational::operator<=(const Rational &r) const {
  return rationalCmp(*this, r) <= 0;
}

bool Rational::operator>=(const Rational &r) const {
  return rationalCmp(*this, r) >= 0;
}

bool Rational::operator==(const Rational &r) const {
  return (m_numerator == r.m_numerator) && (m_denominator == r.m_denominator);
}

bool Rational::operator!=(const Rational &r) const {
  return !(*this == r);
}

long getLong(const Rational &r) {
  DEFINEMETHODNAME;
  const __int64 v = getInt64(r);
  if((v < _I32_MIN) || (v > _I32_MAX)) {
    throwMethodException(Rational::s_className, method, _T("Value (=%I64d) out of range. Legal range is [%d..%d]"), v, _I32_MIN, _I32_MAX);
  }
  return (long)v;
}

unsigned long getUlong(const Rational &r) {
  DEFINEMETHODNAME;
  if(r.isNegative()) {
    throwMethodException(Rational::s_className, method, _T("Value is negative(=%s)"), r.toString().cstr());
  }
  const unsigned __int64 v = getUint64(r);
  if(v > _UI32_MAX) {
    throwMethodException(Rational::s_className, method, _T("OverFlow. Rational=%I64u, _UI32_MAX=%lu"), v, _UI32_MAX);
  }
  return (unsigned long)v;
}

__int64 getInt64(const Rational &r) {
  return r.m_numerator / r.m_denominator;
}

unsigned __int64 getUint64(const Rational &r) {
  DEFINEMETHODNAME;
  if(r.isNegative()) {
    throwMethodException(Rational::s_className, method, _T("Value is negative (=%s)"), r.toString().cstr());
  }
  return r.m_numerator / r.m_denominator;
}

__int64 Rational::safeProd(const __int64 &a, const __int64 &b, int line) { // static
  DEFINEMETHODNAME;
  if(a == 0 || b == 0) return 0;
  const int sa = sign(a), sb = sign(b);
  const int expectedSign = sa * sb;
  __int64 result = a * b;
  const int sr = sign(result);
  if(sr != expectedSign) {
    throwException(method, _T("%s line %d: Product overflow. a=%I64d, b=%I64d"), thisFile, line, a, b);
  }
  return result;
}

__int64 Rational::findGCD(const __int64 &a, const __int64 &b) { // static
  DEFINEMETHODNAME;
  if(a <= 0 || b <= 0) {
    throwInvalidArgumentException(method, _T("a=%I64d, b=%I64d"), a, b);
  }
  __int64 g = 1;
  __int64 u = a;
  __int64 v(b);

  while(EVEN(u) && EVEN(v)) {
    u /= 2;
    v /= 2;
    g *= 2;
  }

  // Now u or v (or both) are odd
  while(u > 0) {
    if(EVEN(u)) {
      u /= 2;
    } else if(EVEN(v)) {
      v /= 2;
    } else if(u < v) {
      v = (v-u)/2;
    } else {
      u = (u-v)/2;
    }
  }
  return g*v;
}

bool Rational::isRealRational(const Real &x, Rational *r) { // static
  if(x == floor(x)) {
    if(r) *r = getInt(x);
    return true;
  }
  const Rational tmp(x);
  if(tmp.getDenominator() <= 400) { // This is not good enough, but how should it be done ?
    if(r) *r = tmp;
    return true;
  } else {
    return false;
  }
}

String Rational::toString() const {
  if(m_denominator == 1) {
    return format(_T("%I64d"), m_numerator);
  } else {
    return format(_T("%I64d/%I64d"), m_numerator, m_denominator);
  }
}
