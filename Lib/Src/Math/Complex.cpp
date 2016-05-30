#include "pch.h"
#include <StreamParameters.h>
#include <Math/Complex.h>

const Complex Complex::i    = Complex(0,1);
const Complex Complex::zero = Complex(0,0);
const Complex Complex::one  = Complex(1,0);

#define EATWHITE() while(_istspace(*s)) s++

void Complex::init(const _TUCHAR *s) {
  bool ok = true;
  const _TUCHAR *reStr, *imStr = (_TUCHAR*)_T("0");
  EATWHITE();
  if(*s == _T('(')) {
    s++;
    EATWHITE();
    if(s = parseReal(reStr = s)) EATWHITE(); else ok = false;
    if(*s == _T(','))  {
      s++;
      EATWHITE();
      if(s = parseReal(imStr = s)) EATWHITE(); else ok = false;
    }
    if(*s == _T(')')) s++;else ok = false;
  } else {
    if(!(s = parseReal(reStr = s))) ok = false;
  }
  if(ok) {
    re = ttor((TCHAR*)reStr);
    im = ttor((TCHAR*)imStr);
  }
}

Complex operator+(const Complex &lts, const Complex &rhs) {
  return Complex(lts.re + rhs.re,lts.im + rhs.im);
}

Complex operator-(const Complex &lts, const Complex &rhs) {
  return Complex(lts.re - rhs.re,lts.im - rhs.im);
}

Complex operator*(const Complex &lts, const Complex &rhs) {
  return Complex(lts.re * rhs.re - lts.im * rhs.im
                ,lts.re * rhs.im + lts.im * rhs.re
                );
}

Complex operator/(const Complex &lts, const Complex &rhs) {
  Real d = rhs.re * rhs.re + rhs.im * rhs.im;
  return Complex((lts.re * rhs.re + lts.im * rhs.im)/d
                ,(lts.im * rhs.re - lts.re * rhs.im)/d
                );
}

Complex &Complex::operator+=(const Complex &rhs) {
  re += rhs.re;
  im += rhs.im;
  return *this;
}

Complex &Complex::operator-=(const Complex &rhs) {
  re -= rhs.re;
  im -= rhs.im;
  return *this;
}

Complex &Complex::operator*=(const Complex &rhs) {
  *this = *this * rhs;
  return *this;
}

Complex &Complex::operator/=(const Complex &rhs) {
  *this = *this / rhs;
  return *this;
}

Complex  operator-(const Complex &c) {
  return Complex(-c.re,-c.im);
}

Complex conjugate(const Complex &c) {
  return Complex(c.re,-c.im);
}

Complex sqr(const Complex &c) {
  return c*c;
}

Real arg2(const Complex &c) {
  return c.re * c.re + c.im * c.im;
}

Real arg(const Complex &c) {
  return sqrt(arg2(c));
}

Real fabs(const Complex &c) {
  return arg(c);
}

bool operator==(const Complex &lts, const Complex &rhs) {
  return lts.re == rhs.re && lts.im == rhs.im;
}

bool operator!=(const Complex &lts, const Complex &rhs) {
  return lts.re != rhs.re || lts.im != rhs.im;
}

Complex exp(const Complex &c) {
  Real l = exp(c.re);
  return Complex(l * cos(c.im), l * sin(c.im));
}

Complex sqrt(const Complex &c) {
  Polar p(c);
  p.theta /= 2;
  p.r = sqrt(p.r);
  return Complex(p.r * cos(p.theta), p.r * sin(p.theta));
}

Complex log(const Complex &c) {
  Polar p(c);
  Real re = log(p.r);
  return Complex(re, p.theta);
}

Complex pow(const Complex &c, const Complex &p) {
  return exp(log(c) * p);
}

Complex root(const Complex &c, const Complex &r) {
  return exp(log(c) / r);
}

Complex sin(const Complex &c) {
  return Complex(exp(Complex::i*c) - exp(-Complex::i*c)) / Complex::i / 2;
}

Complex cos(const Complex &c) {
  return Complex(exp(Complex::i*c) + exp(-Complex::i*c)) / 2.0;
}

Complex tan(const Complex &c) {
  return Complex(sin(c) / cos(c));
}

class PolarAB {
public:
  Real A,B;
  PolarAB(const Complex &c);
};

PolarAB::PolarAB(const Complex &c) {
  const Real &a = c.re;
  Real b  = sqr(c.im);
  Real t1 = sqrt(sqr(a+1) + b) / 2;
  Real t2 = sqrt(sqr(a-1) + b) / 2;
  A = t1 + t2;
  B = t1 - t2;

//  cout << " " << A << endl;

  if(B > 1) {
    B = 1;
  } else if(B < -1) {
    B = -1;
  }
}

Complex asin(const Complex &c) {
  PolarAB T(c);
  Real re = asin(T.B);
  Real im = log(T.A + sqrt((sqr(T.A)-1)));
  return Complex(re, im);
}

Complex acos(const Complex &c) {
  PolarAB T(c);
  Real re = acos(T.B);
  Real im = -log(T.A + sqrt((sqr(T.A)-1)));
  return Complex(re, im);
}

Complex atan(const Complex &c) {
  const Real &a = c.re;
  const Real &b = c.im;
  Real a2 = sqr(a);
  return Complex( 
           atan2(2.0*a, 1.0 - a2 - b*b) / 2
          ,log((a2 + sqr(b+1)) / (a2 + sqr(b-1))) / 4
         );
}

void setToRandom(Complex &c) {
  setToRandom(c.re);
  setToRandom(c.im);
}

String toString(const Complex &c, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << c;
  return stream;
}

StrStream &operator<<(StrStream &stream, const Complex &c) {
  if(c.im == 0) {
    stream << c.re;
  } else {
    stream.append(_T("("));
    stream << c.re;
    stream.append(_T(","));
    stream << c.im;
    stream.append(_T(")"));
  }
  return stream;
}

#define peekChar(in,ch) { ch = in.peek(); if(ch == EOF) in >> ch; }

template <class IStreamType, class CharType> void eatWhite(IStreamType &in) {
  CharType ch;
  for(;;in >> ch) {
    peekChar(in, ch);
    if(!_istspace(ch)) {
      return;
    }
  }
}

template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, Complex &c) {
  /* inputformat for Complex; 
      re
      (re)
      (re,im)
  */

  if(in.ipfx(0)) {
    Real re = 0, im = 0;
    CharType ch = 0;
    eatWhite<IStreamType, CharType>(in);
    peekChar(in, ch);
    if(ch == _T('(')) {
      in >> ch;
      eatWhite<IStreamType, CharType>(in);
      in >> re;
      eatWhite<IStreamType, CharType>(in);
      peekChar(in, ch);
      if(ch == _T(','))  {
        in >> ch;
        eatWhite<IStreamType, CharType>(in);
        in >> im;
        eatWhite<IStreamType, CharType>(in);
      }
      peekChar(in, ch);
      if(ch == _T(')')) {
        in >> ch;
      } else {
        in.clear(std::ios::badbit);
      }
    } else {
      in >> re;
    }
    if(in) {
      c = Complex(re,im);
    }
    in.isfx();
  }
  return in;
}


template <class OStreamType> OStreamType &operator<<(OStreamType &out, const Complex &c) {
  if(out.opfx()) {
    StrStream stream(out);
    stream << c;
    out << stream.cstr();
    out.osfx();
  }
  return out;
}

tistream &operator>>(tistream &in, Complex &c) {
  return ::operator>> <tistream, TCHAR> (in, c);
}

tostream &operator<<(tostream &out, const Complex &c) {
  return ::operator<< <tostream>(out, c);
}
