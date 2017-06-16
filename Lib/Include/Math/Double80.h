#pragma once

#include <StrStream.h>
#include <Random.h>
#include "PragmaLib.h"

#pragma check_stack(off)

typedef enum {
  FPU_LOW_PRECISION            // 32-bits floatingpoint
 ,FPU_NORMAL_PRECISION         // 64-bits floatingpoint
 ,FPU_HIGH_PRECISION           // 80-bits floatingpoint
} FPUPrecisionMode;

typedef enum {
  FPU_ROUNDCONTROL_ROUND       // Round to nearest, or to even if equidistant 
 ,FPU_ROUNDCONTROL_ROUNDDOWN   // Round down (toward -infinity) 
 ,FPU_ROUNDCONTROL_ROUNDUP     // Round up (toward +infinity)
 ,FPU_ROUNDCONTROL_TRUNCATE    // Truncate (toward 0)
} FPURoundMode;

#define FPU_INVALID_OPERATION_EXCEPTION 0x01
#define FPU_DENORMALIZED_EXCEPTION      0x02
#define FPU_DIVIDE_BY_ZERO_EXCEPTION    0x04
#define FPU_OVERFLOW_EXCEPTION          0x08
#define FPU_UNDERFLOW_EXCEPTION         0x10
#define FPU_PRECISION_EXCEPTION         0x20

#define FPU_ALL_EXCEPTIONS              0x3f

#ifdef IS64BIT
extern "C" {
void FPUinit();
void FPUgetStatusWord(WORD &dst);
void FPUgetControlWord(WORD &dst);
void FPUsetControlWord(const WORD &flags);
void FPUgetTagsWord(WORD *buffer); // WORD[14]
void FPUclearExceptions();
};
#endif

class FPU {
private:
  DECLARECLASSNAME;
  FPU() {} // Cannot be instatiated
public:
#ifdef IS32BIT
  static void             init();
  static unsigned short   getStatusWord();
  static unsigned short   getControlWord();
  static void             setControlWord(unsigned short flags);
  static unsigned short   getTagsWord();
  static void             clearExceptions();
#else
  static inline void             init() {
    FPUinit();
  }
  static inline unsigned short   getStatusWord() {
    WORD tmp;
    FPUgetStatusWord(tmp);
    return tmp;
  }
  static inline unsigned short   getControlWord() {
    WORD tmp;
    FPUgetControlWord(tmp);
    return tmp;
  }
  static inline void setControlWord(unsigned short flags) {
    FPUsetControlWord(flags);
  }
  static unsigned short          getTagsWord() {
    WORD buffer[14];
    FPUgetTagsWord(buffer);
    return buffer[4];
  }
  static inline void             clearExceptions() {
    FPUclearExceptions();
  }
#endif // IS32BIT

  static void             clearStatusWord();
  static unsigned short   setPrecisionMode(FPUPrecisionMode p); // returns currnet FPU controlwoed
  static FPUPrecisionMode getPrecisionMode();
  static unsigned short   setRoundMode(FPURoundMode mode);      // returns current FPU controlword
  static FPURoundMode     getRoundMode();
  static inline void      restoreControlWord(unsigned short ctrlWord) {
    setControlWord(ctrlWord);
  }
  static int              getStackHeight();
  static bool             stackOverflow();
  static bool             stackUnderflow();
  static bool             stackFault();
  static void             enableExceptions(bool enable, unsigned short flags);

};

class Double80;

