#include "stdafx.h"
#include <math.h>
#include <Random.h>
#include <CompactLineArray.h>
#include <StrStream.h>
#include <Math/MathLib.h>
#include <Math/Rational.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

#define VERIFYOP(op, maxError) {                                                                     \
  const Rational rBinResult = (r1) op (r2);                                                          \
  const double   dBinResult = (d1) op (d2);                                                          \
  const double   dFromR     = getDouble(rBinResult);                                                 \
  const double   error      = fabs(dFromR - dBinResult);                                             \
  if(error > maxError) {                                                                             \
    throwException(_T("%s %s %s = %s = %23.16le. %23.16le %s %23.16le = %23.16le. Error:%le > %le")  \
                  ,toString(r1).cstr(), _T(#op), toString(r2).cstr()                                 \
                  ,toString(rBinResult).cstr(), dFromR                                               \
                  ,d1, _T(#op), d2, dBinResult                                                       \
                  ,error, maxError                                                                   \
                  );                                                                                 \
  }                                                                                                  \
}

namespace TestRational {		

#include <UnitTestTraits.h>

  TEST_CLASS(TestRational) {
    public:

    TEST_METHOD(RationalTestBasicOperations) {
      randomize();

      //  double maxTotalError = 0;
      for(int i = 0; i < 100000; i++) {
        /*
        if(i % 100 == 99) {
        printf("i:%10d. maxTotalError:%le\r", i, maxTotalError);
        }
        */
        const int nm1 = randInt(-1000, 1000);
        const int dn1 = randInt(1, 1000000);
        const int nm2 = randInt(-1000, 1000);
        const int dn2 = randInt(1, 1000000);

        const Rational r1(nm1, dn1);
        const Rational r2(nm2, dn2);

        const double d1 = getDouble(r1);
        const double d2 = getDouble(r2);

        const bool rgt = r1 >  r2, dgt = d1 >  d2;
        const bool rge = r1 >= r2, dge = d1 >= d2;
        const bool rlt = r1 <  r2, dlt = d1 <  d2;
        const bool rle = r1 <= r2, dle = d1 <= d2;
        const bool req = r1 == r2, deq = d1 == d2;
        const bool rne = r1 != r2, dne = d1 != d2;

        verify(rgt == dgt);
        verify(rge == dge);
        verify(rlt == dlt);
        verify(rle == dle);
        verify(req == deq);
        verify(rne == dne);

        VERIFYOP(+, 1e-13)
        VERIFYOP(-, 1e-13)
        VERIFYOP(*, 1e-14)

        if (!r2.isZero()) {
          VERIFYOP(/ , 3e-9)
        }

        const int      expo = r1.isZero() ? randInt(0, 4) : randInt(-3, 3);
        const Rational r1Pe = pow(r1, expo);
        const Real     d1Pe = mypow(d1, expo);
        Real           error = fabs(getReal(r1Pe) - d1Pe);
        if (d1Pe != 0) error /= d1Pe;
        verify(error < 1e-15);

        const Rational rfd(d1);
        const double   dfr = getDouble(rfd);
        error = fabs(dfr - d1);
        verify(error < 1e-13);
      }
    }

    typedef CompactArray<Rational> CompactRationalArray;

    static CompactRationalArray generateTestArray() {
      CompactRationalArray result;
      for(INT64 d = 1; d >= 0; d = (d + 1) * 31) {
        for(INT64 n = 0; n >= 0; n = (n + 1) * 29) {
          result.add(Rational(n, d));
        }
      }
      const size_t n = result.size();
      CompactRationalArray resultNeg(n);
      for(size_t i = 0; i < n; i++) {
        resultNeg.add(-result[i]);
      }
      result.addAll(resultNeg);
      return result;
    }

    static void testRationalToFromStr(const Rational &r) {
      for(UINT radix = 2; radix <= 36; radix++) {
        TCHAR charBuf[300], *endp;

        const String   str = _rattot(charBuf, r, radix);
        const Rational r1  = _tcstorat(str.cstr(), &endp, radix);
        verify((r1 == r) && (errno == 0));
        verify(endp == str.cstr() + str.length());
      }
    }

    TEST_METHOD(RationalTestToFromString) {

      testRationalToFromStr( RAT_MIN);
      testRationalToFromStr( RAT_MAX);
      testRationalToFromStr(-RAT_MIN);
      testRationalToFromStr(-RAT_MAX);

      const CompactRationalArray a = generateTestArray();
      for(size_t i = 0; i < a.size(); i++) {
        const Rational &r = a[i];
        testRationalToFromStr(r);
      }
    }

    TEST_METHOD(RationalTestIO) {
      try {
        const CompactRationalArray a = generateTestArray();

        StreamParametersIterator it               = StreamParameters::getIntParamIterator(20);
        const UINT               totalFormatCount = (UINT)it.getMaxIterationCount();
        UINT                     formatCounter    = 0;
        while(it.hasNext()) {
          const StreamParameters &param = it.next();
          formatCounter++;
          if(formatCounter % 50 == 0) {
            OUTPUT(_T("%s progress:%.2lf%%"), __TFUNCTION__, PERCENT(formatCounter, totalFormatCount));
          }
          if((param.flags()&ios::adjustfield) == ios::internal) {
            continue;
          }
          const UINT     radix = param.radix();

          ostringstream  costr;
          wostringstream wostr;

//          OUTPUT(_T("formatCounter:%d format:%s"), formatCounter, param.toString().cstr());

          for(size_t i = 0; i < a.size(); i++) { // write signed
            const Rational &x = a[i];
            setFormat(costr, param);
            setFormat(wostr, param);
            costr << x << "\n";
            wostr << x << "\n";
          }
          const string  cstr  = costr.str();
          const wstring wstr = wostr.str();
          verify(String(cstr.c_str()) == String(wstr.c_str()));

          CompactLineArray lineArray(wstr);
          verify((StreamSize)lineArray.minLength() >= param.width());

          istringstream  cistr(cstr);
          wistringstream wistr(wstr);

          StreamParameters ip(param);
          ip.flags(param.flags() | ios::skipws);
          for(size_t i = 0; i < a.size(); i++) {
            const Rational &expected = a[i];

            setFormat(cistr, ip);
            setFormat(wistr, ip);
            if(!iswspace(ip.fill())) {
              skipspace(cistr);
              skipfill(cistr);
              skipspace(wistr);
              skipfill(wistr);
            }
            Rational cx = RAT_NAN;
            Rational wx = RAT_NAN;

            cistr >> cx;
            wistr >> wx;
            verify(cx == expected);
            verify(wx == expected);
          }
        }
      } catch(Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    template<class T> void verifyRationalNanTypes(const T &f) {
      const bool fisNan  = isnan(      f);
      const bool fisInf  = isinf(      f);
      const bool fisPInf = isPInfinity(f);
      const bool fisNInf = isNInfinity(f);
      const Rational r(f);
      verify(isnan(      r)  == fisNan );
      verify(isinf(      r)  == fisInf );
      verify(isPInfinity(r)  == fisPInf);
      verify(isNInfinity(r)  == fisNInf);

      const float    f1  = getFloat(   r);
      const double   d1  = getDouble(  r);
      const Double80 d80 = getDouble80(r);

      verify(isnan(      f1 ) == fisNan );
      verify(isinf(      f1 ) == fisInf );
      verify(isPInfinity(f1 ) == fisPInf);
      verify(isNInfinity(f1 ) == fisNInf);

      verify(isnan(      d1 ) == fisNan );
      verify(isinf(      d1 ) == fisInf );
      verify(isPInfinity(d1 ) == fisPInf);
      verify(isNInfinity(d1 ) == fisNInf);

      verify(isnan(      d80) == fisNan );
      verify(isinf(      d80) == fisInf );
      verify(isPInfinity(d80) == fisPInf);
      verify(isNInfinity(d80) == fisNInf);
    }

#pragma warning(disable : 4723) // Potential divide by 0
#pragma warning(disable : 4756) // overflow in constant arithmetic

    template<class T> void rationalNanTest(const T m) {
      T f = (T)sqrt(-1);
      verifyRationalNanTypes(f);

      f = m;
      f *= 2;
      verifyRationalNanTypes(f);

      f = m;
      f *= -2;
      verifyRationalNanTypes(f);

      f = 1;
      T g = 0;
      f /= g;
      verifyRationalNanTypes(f);

      f = -1;
      f /= g;
      verifyRationalNanTypes(f);

      f =  0;
      f /= g;
      verifyRationalNanTypes(f);
    }

    TEST_METHOD(RationalNaN) {
      try {
        rationalNanTest(FLT_MAX  );
        rationalNanTest(DBL_MAX  );
        rationalNanTest(DBL80_MAX);
      } catch(Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(RationalPowers) {
      try {
        const Rational b(46656,12167000);
        const Rational e(2,3);
        Rational p;
        verify(Rational::isRationalPow(b,e,&p));
        const double pd = pow(getDouble(b),getDouble(e));
        const double error = pd - getDouble(p);
        const double relError = fabs(error / pd);
        verify(relError < 1e-15);
      } catch(Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

  };
}
