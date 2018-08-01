#pragma once

#include <MyString.h>
#include "Double80.h"
#include "Rational.h"

typedef enum {
  NUMBERTYPE_UNDEFINED
 ,NUMBERTYPE_FLOAT
 ,NUMBERTYPE_DOUBLE
 ,NUMBERTYPE_DOUBLE80
 ,NUMBERTYPE_RATIONAL
} NumberType;

#ifdef LONGDOUBLE
#define NUMBERTYPE_REAL NUMBERTYPE_DOUBLE80
#else
#define NUMBERTYPE_REAL NUMBERTYPE_DOUBLE
#endif

class Number {
private:
  NumberType   m_type;
  union {
    float     *m_flt;
    double    *m_d64;
    Double80  *m_d80;
    Rational  *m_rational;
  };

  // Must be private
  void setType(NumberType type);
  void cleanup();
  static void throwTypeIsUndefinedException(const TCHAR *method);
  static void throwUnknownTypeException(    const TCHAR *method, NumberType type);
         void throwUnknownTypeException(    const TCHAR *method) const;
public:
  Number();
  Number(const Number   &v);
  Number(int             v);
  Number(UINT            v);
  Number(const float    &v);
  Number(const double   &v);
  Number(const Double80 &v);
  Number(const Rational &v);

  virtual ~Number();

  Number &operator=(const Number   &v);
  Number &operator=(int             v);
  Number &operator=(UINT            v);
  Number &operator=(const float    &v);
  Number &operator=(const double   &v);
  Number &operator=(const Double80 &v);
  Number &operator=(const Rational &v);

  inline NumberType getType() const {
    return m_type;
  }
  static String getTypeName(NumberType nt);
  String getTypeName() const {
    return getTypeName(getType());
  }

  inline bool isUndefined() const {
    return m_type == NUMBERTYPE_UNDEFINED;
  }

  inline bool isInteger() const {
    return isRational() && m_rational->isInteger();
  }
  inline bool isRational() const {
    return getType() == NUMBERTYPE_RATIONAL;
  }

  friend inline bool isInteger(const Number &n) {
    return n.isRational() && n.m_rational->isInteger();
  }
  friend inline bool isRational(const Number &n) {
    return n.getType() == NUMBERTYPE_RATIONAL;
  }
  friend inline bool isInt(const Number &n) {
    return n.isRational() && isInt(*n.m_rational);
  }
  friend inline bool isEven(const Number &n) {
    return n.isRational() && isEven(*n.m_rational);
  }
  friend inline bool isOdd(const Number &n) {
    return n.isRational() && isOdd(*n.m_rational);
  }

  friend int      getInt(     const Number &n);
  friend float    getFloat(   const Number &n);
  friend double   getDouble(  const Number &n);
  friend Double80 getDouble80(const Number &n);
  friend Rational getRational(const Number &n);
  friend Number operator+( const Number &n1, const Number &n2);
  friend Number operator-( const Number &n1, const Number &n2);
  friend Number operator-( const Number &v);
  friend Number operator*( const Number &n1, const Number &n2);
  friend Number operator/( const Number &n1, const Number &n2);
  friend Number operator%( const Number &n1, const Number &n2);
  friend Number reciprocal(const Number &n);
  friend Number pow(       const Number &n1, const Number &n2);

  friend int numberCmp(const Number &n1, const Number &n2);

  inline bool operator< (const Number &n) const { return numberCmp(*this, n) <  0; }
  inline bool operator> (const Number &n) const { return numberCmp(*this, n) >  0; }
  inline bool operator<=(const Number &n) const { return numberCmp(*this, n) <= 0; }
  inline bool operator>=(const Number &n) const { return numberCmp(*this, n) >= 0; }
  bool operator==(const Number &n) const;
  inline bool operator!=(const Number &n) const {
    return !(*this == n);
  }

  String toString() const;
};

inline Real getReal( const Number &n) {
#ifdef LONGDOUBLE
  return getDouble80(n);
#else
  return getDouble(n);
#endif
}

Number strton(const char    *s, char    **end);
Number wcston(const wchar_t *s, wchar_t **end);

#ifdef _UNICODE
#define _tcston wcston
#else
#define _tcston strton
#endif // _UNICODE