#ifdef IS64BIT
extern "C" {
void D80consLong(       Double80 &s, const long             &x);
void D80consULong(      Double80 &s, const unsigned long     x);
void D80consLongLong(   Double80 &s, const __int64          &x);
void D80consULongLong(  Double80 &s, const unsigned __int64  x);
void D80consFloat(      Double80 &s, float                  &x);
void D80consDouble(     Double80 &s, const double           &x);
void D80ToLong(      long               &dst, const Double80 &src);
void D80ToULong(     unsigned long      &dst, const Double80 &src);
void D80ToLongLong(  long long          &dst, const Double80 &src);
void D80ToULongLong( unsigned long long &dst, const Double80 &src);
void D80ToFloat(     float              &dst, const Double80 &src);
void D80ToDouble(    double             &dst, const Double80 &src);
void D80D80sum(      Double80 &dst, const Double80 &x, const Double80 &y);
void D80D80dif(      Double80 &dst, const Double80 &x, const Double80 &y);
void D80D80mul(      Double80 &dst, const Double80 &x, const Double80 &y);
void D80D80div(      Double80 &dst, const Double80 &x, const Double80 &y);
int  D80D80Compare(  const Double80 &x, const Double80 &y);
int  D80isZero(      const Double80 &x);
void D80assignAdd(   Double80 &dst, const Double80 &x);
void D80assignSub(   Double80 &dst, const Double80 &x);
void D80assignMul(   Double80 &dst, const Double80 &x);
void D80assignDiv(   Double80 &dst, const Double80 &x);
void D80neg(         Double80 &dst, const Double80 &x);
void D80increment(   Double80 &dst);
void D80decrement(   Double80 &dst);
void D80getPi(       Double80 &dst);
void D80getEps(      Double80 &dst);
void D80getMin(      Double80 &dst);
void D80getMax(      Double80 &dst);
void D80getExpo2(    int &dst         , const Double80 &x);
void D80getExpo10(   int &dst         , const Double80 &x);
void D80fabs(        Double80 &dst, const Double80 &x);
void D80sqr(         Double80 &dst, const Double80 &x);
void D80sqrt(        Double80 &dst, const Double80 &x);
void D80modulus(     Double80 &dst, const Double80 &x, const Double80 &y);
void D80sin(         Double80 &dst, const Double80 &x);
void D80cos(         Double80 &dst, const Double80 &x);
void D80tan(         Double80 &dst, const Double80 &x);
void D80atan(        Double80 &dst, const Double80 &x);
void D80atan2(       Double80 &dst, const Double80 &y, const Double80 &x);
void D80sincos(      Double80     &c  , Double80       &s);
void D80exp(         Double80 &dst, const Double80 &x);
void D80log(         Double80 &dst, const Double80 &x);
void D80log10(       Double80 &dst, const Double80 &x);
void D80log2(        Double80 &dst, const Double80 &x);
void D80pow(         Double80 &dst, const Double80 &x, const Double80 &y);
void D80pow10(       Double80 &dst, const Double80 &x);
void D80pow2(        Double80 &dst, const Double80 &x);
void D80floor(       Double80 &dst, const Double80 &x);
void D80ceil(        Double80 &dst, const Double80 &x);
void D80ToBCD(BYTE bcd[10], const Double80 &src);
void D80ToBCDAutoScale(BYTE bcd[10], const Double80 &x, int &expo10);
}
#endif // IS64BIT

class Double80 {
private:
  DECLARECLASSNAME;
  BYTE m_value[10]; // Must be the first field in the class
  void init(const _TUCHAR *s);

public:
  inline Double80() {
  }

  inline Double80(int x) {
    *this = (Double80)(long)(x);
  }

  inline Double80(UINT x) {
    *this = (Double80)(ULONG)(x);
  }

#ifdef IS32BIT
  Double80(long             x);
  Double80(ULONG            x);
  Double80(INT64            x);
  Double80(UINT64           x);
  Double80(float            x);
  Double80(double           x);
#else
  inline Double80(long x) {
    D80consLong(*this, x);
  }
  inline Double80(ULONG x) {
    D80consULong(*this, x);
  }
  inline Double80(INT64 x) {
    D80consLongLong(*this, x);
  }
  inline Double80(UINT64 x) {
    D80consULongLong(*this, x);
  }
  inline Double80(float x) {
    D80consFloat(*this, x);
  }
  inline Double80(double x) {
    D80consDouble(*this, x);
  }
#endif // IS32BIT

