#pragma once

#include "MyString.h"

class StreamParameters {
private:
  int m_precision, m_width, m_flags;
  TCHAR  *addModifier(      TCHAR *dst)                     const;
  TCHAR  *addWidth(         TCHAR *dst)                     const;
  TCHAR  *addPrecision(     TCHAR *dst)                     const;
  TCHAR  *addPrefix(        TCHAR *dst, bool withPrecision) const;
  TCHAR  *addIntSpecifier(  TCHAR *dst, bool isSigned)      const;
  TCHAR  *addFloatSpecifier(TCHAR *dst)                     const;
public:
  StreamParameters(int precision=6, int width=0, int flags=0);

  StreamParameters(const tostream &stream);
  friend tostream &operator<<(tostream &out, const StreamParameters &p);

  void setPrecision(int precision) { m_precision = precision; }
  void setWidth(    int width    ) { m_width     = width;     }
  void setFlags(    int flags    ) { m_flags     = flags;     }
  int  getPrecision() const        { return m_precision; }
  int  getWidth()     const        { return m_width;     }
  int  getFlags()     const        { return m_flags;     }

  String getStringFormat() const;
  String getCharFormat()   const;
  String getUCharFormat()  const;
  String getShortFormat()  const;
  String getUShortFormat() const;
  String getIntFormat()    const;
  String getUIntFormat()   const;
  String getLongFormat()   const;
  String getULongFormat()  const;
  String getInt64Format()  const;
  String getUInt64Format() const;
  String getFloatFormat()  const;
  String getDoubleFormat() const;
};

#define iparam(width) StreamParameters(0, width, std::ios::unitbuf)
#define dparam( prec) StreamParameters(prec, (prec)+8, std::ios::scientific | std::ios::showpos | std::ios::unitbuf)
#define udparam(prec) StreamParameters(prec, (prec)+7, std::ios::scientific | std::ios::unitbuf)
#define fparam( prec) StreamParameters(prec, (prec)+2, std::ios::fixed      | std::ios::showpos | std::ios::unitbuf)
#define ufparam(prec) StreamParameters(prec, (prec)+2, std::ios::fixed      | std::ios::unitbuf)
