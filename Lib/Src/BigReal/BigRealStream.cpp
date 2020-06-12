#include "pch.h"
#include "BigRealStream.h"

#define addDecimalPoint(s)       { if(!decimalPointAdded) { s += _T("."); decimalPointAdded = true; } }

static TCHAR *digitToStr(TCHAR *dst, BRDigitType n, UINT width = 0) {
  TCHAR tmp[50], *d = width ? tmp : dst;
#if defined(IS64BIT)
  _i64tot(n, d, 10);
#else
  _itot(n, d, 10);
#endif // IS53BIT
  if(width) {
    const int zeroCount = (int)width - (int)_tcslen(tmp);
    if(zeroCount <= 0) {
      _tcscpy(dst, d);
    } else { // zeroCount > 0
      TMEMSET(dst, _T('0'), zeroCount);
      _tcscpy(dst + zeroCount, d);
    }
  }
  return dst;
}

void BigRealStream::formatFixed(String &dst, const BigReal &x, StreamSize precision, FormatFlags flags, bool removeTrailingZeroes) {
  bool decimalPointAdded = false;

  const BigReal nn(round(x, (intptr_t)precision));
  if(nn.isZero()) {
    StrStream::formatZero(dst, precision, flags);
    return;
  }

  const      Digit *digit        = nn.m_first;
  BRExpoType        d            = BigReal::getExpo10(nn);
  int               decimalsDone = 0;
  TCHAR             digStr[100];

  if(d < 0) { // first handle integerpart
    dst += _T('0');
  } else {
    dst += digitToStr(digStr, digit->n);
    d -= BigReal::getDecimalDigitCount(digit->n);
    for(digit = digit->next; digit != NULL && (d >= 0); d -= BIGREAL_LOG10BASE, digit = digit->next) {
      dst += digitToStr(digStr, digit->n, BIGREAL_LOG10BASE);
    }
    addZeroes(dst,d + 1);
    d = -1;
  }

  // now handle fraction if wanted
  if((flags & ios::showpoint) || precision > 0) {
    addDecimalPoint(dst);

    if(precision > 0) {
      for(int i = 0; (i - BIGREAL_LOG10BASE > d) && (decimalsDone < precision); i -= BIGREAL_LOG10BASE) {
        int      partLength = BIGREAL_LOG10BASE;
        intptr_t rest       = (intptr_t)(precision - decimalsDone);
        if(rest < BIGREAL_LOG10BASE) {
          partLength = (int)rest;
        }
        dst += digitToStr(digStr, 0, partLength);
        decimalsDone += partLength;
      }
      for(; digit && (decimalsDone < precision); digit = digit->next) {
        BRDigitType part       = digit->n;
        int         partLength = BIGREAL_LOG10BASE;
        intptr_t    rest       = (intptr_t)(precision - decimalsDone);
        if(rest < BIGREAL_LOG10BASE) {
          partLength = (int)rest;
          part /= BigReal::pow10(BIGREAL_LOG10BASE - (int)rest);
        }
        dst += digitToStr(digStr, part, partLength);
        decimalsDone += partLength;
      }
      if(!removeTrailingZeroes && (precision > decimalsDone)) {
        addZeroes(dst,precision - decimalsDone);
      }
      if(removeTrailingZeroes) {
        removeTralingZeroDigits(dst);
      }
    }
  }
}