  explicit Double80(const String &s);
  explicit Double80(const TCHAR  *s);
  explicit Double80(const BYTE   *bytes);
#ifdef UNICODE
  explicit Double80(const char   *s);
#endif // UNICODE

  inline friend int getInt(const Double80 &x) {
    return getLong(x);
  }

  inline friend UINT getUint(const Double80 &x) {
    return getUlong(x);
  }

#ifdef IS32BIT
  friend          long    getLong(  const Double80 &x);
  friend unsigned long    getUlong( const Double80 &x);
  friend          __int64 getInt64( const Double80 &x);
  friend unsigned __int64 getUint64(const Double80 &x);
  friend float            getFloat( const Double80 &x);
  friend double           getDouble(const Double80 &x);

  static int              getExpo2(const Double80 &x);
  static int              getExpo10(const Double80 &x); // x == 0 ? 0 : floor(log10(|x|))

  Double80 &operator+=(const Double80 &x);
  Double80 &operator-=(const Double80 &x);
  Double80 &operator*=(const Double80 &x);
  Double80 &operator/=(const Double80 &x);

  Double80 &operator++();   // prefix-form
  Double80 &operator--();   // prefix-form
  Double80 operator++(int); // postfix-form
  Double80 operator--(int); // postfix-form

  bool isZero()     const;

#else // !IS32BIT (ie IS64BIT)

  friend inline long getLong(const Double80 &x) {
    long tmp; 
    D80ToLong(tmp, x);
    return tmp;
  }

  friend inline ULONG getUlong(const Double80 &x) {
    unsigned long tmp;
    D80ToULong(tmp, x);
    return tmp;
  }

  friend inline INT64 getInt64(const Double80 &x) {
    long long tmp;
    D80ToLongLong(tmp, x);
    return tmp;
  }

  friend inline UINT64 getUint64(const Double80 &x) {
    unsigned long long tmp;
    D80ToULongLong(tmp, x);
    return tmp;
  }

  friend inline float getFloat(const Double80 &x) {
    float tmp;
    D80ToFloat(tmp, x);
    return tmp;
  }

  friend inline double getDouble(const Double80 &x) {
    double tmp;
    D80ToDouble(tmp, x);
    return tmp;
  }

  static inline int getExpo2(const Double80 &x) {
    int result;
    D80getExpo2(result, x);
    return result;
  }

  static inline int getExpo10(const Double80 &x) { // x == 0 ? 0 : floor(log10(|x|))
    int result;
    D80getExpo10(result, x);
    return result;
  }

  inline Double80 &operator+=(const Double80 &x) {
    D80assignAdd(*this, x);
    return *this;
  }

  inline Double80 &operator-=(const Double80 &x) {
    D80assignSub(*this, x);
    return *this;
  }

  inline Double80 &operator*=(const Double80 &x) {
    D80assignMul(*this, x);
    return *this;
  }

  inline Double80 &operator/=(const Double80 &x) {
    D80assignDiv(*this, x);
    return *this;
  }

  inline Double80 &operator++() {   // prefix-form
    D80increment(*this);
    return *this;
  }

  inline Double80 &operator--() {   // prefix-form
    D80decrement(*this);
    return *this;
  }

  inline Double80 operator++(int) { // postfix-form
    Double80 result(*this);
    D80increment(*this);
    return result;
  }

  inline Double80 operator--(int) { // postfix-form
    Double80 result(*this);
    D80decrement(*this);
    return result;
  }

  inline bool isZero() const {
    return D80isZero(*this) ? true : false;
  }

#endif // IS32BIT

  static TCHAR *d80tot(TCHAR *dst, const Double80 &x); // dst must point to memory with at least 26 free TCHAR
  static char  *d80toa(char  *dst, const Double80 &x); // dst must point to memory with at least 26 free char
  bool isPositive() const { return (m_value[9] & 0x80) == 0; }
  bool isNegative() const { return (m_value[9] & 0x80) != 0; }

  ULONG hashCode() const;

  String toString() const;

