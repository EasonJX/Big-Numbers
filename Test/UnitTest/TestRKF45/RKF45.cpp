#include "stdafx.h"
#include "CppUnitTest.h"
#include <Math/MathLib.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef verify
#undef verify
#endif
#define verify(expr) Assert::IsTrue(expr, _T(#expr))

namespace TestRKF45 {

  void OUTPUT(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    Logger::WriteMessage(msg.cstr());
  }

  /* Example equation
    "A first course in numerical analysis", Ralston & Rabinowitz 2. ed. p. 231
  */
  class ral231 : public VectorFunction {
  public:
    Vector operator()(const Vector &v);
  };

  Vector ral231::operator()( const Vector &v ) {
    Vector dv(v.getDimension());
    dv[0] = 0.0;
    dv[1] = 0.01 - (0.01 + v[1] + v[2]) * (1.0 + (v[1]+1000.0)*(v[1]+1.0));
    dv[2] = 0.01 - (0.01 + v[1] + v[2]) * (1.0 + v[2]*v[2]);
    return dv;
  }

  // f' = f, f(0) = 1.  -> f(x) = exp(x)
  class Expdiff : public VectorFunction {
  public:
    Vector operator()(const Vector &v);
  };

  Vector Expdiff::operator()( const Vector &v ) {
    Vector dv(v.getDimension());
    dv[0] = 0.0;
    dv[1] = v[1];
    return dv;
  }

  class ValueHandler : public RungeKuttaFehlbergHandler {
  private:

  public:
    void handleData(const RungeKuttaFehlberg &data);
  };

  void ValueHandler::handleData(const RungeKuttaFehlberg &data) {
    OUTPUT(_T("%s"), data.getvalue().toString().cstr());
  }

	TEST_CLASS(RKF45)	{
	public:

		TEST_METHOD(RKF45_1) {
      Vector start(2);

      start[0] = 0;
      start[1] = 1;

      RungeKuttaFehlberg(Expdiff(), ValueHandler()).calculate(start, 1.0, 0.01);
		}
	};
}