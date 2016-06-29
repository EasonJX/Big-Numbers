#include "pch.h"

DEFINECLASSNAME(BigReal);

void BigReal::insertAfter(Digit *q, BRDigitType n) {
//  assert(n < BIGREALBASE);
  Digit *p = newDigit();
  p->n = n;
  p->prev = q;
  p->next = q->next;
  if(q->next) {
    q->next->prev = p;
  } else {
    m_last = p;
  }
  q->next = p;
}

void BigReal::insertDigit(BRDigitType n) {
//  assert(n < BIGREALBASE);
  Digit *p = newDigit();
  p->n = n;
  p->next = m_first;
  p->prev = NULL;
  if(m_last == NULL) {
    m_last = p;
  } else {
    m_first->prev = p;
  }
  m_first = p;
}

//#define COUNT_CALLS

#include <CallCounter.h>

#ifdef COUNT_CALLS
#define DECLARE_CALLCOUNTER(name) static CallCounter _callCounter(name);
#define COUNTKEYCALL(n)                              _callCounter.incr(n)
#define COUNTCALL()                                  _callCounter.incr()
#else
#define DECLARE_CALLCOUNTER(name)
#define COUNTKEYCALL(n)
#define COUNTCALL()
#endif

// Assume *this != zero. ie m_first != NULL (and m_last != NULL)
void BigReal::insertZeroDigits(size_t count) { 
  DECLARE_CALLCOUNTER("insertZeroDigits");

  COUNTKEYCALL(count);

  Digit *p;
  for(p = m_first; count--;) {
    Digit *q = newDigit();
    q->n    = 0;
    q->next = p;
    p->prev = q;
    p       = q;
  }
  (m_first = p)->prev = NULL;
}


void BigReal::insertZeroDigitsAfter(Digit *p, size_t count) {
  DECLARE_CALLCOUNTER("insertZeroDigitsAfter");

  COUNTKEYCALL(count);

  Digit *q = p->next;
  if(q) {                     // p has a tail => p != last
    while(count--) {          // Insert count digits just after p, making the new chain grow at the HEAD (=q)
      Digit *r = newDigit();
      r->n     = 0;
      r->next  = q;
      q->prev  = r;
      q        = r;
    }
    (p->next = q)->prev = p;  // No need to modify m_last. p has a tail, so it is not m_last
  } else {                    // q == NULL => p == m_last (and maybe m_first too)
    for(q = p; count--;) {    // Append count digits after each other, making the new chain grow at the END (=q)
      Digit *r = newDigit();
      r->n     = 0;
      r->prev  = q;
      q->next  = r;
      q        = r;
    }
    (m_last = q)->next = NULL;
  }
}

void BigReal::insertBorrowDigitsAfter(Digit *p, size_t count) {
  DECLARE_CALLCOUNTER("insertBorrowDigitsAfter");

  COUNTKEYCALL(count);

  Digit *q = p->next;
  if(q) {                     // p has a tail => p != last
    while(count--) {          // Insert count digits just after p, making the new chain grow at the HEAD (=q)
      Digit *r = newDigit();
      r->n     = BIGREALBASE - 1;
      r->next  = q;
      q->prev  = r;
      q        = r;
    }
    (p->next = q)->prev = p;  // No need to modify m_last. p has a tail, so it is not m_last
  } else {                    // q == NULL => p == m_last (and maybe m_first too)
    for(q = p; count--;) {    // Append count digits after each other, making the new chain grow at the END (=q)
      Digit *r = newDigit();
      r->n     = BIGREALBASE - 1;
      r->prev  = q;
      q->next  = r;
      q        = r;
    }
    (m_last = q)->next = NULL;
  }
}

void BigReal::trimHead() { // assume m_first && m_first == 0
  m_expo--;
  Digit *p;
  for(p = m_first->next; p && (p->n == 0); p = p->next, m_expo--);

  if(p == NULL) { // all digits were 0 => *this = zero
    setToZero();
  } else {
    deleteDigits(m_first, p->prev);
    (m_first = p)->prev = NULL;

    if(m_last->n == 0) {
      for(m_low++, p = m_last->prev; p && (p->n == 0); p = p->prev, m_low++);
      deleteDigits(p->next, m_last);
      (m_last = p)->next = NULL;
    }
  }
}