  inline void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)m_value, sizeof(m_value));
  }

  inline void load(ByteInputStream &s) {
    s.getBytesForced((BYTE*)m_value, sizeof(m_value));
  }

  friend inline Packer &operator<<(Packer &p, const Double80 &d) {
    return p.addElement(Packer::E_DOUBLE, d.m_value, sizeof(d.m_value));
  }

  friend inline Packer &operator >> (Packer &p, Double80 &d) {
    p.getElement(Packer::E_DOUBLE, d.m_value, sizeof(d.m_value));
    return p;
  }

  static const Double80 zero;          // = 0
  static const Double80 one;           // = 1
  static const Double80 M_PI;          // = 3.1415926535897932384626433
  static const Double80 DBL80_EPSILON; // Smallest such that 1.0+DBL80_EPSILON != 1.0 (=1.08420217248550443e-019)
  static const Double80 DBL80_MIN;     // Min positive value (=3.36210314311209209e-4932)
  static const Double80 DBL80_MAX;     // Max value          (=1.18973149535723227e+4932)
  static const int      DBL80_DIG;     // # of decimal digits of precision

  static void initClass();
};

#ifdef IS32BIT

Double80 operator+(const Double80 &x, const Double80 &y);
Double80 operator-(const Double80 &x, const Double80 &y);
Double80 operator-(const Double80 &x);
Double80 operator*(const Double80 &x, const Double80 &y);
Double80 operator/(const Double80 &x, const Double80 &y);

bool operator==(const Double80 &x, const Double80 &y);
bool operator!=(const Double80 &x, const Double80 &y);
bool operator<=(const Double80 &x, const Double80 &y);
bool operator>=(const Double80 &x, const Double80 &y);
bool operator< (const Double80 &x, const Double80 &y);
bool operator> (const Double80 &x, const Double80 &y);

Double80 fabs(const Double80 &x);
Double80 fmod(const Double80 &x, const Double80 &y);
Double80 sqr( const Double80 &x);
Double80 sqrt(const Double80 &x);
Double80 sin( const Double80 &x);
Double80 cos( const Double80 &x);
Double80 tan( const Double80 &x);
Double80 atan(const Double80 &x);
Double80 atan2(const Double80 &y, const Double80 &x);
void     sincos(Double80 &c, Double80 &s); // calculate both cos and sin. c:inout c, s:out
Double80 exp(const Double80 &x);
Double80 log(const Double80 &x);
Double80 log10(const Double80 &x);
Double80 log2(const Double80 &x);
Double80 pow(const Double80 &x, const Double80 &y);
Double80 pow10(const Double80 &x);
Double80 pow2(const Double80 &x);
Double80 floor(const Double80 &x);
Double80 ceil(const Double80 &x);

#else // !IS32BIT (ie IS64BIT)

inline Double80 operator+(const Double80 &x, const Double80 &y) {
  Double80 tmp;
  D80D80sum(tmp, x, y);
  return tmp;
}

inline Double80 operator-(const Double80 &x, const Double80 &y) {
  Double80 tmp;
  D80D80dif(tmp, x, y);
  return tmp;
}

inline Double80 operator-(const Double80 &x) {
  Double80 tmp;
  D80neg(tmp, x);
  return tmp;
}

inline Double80 operator*(const Double80 &x, const Double80 &y) {
  Double80 tmp;
  D80D80mul(tmp, x, y);
  return tmp;
}

inline Double80 operator/(const Double80 &x, const Double80 &y) {
  Double80 tmp;
  D80D80div(tmp, x, y);
  return tmp;
}

inline bool operator==(const Double80 &x, const Double80 &y) {
  return D80D80Compare(x, y) == 0;
}

inline bool operator!=(const Double80 &x, const Double80 &y) {
  return D80D80Compare(x, y) != 0;
}

inline bool operator<=(const Double80 &x, const Double80 &y) {
  return D80D80Compare(x, y) <= 0;
}

inline bool operator>=(const Double80 &x, const Double80 &y) {
  return D80D80Compare(x, y) >= 0;
}

