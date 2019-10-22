#pragma once

#include <Unicode.h>
#include <StrStream.h>

using namespace std;

class BigRealStream : public StrStream { // Don't derive from standardclass strstream. Its slow!!! (at least in windows)
private:
  TCHAR m_separatorChar;
  static void formatFixed(         String &dst, const BigReal &x, StreamSize precision, FormatFlags flags, bool removeTrailingZeroes);
  static void formatScientific(    String &dst, const BigReal &x, StreamSize precision, FormatFlags flags, BRExpoType expo10, bool removeTrailingZeroes);
  static void formatSeparateDigits(String &dst, const BigReal &x, TCHAR separatorChar);
public:
  BigRealStream(StreamSize precision = 6, StreamSize width = 0, FormatFlags flags = 0, TCHAR separatorChar = 0)
    : StrStream(precision, width, flags)
    , m_separatorChar(separatorChar)
  {
  }
  BigRealStream(const StreamParameters &param, TCHAR separatorChar = 0)
    : StrStream(param)
    , m_separatorChar(separatorChar)
  {
  }
  BigRealStream(ostream &out, TCHAR separatorChar = 0)
    : StrStream(out)
    , m_separatorChar(separatorChar)
  {
  }
  BigRealStream(wostream &out, TCHAR separatorChar = 0)
    : StrStream(out)
    , m_separatorChar(separatorChar)
  {
  }

  inline TCHAR separator(TCHAR separatorChar) {
    const TCHAR old = m_separatorChar;  m_separatorChar = separatorChar; return old;
  }

  inline TCHAR separator() const {
    return m_separatorChar;
  }
  BigRealStream &operator<<(const BigReal &v);
};

template <class IStreamType, class CharType> IStreamType &getBigReal(IStreamType &in, BigReal &x) {
  IStreamScanner<IStreamType, CharType> scanner(in);
  String buf;
  if(DecFloatValueStreamScanner::getInstance().match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    x = BigReal(buf.cstr(), x.getDigitPool());
  } catch (...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
}

template <class OStreamType> OStreamType &putBigReal(OStreamType &out, const BigReal &x, TCHAR separatorChar=0) {
  BigRealStream buf(out, separatorChar);
  buf << x;
  out << (String&)buf;
  return out;
}

template <class IStreamType, class CharType> IStreamType &getBigInt(IStreamType &in, BigInt &n) {
  IStreamScanner<IStreamType, CharType> scanner(in);
  String buf;
  if(DecIntValueStreamScanner::getInstance().match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    n = BigInt(buf.cstr(), n.getDigitPool());
  } catch (...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
}

template <class OStreamType> OStreamType &putBigInt(OStreamType &out, const BigInt &x, TCHAR separatorChar = 0) {
  const StreamSize  oldprec = out.precision();
  const FormatFlags oldflags = out.flags();
  out.flags((oldflags | ios::fixed) & ~(ios::scientific|ios::showpoint));
  out.precision(0);
  putBigReal(out, x, separatorChar);
  out.flags(oldflags);
  out.precision(oldprec);
  return out;
}

template <class OStreamType> OStreamType &putFullFormatBigReal(OStreamType &out, const BigReal &x, TCHAR separatorChar = 0) {
  const StreamSize  oldprec  = out.precision();
  const FormatFlags oldflags = out.flags();
  if(isfinite(x)) {
    BRExpoType e = BigReal::getExpo10(x);
    intptr_t   autoprec;
    if(e < 0) {
      autoprec = x.getLength() * LOG10_BIGREALBASE;
      if(e % LOG10_BIGREALBASE == 0) {
        autoprec -= LOG10_BIGREALBASE;
      } else {
        autoprec -= -e % LOG10_BIGREALBASE;
      }
    } else {
      autoprec = (x.getLength() - 1) * LOG10_BIGREALBASE + (e % LOG10_BIGREALBASE);
    }

    if(x.getLength() > 1) {
      for(BRDigitType last = x.getLastDigit(); last % 10 == 0; last /= 10) autoprec--;
    }
    out.flags((oldflags | ios::scientific) & ~ios::fixed);
    out.precision(autoprec);
  }
  putBigReal(out, x, separatorChar);
  out.flags(    oldflags);
  out.precision(oldprec);
  return out;
}

template <class OStreamType> OStreamType &putBigRational(OStreamType &out, const BigRational &x, TCHAR separatorChar = 0) {
  DigitPool *pool = x.getDigitPool();
  if(!isfinite(x)) {
    char tmp[100];
    out << StrStream::formatUndefined(tmp, _fpclass(x), (out.flags() & ios::uppercase) != 0);
  } else if(x.isInteger()) {
    putBigInt(out, x.getNumerator(), separatorChar);
  } else {
    stringstream tmpstream;
    tmpstream.flags(out.flags());
    putBigInt(tmpstream, x.getNumerator(), separatorChar);
    tmpstream.flags(0);
    tmpstream << "/";
    tmpstream.flags(out.flags() & ~(ios::showpos));
    putBigInt(tmpstream, x.getDenominator(), separatorChar);
    out << tmpstream.str().c_str();
  }
  return out;
}

template <class IStreamType, class CharType> IStreamType &getBigRational(IStreamType &in, BigRational &x) {
  DigitPool        *pool = x.getDigitPool();
  const FormatFlags flg  = in.flags();
  BigInt            num(pool->_0()), den(pool->_1());
  in >> num;
  if(in) {
    in.flags(flg & ~ios::skipws);
    if(in.peek() == '/') {
      in.get();
      in >> den;
    }
    in.flags(flg);
  }
  if(in) {
    try {
      x = BigRational(num, den);
    } catch (...) {
      in.setstate(ios::failbit);
      in.flags(flg);
      throw;
    }
  }
  in.flags(flg);
  return in;
}