void BigReal::trimTail() { // assume m_first != NULL => m_last != NULL and m_last->n == 0
  m_low++;
  Digit *p;
  for(p = m_last->prev; p && (p->n == 0); p = p->prev, m_low++);
  deleteDigits(p->next, m_last); // we know that there is at least one digit != 0 => p != NULL
  (m_last = p)->next = NULL;
}

// Assume src != zero && length <= src.getLength() && m_first == m_last == NULL
void BigReal::copyDigits(const BigReal &src, size_t length) {
  DECLARE_CALLCOUNTER("copyDigits");

  COUNTKEYCALL(length);

  if(length--) {
    const Digit *sd = src.m_first;
    (m_first = newDigit())->prev = NULL;
    Digit *p = m_first;
    p->n = sd->n;
    Digit *dd;
    for(sd = sd->next; length--; sd = sd->next, p = dd) {
      (dd = newDigit())->prev = p;
      dd->n   = sd->n;
      p->next = dd;
      ;
    }
    (m_last = p)->next = NULL;
  } else {
    m_first = m_last = NULL;
  }
}

// Assume src != zero && m_first == m_last == NULL
void BigReal::copyAllDigits(const BigReal &src) {
  DECLARE_CALLCOUNTER("copyAllDigits");

  COUNTCALL();

  const Digit *sd = src.m_first;
  Digit       *dd, *p;
  (m_first = p = newDigit())->prev = NULL;
  p->n = sd->n;
  for(; sd = sd->next; p = dd) {
    (dd = newDigit())->prev = p;
    dd->n   = sd->n;
    p->next = dd;
  }
  (m_last = p)->next = NULL;
}

#define TENE0  1
#define TENE1  10
#define TENE2  100
#define TENE3  1000
#define TENE4  10000
#define TENE5  100000
#define TENE6  1000000
#define TENE7  10000000
#define TENE8  100000000
#define TENE9  1000000000
#define TENE10 10000000000
#define TENE11 100000000000
#define TENE12 1000000000000
#define TENE13 10000000000000
#define TENE14 100000000000000
#define TENE15 1000000000000000
#define TENE16 10000000000000000
#define TENE17 100000000000000000
#define TENE18 1000000000000000000
#define TENE19 10000000000000000000

// Assume n = [0..1e8[
int BigReal::getDecimalDigitCount32(unsigned long n) { // static
  // kind of binary search
  if(n < TENE4) {
    if(n < TENE2) {
      if(n < TENE1) {
        return n ? 1 : 0;
      } else { // 10 <= n < 1e2
        return 2;
      }
    } else { // 1e2 <= n < 1e4
      return (n < TENE3) ? 3 : 4;
    }
  } else { // 1e4 <= n < 1e9
    if(n < TENE6) {
      return (n < TENE5) ? 5 : 6;
    } else { // 1e6 <= n < 1e9
      return (n < TENE7) ? 7 : 8;
    }
  }
}

// Assume n = [0..1e19]
int BigReal::getDecimalDigitCount64(unsigned __int64 n) { // static
  // Binary search
  if(n < TENE10) {                          // n < 1e10
    if(n < TENE5) {                         // n < 1e5
      if(n < TENE2) {                       // n < 1e2
        if(n < TENE1) {                     //
          return n ? 1 : 0;                 // n < 1e1
        } else {                            // 1e1 <= n < 1e2
          return 2;                         //
        }                                   //
      } else {                              // 1e2 <= n < 1e5
        if (n < TENE4) {                    // 1e2 <= n < 1e4
          return (n < TENE3) ? 3 : 4;       // 1e2 <= n < 1e3
        } else {                            // 1e3 <= n < 1e5
          return 5;                         //
        }                                   //
      }                                     //
    } else {                                // 1e5 <= n < 1e10
      if(n < TENE7) {                       // 1e5 <= n < 1e7
        return (n < TENE6) ? 6 : 7;         //
      } else {                              // 1e7 <= n < 1e10
        if (n < TENE9) {                    // 1e7 <= n < 1e9
          return (n < TENE8) ? 8 : 9;       //
        } else {                            // 1e9 <= n < 1e10
          return 10;                        //
        }                                   //
      }                                     //
    }                                       //
  } else {                                  // 1e10 <= n <= 1e19
    if(n < TENE15) {                        // 1e10 <= n < 1e15
      if(n < TENE12) {                      // 1e10 <= n < 1e12
        return (n < TENE11) ? 11 : 12;      // 1e10 <= n < 1e11
      } else {                              // 1e12 <= n < 1e15
        if (n < TENE14) {                   // 1e12 <= n < 1e13
          return (n < TENE13) ? 13 : 14;    //
        } else {                            // 1e13 <= n < 1e15
          return 15;                        //
        }                                   //
      }                                     //
    } else {                                // 1e15 <= n <= 1e19
      if (n < TENE17) {                     // 1e15 <= n < 1e17
        return (n < TENE16) ? 16 : 17;      //
      } else {                              // 1e17 <= n <= 1e19
        if (n < TENE19) {                   // 1e17 <= n < 1e19
          return (n < TENE18) ? 18 : 19;    //
        } else {                            // 1e18 <= n <= 1e19
          return 29;                        //
        }                                   //
      }                                     //
    }                                       //
  }                                         //
}                                           //