void BigRealStream::formatScientific(String &dst, const BigReal &x, StreamSize precision, FormatFlags flags, BRExpoType expo10, bool removeTrailingZeroes) {
  bool decimalPointAdded = false;
  bool exponentCharAdded = false;

  const BigReal nn(round(x, (intptr_t)(precision - expo10)));
  const Digit *digit = nn.m_first;
  int          decimalsDone = 0;
  int          scale = BigReal::getDecimalDigitCount(digit->n) - 1;
  BRDigitType  scaleE10 = BigReal::pow10(scale);

  TCHAR digStr[100];
  dst += digitToStr(digStr, digit->n / scaleE10);
  if((flags & ios::showpoint) || precision > 0) {
    addDecimalPoint(dst);

    if(precision > 0) {
      BRDigitType fraction = digit->n % scaleE10;
      if(precision < scale) {
        fraction /= BigReal::pow10((int)(scale - precision));
        decimalsDone = (int)precision; // precision < scale < BIGREAL_LOG10BASE
      } else {
        decimalsDone = (int)scale;     // scale < BIGREAL_LOG10BASE
      }
      if(decimalsDone > 0) {           // decimalsDone < BIGREAL_LOG10BASE
        dst += digitToStr(digStr, fraction, decimalsDone);
      }
      for(digit = digit->next; digit != NULL && decimalsDone < precision; digit = digit->next) { // now handle tail
        BRDigitType part = digit->n;
        int         partLength = BIGREAL_LOG10BASE;
        intptr_t    rest = (intptr_t)(precision - decimalsDone);
        if(rest < BIGREAL_LOG10BASE) {
          partLength = (int)rest;
          part /= BigReal::pow10(BIGREAL_LOG10BASE - (int)rest);
        }
        dst += digitToStr(digStr, part, partLength);
        decimalsDone += partLength;
      }
      if(!removeTrailingZeroes && decimalsDone < precision) {
        addZeroes(dst, precision - decimalsDone);
      }
      if(removeTrailingZeroes) {
        removeTralingZeroDigits(dst);
      }
    }
  }
  addExponentChar(dst, flags);
#if defined(IS32BIT)
  dst += format(_T("%+03d"), BigReal::getExpo10(nn));
#else
  dst += format(_T("%+03lld"), BigReal::getExpo10(nn));
#endif
}

void BigRealStream::formatSeparateDigits(String &dst, const BigReal &x, TCHAR separatorChar) {
  if(x.isZero()) {
    dst += _T('0');
  } else {
    TCHAR digStr[100];
    bool decimalPointAdded = false;
    BRExpoType d = x.m_expo;
    if(d < 0) {
      dst += _T('0');
      addDecimalPoint(dst);
      digitToStr(digStr, 0, BIGREAL_LOG10BASE);
      for(int i = -1; i > d; i--) {
        dst += digStr;
        dst += separatorChar;
      }
      for(const Digit *digit = x.m_first; digit; digit = digit->next) {
        dst += digitToStr(digStr, digit->n, BIGREAL_LOG10BASE);
        dst += separatorChar;
      }
    } else {
      dst += digitToStr(digStr, x.m_first->n);
      dst += separatorChar;
      if(d-- == 0 && x.m_first->next) {
        addDecimalPoint(dst);
      }
      for(Digit *digit = x.m_first->next; digit; digit = digit->next) {
        dst += digitToStr(digStr, digit->n, BIGREAL_LOG10BASE);
        dst += separatorChar;
        if(d-- == 0 && digit->next) {
          addDecimalPoint(dst);
        }
      }
      digitToStr(digStr, 0, BIGREAL_LOG10BASE);
      for(; d >= 0; d--) {
        dst += digStr;
        dst += separatorChar;
      }
    }
    if(dst.last() == separatorChar) {
      dst.removeLast();
    }
  }
}

BigRealStream &BigRealStream::operator<<(const BigReal &x) {
  intptr_t    precision     = (intptr_t)this->precision();
  FormatFlags flg           = flags();
  const TCHAR separatorChar = separator();

  String result;
  if(!isfinite(x)) {
    TCHAR tmp[100];
    result = formatUndefined(tmp, _fpclass(x), (flg & ios::uppercase), true);
    flg &= ~ios::hexfloat;
  } else if(separatorChar != 0) {
    formatSeparateDigits(result, x, separatorChar);
  } else if(x.isZero()) {
    formatZero(result, precision, flg);
  } else { // x defined && x != 0
    switch(flg & ios::floatfield) {
    case ios::fixed : // Use fixed format
      formatFixed(result, x, precision, flg, false);
      break;
    case ios::scientific:  // Use scientific format
      formatScientific(result, x, precision, flg, BigReal::getExpo10(x), false);
      break;
    default:  // neither scientific nor fixed format (or both) are specified
      { BRExpoType expo10 = BigReal::getExpo10(x);
        if((expo10 < -4) || (expo10 > 14) || ((expo10 > 0) && (expo10 >= precision)) || (expo10 > precision)) {
          precision = max(0, precision - 1);
          formatScientific(result, x, precision, flg, expo10, (flg & ios::showpoint) == 0);
        } else {
          const intptr_t prec = (precision == 0) ? abs(expo10) : max(0, precision - expo10 - 1);
          formatFixed(result, x, prec, flg, ((flg & ios::showpoint) == 0) || precision <= 1);
        }
      }
      break;
    } // switch
  } // x defined && x != 0
  formatFilledFloatField(result, x.isNegative(), flg);
  return *this;
}
