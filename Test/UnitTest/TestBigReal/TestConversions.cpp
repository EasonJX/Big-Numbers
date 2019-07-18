#include "stdafx.h"
#include "FunctionTest.h"

class ConversionTest {
public:
  TestStatistic &m_stat;
  double         m_maxError;
  UINT           m_conversionCount;
  UINT           m_errorCount;
  UINT           m_sumLength, m_maxLength;
  ConversionTest(TestStatistic &stat);
  ~ConversionTest();
  void inline test(double error) {
    m_conversionCount++;
    if(error != 0) {
      m_errorCount++;
      if(error > m_maxError) m_maxError = error;
    }
  }
  void inline addLength(size_t length) {
    m_sumLength += (UINT)length;
    if(length > m_maxLength) m_maxLength = (UINT)length ;
  }
  String toString() const;
};

ConversionTest::ConversionTest(TestStatistic &stat) : m_stat(stat) {
  m_maxError        = 0;
  m_conversionCount = 0;
  m_errorCount      = 0;
  m_sumLength       = 0;
  m_maxLength       = 0;
}

ConversionTest::~ConversionTest() {
  m_stat.setEndMessage(_T("%s"), toString().cstr());
}

String ConversionTest::toString() const {
  return format(_T("errors/conv:%4lu/%-8lu Max rel.error:%le, avgLen:%6.4lf maxLen:%lu")
               ,m_errorCount
               ,m_conversionCount
               ,m_maxError
               ,(double)m_sumLength/m_conversionCount
               ,m_maxLength
               );
}

template<class T> static void testUndefined(const T fld, DigitPool *pool) {
  const bool nan     = isnan(      fld);
  const bool pinf    = isPInfinity(fld);
  const bool ninf    = isNInfinity(fld);
  const bool inf     = isinf(      fld);
  const bool finite  = isfinite(   fld);
  const bool isnorm  = isnormal(   fld);
  const int  cls     = fpclassify( fld);

  BigReal br(fld, pool);

  const bool nan1    = isnan(      br );
  const bool pinf1   = isPInfinity(br );
  const bool ninf1   = isNInfinity(br );
  const bool inf1    = isinf(      br );
  const bool finite1 = isfinite(   br );
  const bool isnorm1 = isnormal(   br );
  const int  cls1    = fpclassify( br );

  verify(nan    == nan1   );
  verify(pinf   == pinf1  );
  verify(ninf   == ninf1  );
  verify(inf    == inf1   );
  verify(finite == finite1);
  verify(isnorm == isnorm1);
  verify(cls    == cls1   );
}

static void testFloatConversion(TestStatistic &stat, int sign) {
  DigitPool     *pool       = stat.getDigitPool();
  const float    loopStart  = sign * FLT_MIN;
  const float    loopEnd    = sign * FLT_MAX / 2;
  const float    stepFactor = 1 + 0.0012345f / 2;
  const float    fltnan     = FLT_NAN;
  const float    fltpinf    = FLT_PINF;
  const float    fltninf    = FLT_NINF;
  size_t         length;

  ConversionTest convTest(stat);
  stat.setTotalTestCount(284190);

  if(sign > 0) {
    for(float f32 = loopStart; f32 < loopEnd; f32 *= stepFactor) {
      convTest.test(getRelativeError32(f32, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%5.1lf%%"), stat.getPercentDone());
    }
  } else {
    for(float f32 = loopStart; f32 > loopEnd; f32 *= stepFactor) {
      convTest.test(getRelativeError32(f32, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%5.1lf%%"), stat.getPercentDone());
    }
  }
  testUndefined( fltnan ,pool);
  testUndefined( fltpinf,pool);
  testUndefined( fltpinf,pool);
}

static void testDoubleConversion(TestStatistic &stat, int sign) {
  DigitPool     *pool       = stat.getDigitPool();
  const double   loopStart  = sign * DBL_MIN;
  const double   loopEnd    = sign * DBL_MAX / 2;
  const double   stepFactor = 1 + 0.00456789 / 2;
  const double   dblnan     = DBL_NAN;
  const double   dblpinf    = DBL_PINF;
  const double   dblninf    = DBL_NINF;

  size_t         length;
  ConversionTest convTest(stat);
  stat.setTotalTestCount(621339);

  if(sign > 0) {
    for(double d64 = loopStart; d64 < loopEnd; d64 *= stepFactor) {
      convTest.test(getRelativeError64(d64, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%5.1lf%%"), stat.getPercentDone());
    }
  } else {
    for(double d64 = loopStart; d64 > loopEnd; d64 *= stepFactor) {
      convTest.test(getRelativeError64(d64, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%5.1lf%%"), stat.getPercentDone());
    }
  }
  testUndefined( dblnan ,pool);
  testUndefined( dblpinf,pool);
  testUndefined( dblpinf,pool);
}

static void testDouble80Conversion(TestStatistic &stat, int sign) {
  DigitPool     *pool       = stat.getDigitPool();
  const Double80 loopStart  = DBL80_MIN * sign;
  const Double80 loopEnd    = DBL80_MAX / 2 * sign;
  const Double80 stepFactor = 1 + 0.012345/4;
  const Double80 dblnan     = DBL80_NAN;
  const Double80 dblpinf    = DBL80_PINF;
  const Double80 dblninf    = DBL80_NINF;
  size_t         length;
  ConversionTest convTest(stat);
  stat.setTotalTestCount(7370108);

  if(sign > 0) {
    for(Double80 d80 = loopStart; d80 < loopEnd; d80 *= stepFactor) {
      convTest.test(getRelativeError80(d80, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%5.1lf%%"), stat.getPercentDone());
    }
  } else {
    for(Double80 d80 = loopStart; d80 > loopEnd; d80 *= stepFactor) {
      convTest.test(getRelativeError80(d80, pool, &length));
      convTest.addLength(length);
      if(stat.isTimeToPrint()) stat.printLoopMessage(_T("%5.1lf%%"), stat.getPercentDone());
    }
  }
  testUndefined( dblnan ,pool);
  testUndefined( dblpinf,pool);
  testUndefined( dblpinf,pool);

}

void testPositiveFloatConversion(TestStatistic &stat) {
  testFloatConversion(stat, 1);
}

void testNegativeFloatConversion(TestStatistic &stat) {
  testFloatConversion(stat, -1);
}

void testPositiveDoubleConversion(TestStatistic &stat) {
  testDoubleConversion(stat, 1);
}

void testNegativeDoubleConversion(TestStatistic &stat) {
  testDoubleConversion(stat, -1);
}

void testPositiveDouble80Conversion(TestStatistic &stat) {
  testDouble80Conversion(stat, 1);
}

void testNegativeDouble80Conversion(TestStatistic &stat) {
  testDouble80Conversion(stat, -1);
}

void testGetExpo2(TestStatistic &stat) {
  DigitPool     *pool       = stat.getDigitPool();
  const Double80 startValue = DBL80_MIN;
  const Double80 stepFactor = 1.00012345;

  ConversionTest convTest(stat);

  const String intervalString = format(_T("[%s..%s]"), toString(startValue).cstr(), toString(DBL80_MAX).cstr());
  const Double80 loopStart  = startValue;
  stat.out() << _T("Start:") << loopStart << _T(", Step:") << stepFactor << endl;
  for(Double80 x = loopStart; !isnan(x); x *= stepFactor) {
    convTest.test(getRelativeError80(x, pool));

    if(stat.isTimeToPrint()) {
      TCHAR tmpStr[30];
      stat.printLoopMessage(_T("x:%s %s"), d80tot(tmpStr, x), intervalString.cstr());
    }
  }
}