#ifdef HAS_LOOP_DIGITCOUNT
// Assume n = [0..1eMAXDIGITS_INT64]
int BigReal::getDecimalDigitCount64Loop(unsigned __int64 n) { // static
  static const unsigned __int64 pow10Table[] = {
     TENE0    ,TENE1    ,TENE2    ,TENE3
    ,TENE4    ,TENE5    ,TENE6    ,TENE7
    ,TENE8    ,TENE9    ,TENE10   ,TENE11
    ,TENE12   ,TENE13   ,TENE14   ,TENE15
    ,TENE16   ,TENE17   ,TENE18   ,TENE19
  };
  int l = 0, r = ARRAYSIZE(pow10Table);
  while(l < r) {
    const int m = (l+r)/2;
    const unsigned __int64 &p10 = pow10Table[m];
    if(p10 <= n) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  return r;
}
#endif // HAS_LOOP_DIGITCOUNT

const BRDigitType BigReal::s_power10Table[POWER10TABLESIZE] = {
  TENE0    ,TENE1
 ,TENE2    ,TENE3
 ,TENE4    ,TENE5
 ,TENE6    ,TENE7
 ,TENE8    ,TENE9
#ifdef IS64BIT
 ,TENE10   ,TENE11
 ,TENE12   ,TENE13
 ,TENE14   ,TENE15
 ,TENE16   ,TENE17
 ,TENE18   ,TENE19
#endif // IS64BIT
};

// Return p if n = 10^p for p = [0..9]. else return -1.
int BigReal::isPow10(size_t n) { // static
  switch(n) {
  case TENE0:  return  0;
  case TENE1:  return  1;
  case TENE2:  return  2;
  case TENE3:  return  3;
  case TENE4:  return  4;
  case TENE5:  return  5;
  case TENE6:  return  6;
  case TENE7:  return  7;
  case TENE8:  return  8;
  case TENE9:  return  9;
#ifdef IS64BIT
  case TENE10: return  10;
  case TENE11: return  11;
  case TENE12: return  12;
  case TENE13: return  13;
  case TENE14: return  14;
  case TENE15: return  15;
  case TENE16: return  16;
  case TENE17: return  17;
  case TENE18: return  18;
  case TENE19: return  19;
#endif // IS64BIT
  default        : return -1;
  }
}

bool BigReal::isPow10(const BigReal &x) { // static
  return !x.isZero() && (x.getLength() == 1) && (isPow10(x.getFirstDigit()) >= 0);
}

BigReal &BigReal::multPow10(BRExpoType exp) {
  DEFINEMETHODNAME;
  if(isZero()) {
    return *this;
  }
  int m = exp % LOG10_BIGREALBASE;
  BRExpoType   n = exp / LOG10_BIGREALBASE;
  if(m == 0) {
    m_expo += n;
    m_low  += n;
    if(m_expo > BIGREAL_MAXEXPO || m_expo < BIGREAL_MINEXPO) {
      throwBigRealInvalidArgumentException(method, _T("Invalid m_expo:%s"), format1000(m_expo).cstr());
    }
  } else {
    if(m < 0) {
      m = -m;
    }
    const BRDigitType s = pow10(m);
    const BRDigitType t = BIGREALBASE / s;
    if(exp > 0) { // shift left
      Digit *p = m_first; 
      Digit *q = p->next; // *this != 0, =>. p != NULL
      if(p->n >= t) {
        insertDigit(p->n / t);
        m_expo++;
      }
      while(q) {
        p->n = q->n/t + (p->n%t) * s;
        p = q;
        q = q->next;
      }
      p->n = (p->n%t) * s;
    } else { // exp < 0. shift right
      Digit *p = m_last;
      Digit *q = p->prev; // *this != 0, =>. p != NULL
      appendDigit((p->n % s) * t);
      m_low--;
      while(q) {
        p->n = p->n/s + (q->n%s) * t;
        p = q;
        q = q->prev;
      }
      p->n /= s;
    }
    m_expo += n;
    m_low  += n;
    trimZeroes();
  }
  SETBIGREALDEBUGSTRING(*this);
  return *this;
}


BigReal &copy(BigReal &to, const BigReal &from, const BigReal &f) {
  if(!f.isPositive() || from.isZero()) {
    to = from;
  } else if(f.m_expo < from.m_low) {
    to = from;
  } else if(f.m_expo > from.m_expo) {
    to = to.getDigitPool()->get0();
  } else { // from.m_low <= f.m_expo <= from.m_expo
    to.clearDigits();
    to.m_expo     = from.m_expo;
    to.m_low      = max(from.m_low, f.m_expo);
    to.m_negative = from.m_negative;
    to.copyDigits(from, to.m_expo - to.m_low + 1);
    to.trimZeroes();
    SETBIGREALDEBUGSTRING(to);
  }
  return to;
}

BigReal &copy(BigReal &to, const BigReal &from, size_t length) {
  if(from.isZero()) {
    to.setToZero();
  } else {
    length = minMax(length, (size_t)1, from.getLength());
    to.m_low = (to.m_expo = from.m_expo) - length + 1;
    to.m_negative = from.m_negative;
    to.clearDigits();
    to.copyDigits(from, length);
    to.trimZeroes();
    SETBIGREALDEBUGSTRING(to);
  }
  return to;
}

int compare(const BigReal &x, const BigReal &y) {
  if(&x == &y) {
    return 0;
  }
  if(x.isZero()) {
    return y.isZero() ? 0 : y.m_negative ? 1 : -1;
  }

  // x != 0
  if(y.isZero()) {
    return x.m_negative ? -1 : 1;
  }

  // x != 0 && y != 0
  if(x.m_negative != y.m_negative) {
    return x.m_negative ? -1 : 1;
  }

  // sign(x) == sign(y) 
  if(x.m_expo > y.m_expo) { // abs(x) > abs(y)
    return x.m_negative ? -1 : 1;
  }
  if(x.m_expo < y.m_expo) { // abs(x) < abs(y)
    return x.m_negative ? 1 : -1;
  }

  // Compare digits
  const int s = x.m_negative ? -1 : 1;
  const Digit *xp, *yp;
  for(xp = x.m_first, yp = y.m_first; xp && yp; xp = xp->next, yp = yp->next) {
    const BRDigitDiffType d = (BRDigitDiffType)xp->n - (BRDigitDiffType)yp->n;
    if(d) {
      return sign(d) * s;
    }
  }
  return xp ? s : yp ? -s : 0;
}

int compareAbs(const BigReal &x, const BigReal &y) {
  if(&x == &y) {
    return 0;
  }
  if(x.isZero()) {
    return y.isZero() ? 0 : -1;
  }

  // x != 0
  if(y.isZero()) {
    return 1;
  }

  // x != 0 && y != 0
  if(x.m_expo > y.m_expo) { // abs(x) > abs(y)
    return 1;
  }
  if(x.m_expo < y.m_expo) { // abs(x) < abs(y)
    return -1;
  }

  // Compare digits
  const Digit *xp, *yp;
  for(xp = x.m_first, yp = y.m_first; xp && yp; xp = xp->next, yp = yp->next) {
    const BRDigitDiffType d = (BRDigitDiffType)xp->n - (BRDigitDiffType)yp->n;
    if(d) {
      return sign(d);
    }
  }
  return xp ? 1 : yp ? -1 : 0;
}

bool operator==(const BigReal &x,  const BigReal &y) {
  return compare(x,y) == 0;
}
  
bool operator!=(const BigReal &x,  const BigReal &y) {
  return compare(x,y) != 0;
}
  
bool operator>=(const BigReal &x,  const BigReal &y) {
  return compare(x,y) >= 0;
}
  
bool operator<=(const BigReal &x,  const BigReal &y) {
  return compare(x,y) <= 0;
}
  
bool operator>(const BigReal &x,  const BigReal &y) {
  return compare(x,y) >  0;
}
  
bool operator<(const BigReal &x,  const BigReal &y) {
  return compare(x,y) <  0;
}
  
int getInt(const BigReal &x) {
  return (int)getLong(x);
}

unsigned int getUint(const BigReal &x) {
  return (unsigned int)getUlong(x);
}

long getLong(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(x > ConstBigReal::_long_max) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_long_max));
  }
  if(x < ConstBigReal::_long_min) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_long_min));
  }

  intptr_t   result = 0;
  BRExpoType i      = x.m_expo;
  for(const Digit *p = x.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;

  return (long)(x.isNegative() ? -result : result);
}