inline bool operator< (const Double80 &x, const Double80 &y) {
  return D80D80Compare(x, y) < 0;
}

inline bool operator> (const Double80 &x, const Double80 &y) {
  return D80D80Compare(x, y) > 0;
}

inline Double80 fabs(const Double80 &x) {
  Double80 result;
  D80fabs(result, x);
  return result;
}

inline Double80 fmod(const Double80 &x, const Double80 &y) {
  Double80 result;
  D80modulus(result, x, y);
  return result;
}

inline Double80 sqr(const Double80 &x) {
  Double80 result;
  D80sqr(result, x);
  return result;
}

inline Double80 sqrt(const Double80 &x) {
  Double80 result;
  D80sqrt(result, x);
  return result;
}

inline Double80 sin(const Double80 &x) {
  Double80 result;
  D80sin(result, x);
  return result;
}

inline Double80 cos(const Double80 &x) {
  Double80 result;
  D80cos(result, x);
  return result;
}

inline Double80 tan(const Double80 &x) {
  Double80 result;
  D80tan(result, x);
  return result;
}

inline Double80 atan(const Double80 &x) {
  Double80 result;
  D80atan(result, x);
  return result;
}

inline Double80 atan2(const Double80 &y, const Double80 &x) {
  Double80 result;
  D80atan2(result, y, x);
  return result;
}

inline void sincos(Double80 &c, Double80 &s) { // calculate both cos and sin. c:inout c, s:out
  D80sincos(c, s);
}

inline Double80 exp(const Double80 &x) {
  Double80 result;
  D80exp(result, x);
  return result;
}

inline Double80 log(const Double80 &x) {
  Double80 result;
  D80log(result, x);
  return result;
}

inline Double80 log10(const Double80 &x) {
  Double80 result;
  D80log10(result, x);
  return result;
}

inline Double80 log2(const Double80 &x) {
  Double80 result;
  D80log2(result, x);
  return result;
}

inline Double80 pow(const Double80 &x, const Double80 &y) {
  Double80 result;
  D80pow(result, x, y);
  return result;
}

inline Double80 pow10(const Double80 &x) {
  Double80 result;
  D80pow10(result, x);
  return result;
}

inline Double80 pow2(const Double80 &x) {
  Double80 result;
  D80pow2(result, x);
  return result;
}

inline Double80 floor(const Double80 &x) {
  Double80 result;
  D80floor(result, x);
  return result;
}

inline Double80 ceil(const Double80 &x) {
  Double80 result;
  D80ceil(result, x);
  return result;
}

#endif // IS32BIT

Double80 cot(  const Double80 &x);
Double80 asin( const Double80 &x);
Double80 acos( const Double80 &x);
Double80 acot( const Double80 &x);
Double80 root( const Double80 &x, const Double80 &y);
Double80 cosh( const Double80 &x);
Double80 sinh( const Double80 &x);
Double80 tanh( const Double80 &x);
Double80 acosh(const Double80 &x);
Double80 asinh(const Double80 &x);
Double80 atanh(const Double80 &x);
Double80 fraction(   const Double80 &x);
int      sign(       const Double80 &x);
Double80 round(      const Double80 &x, int prec = 0);
Double80 Max(        const Double80 &x, const Double80 &y);
Double80 Min(        const Double80 &x, const Double80 &y);
Double80 minMax(     const Double80 &x, const Double80 &x1, const Double80 &x2);
bool     isNan(      const Double80 &x);
bool     isPInfinity(const Double80 &x);
bool     isNInfinity(const Double80 &x);
bool     isInfinity( const Double80 &x);

class RandomD80 : public Random {
public:
  Double80 nextDouble80();
  Double80 nextDouble80(const Double80 &low, const Double80 &high);
};

String toString(      const Double80 &x, int precision=6, int width=0, int flags=0);

tistream &operator>>(tistream &s,       Double80 &x);
tostream &operator<<(tostream &s, const Double80 &x);

StrStream &operator<<(StrStream &stream, const Double80 &x);