unsigned long getUlong(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(x.isNegative()) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, _T("0"));
  }
  if(x > ConstBigReal::_ulong_max) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_ulong_max));
  }

  size_t     result = 0;
  BRExpoType i      = x.m_expo;
  for(const Digit *p = x.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;

  return (unsigned long)result;
}

__int64 getInt64(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(x > ConstBigReal::_i64_max) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_i64_max));
  }
  if(x < ConstBigReal::_i64_min) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_i64_min));
  }

  __int64    result = 0;
  BRExpoType i      = x.m_expo;
  for(const Digit *p = x.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return x.isNegative() ? -result : result;
}

unsigned __int64 getUint64(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(x.isNegative()) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, _T("0"));
  }
  if(x > ConstBigReal::_ui64_max) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_ui64_max));
  }

  unsigned __int64 result = 0;
  BRExpoType       i      = x.m_expo;
  for(const Digit *p = x.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return result;
}

#ifdef IS64BIT
_int128 getInt128(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(x > ConstBigReal::_i128_max) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_i128_max));
  }
  if(x < ConstBigReal::_i128_min) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_i128_min));
  }

  _int128    result = 0;
  BRExpoType i      = x.m_expo;
  for(const Digit *p = x.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return x.isNegative() ? -result : result;
}

_uint128 getUint128(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(x.isNegative()) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, _T("0"));
  }
  if(x > ConstBigReal::_ui128_max) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_ui128_max));
  }

  _uint128   result = 0;
  BRExpoType i      = x.m_expo;
  for(const Digit *p = x.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return result;
}
#endif // IS64BIT

unsigned long BigReal::hashCode() const {
  size_t s = m_expo;
  if(isZero()) {
    return 0;
  }
  if(m_negative) s = ~s;
  for(const Digit *p = m_first; p; p = p->next) {
    s = s * 17 + p->n;
  }
#ifdef IS32BIT
  return s;
#else // IS64BIT
  return uint64Hash(s);
#endif
}

static void throwAssertionException(_In_z_ _Printf_format_string_ const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format,argptr);
  va_end(argptr);
  throwBigRealException(_T("assertIsValidBigReal:%s"), msg.cstr());
}

void BigReal ::assertIsValidBigReal() const {
  if(m_expo == BIGREAL_ZEROEXPO) {
    if(m_first != NULL) {
      throwAssertionException(_T("m_expo == BIGREAL_ZEROEXPO, but m_first != NULL"));
    }
    if(m_last != NULL) {
      throwAssertionException(_T("m_expo == BIGREAL_ZEROEXPO, but m_last != NULL"));
    }
    return;
  }
  size_t digitCount = 0;
  for(const Digit *p = m_first; p; p = p->next) {
    if(p->n >= BIGREALBASE) {
      throwAssertionException(_T("Digit(%s) (=%s) >= BIGREALBASE (=%lu)")
                             ,format1000(digitCount).cstr()
                             ,format1000(p->n).cstr()
                             ,format1000(BIGREALBASE).cstr());
    }
    digitCount++;
  }
  if(digitCount == 0) {
    throwAssertionException(_T("#digits in chain = 0. x != 0"));
  }
  if(digitCount != getLength()) {
    throwAssertionException(_T("#digits in chain (=%s) != getLength() (=%s)")
                           ,format1000(digitCount).cstr()
                           ,format1000(getLength()).cstr());
  }
  if(m_first->n == 0) {
    throwAssertionException(_T("m_first->n = 0"));
  }
  if(m_last->n == 0) {
    throwAssertionException(_T("m_last->n = 0"));
  }
  if(m_expo > BIGREAL_MAXEXPO) {
    throwAssertionException(_T("m_expo > BIGREAL_MAXEXPO (=%s)"), format1000(m_expo).cstr());
  }
  if(m_expo < BIGREAL_MINEXPO) {
    throwAssertionException(_T("m_expo < BIGREAL_MINEXPO (=%s)"), format1000(m_expo).cstr());
  }
}
