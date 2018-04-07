#include "stdafx.h"
#include "CppUnitTest.h"
#include <Math/FPU.h>
#include <Math/Expression/Expression.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static Real relativeDiff(const Real &x, const Real &x0) {
  return (x0 == 0) ? fabs(x) : fabs((x-x0)/x0);
}

#define endl _T("\n")

#define IF(e,e1,e2) ((e)?(e1):(e2))
#define NOT !
#define AND &&
#define OR  ||

#define EPS 1e-8

namespace TestExpression {

#include <UnitTestTraits.h>

  class LOG : public std::wostringstream {
  public:
    ~LOG() {
      OUTPUT(_T("%s"), str().c_str());
    }
  };

  static void vprint(const TCHAR *format, va_list argptr) {
    OUTPUT(_T("%s"),vformat(format,argptr).cstr());
  }

  class ExpressionTest {
  private:
    static bool          s_evaluateTestStarted;
    static bool          s_deriveTestStarted;
    static int           s_instanceCount;
    static int           s_maxInstanceCount;
    static int           s_evaluatedCount;
    static int           s_maxDerivableCount;
    static int           s_derivableCount;
    static int           s_derivedCount;
    const bool           m_derivable : 1;
    bool                 m_evaluated : 1;
    bool                 m_derived   : 1;
  public:
    ExpressionTest(bool derivable) : m_derivable(derivable) {
      m_evaluated = m_derived = false;
      if(++s_instanceCount > s_maxInstanceCount) s_maxInstanceCount = s_instanceCount;
      if(derivable) {
        if(++s_derivableCount > s_maxDerivableCount) s_maxDerivableCount = s_derivableCount;
      }
    }
    virtual ~ExpressionTest();
    void setEvaluated() {
      m_evaluated = true;
      s_evaluatedCount++;
    }
    void setDerived() {
      verify(getReturnType() == EXPR_RETURN_REAL);
      verify(m_derivable);
      m_derived = true;
      s_derivedCount++;
    }
    bool isDerivable() const {
      return m_derivable;
    }
    virtual String getExpr() const = 0;
    virtual ExpressionReturnType getReturnType() const = 0;
    virtual Real fr(const Real &x) const = 0;
    virtual bool fb(const Real &x) const = 0;
    static void startEvaluateTest() {
      s_evaluateTestStarted =  true;
    }
    static void startDeriveTest() {
      s_deriveTestStarted = true;
    }
    Real numDFDX(const Real &x) const {
      verify(getReturnType() == EXPR_RETURN_REAL);
      Real x1, x2;
      if(x == 0) { x1 = EPS/2; x2 = -EPS/2; }
      else { x1 = x*(1+EPS); x2 = x*(1-EPS); }
      const Real y1 = fr(x1), y2=fr(x2);
      return (y1-y2)/(x1-x2);
    }
  };

  bool ExpressionTest::s_evaluateTestStarted = false;
  bool ExpressionTest::s_deriveTestStarted   = false;
  int  ExpressionTest::s_instanceCount       = 0;
  int  ExpressionTest::s_maxInstanceCount    = 0;
  int  ExpressionTest::s_evaluatedCount      = 0;
  int  ExpressionTest::s_maxDerivableCount   = 0;
  int  ExpressionTest::s_derivableCount      = 0;
  int  ExpressionTest::s_derivedCount        = 0;

  class RealExpressionTest : public ExpressionTest {
  public:
    RealExpressionTest(bool derivable = true) : ExpressionTest(derivable) {
    }
    bool fb(const Real &x) const {
      throwException(_T("RealExpressionTest(\"%s\"):fb should not be called"), getExpr());
      return 0;
    }
    ExpressionReturnType getReturnType() const {
      return EXPR_RETURN_REAL;
    }
  };

  class BoolExpressionTest : public ExpressionTest {
  public:
    BoolExpressionTest() : ExpressionTest(false) {
    }
    Real fr(const Real &x) const {
      throwException(_T("BoolExpressionTest(\"%s\"):fr should not be called"), getExpr());
      return 0;
    }
    ExpressionReturnType getReturnType() const {
      return EXPR_RETURN_BOOL;
    }
  };

  ExpressionTest::~ExpressionTest() {
    s_instanceCount--;
    if((s_instanceCount == 0) && s_evaluateTestStarted) {
      const int missingEvaluationCount = s_maxInstanceCount - s_evaluatedCount;
      if(missingEvaluationCount > 0) {
        if(missingEvaluationCount == 1) {
          OUTPUT(_T("1 Expression test has not been evaluated"));
        } else {
          OUTPUT(_T("%d Expression tests have not been evaluated"), missingEvaluationCount);
        }
      }
    }
    if(isDerivable()) {
      s_derivableCount--;
      if((s_derivableCount == 0) && s_deriveTestStarted) {
        const int missingDerivableCount = s_maxDerivableCount - s_derivedCount;
        if(missingDerivableCount > 0) {
          if(missingDerivableCount == 1) {
            OUTPUT(_T("1 Expression test has not been derived"));
          } else {
            OUTPUT(_T("%d Expression tests have not been derived"), missingDerivableCount);
          }
        }
      }
    }
  }

  class Test00 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("1");
    }
    Real fr(const Real &x) const {
      return 1;
    }
  };

  class Test01 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("1+2");
    }
    Real fr(const Real &x) const {
      return 1 + 2;
    }
  };

  class Test02 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x");
    }
    Real fr(const Real &x) const {
      return x;
    }
  };

  class Test03 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x+1");
    }
    Real fr(const Real &x) const {
      return x + 1;
    }
  };

  class Test04 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x-1");
    }
    Real fr(const Real &x) const {
      return x - 1;
    }
  };

  class Test05 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x*2");
    }
    Real fr(const Real &x) const {
      return x * 2;
    }
  };

  class Test06 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x/2");
    }
    Real fr(const Real &x) const {
      return x / 2;
    }
  };

  class Test07 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x^2");
    }
    Real fr(const Real &x) const {
      return x*x;
    }
  };

  class Test08 : public RealExpressionTest {
  public:
    Test08() : RealExpressionTest(false) {
    }
    String getExpr() const {
      return _T("(x+1)%2");
    }
    Real fr(const Real &x) const {
      return fmod(x + 1, 2);
    }
  };

  class Test09 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x^-2");
    }
    Real fr(const Real &x) const {
      return 1 / (x*x);
    }
  };

  class Test10 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("-x");
    }
    Real fr(const Real &x) const {
      return -x;
    }
  };

  class Test11 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("(x+1)+2");
    }
    Real fr(const Real &x) const {
      return (x + 1) + 2;
    }
  };

  class Test12 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("(x+1)-2");
    }
    Real fr(const Real &x) const {
      return (x + 1) - 2;
    }
  };

  class Test13 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("(x+1)*2");
    }
    Real fr(const Real &x) const {
      return (x + 1) * 2;
    }
  };

  class Test14 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("(x+1)/2");
    }
    Real fr(const Real &x) const {
      return (x + 1) / 2;
    }
  };

  class Test15 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("(x+1)^2");
    }
    Real fr(const Real &x) const {
      return (x + 1)*(x + 1);
    }
  };

  class Test16 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x+1+x+2");
    }
    Real fr(const Real &x) const {
      return x + 1 + x + 2;
    }
  };

  class Test17 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("(x+1)-(x+2)");
    }
    Real fr(const Real &x) const {
      return (x + 1) - (x + 2);
    }
  };

  class Test18 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("(x+1)*(x+2)");
    }
    Real fr(const Real &x) const {
      return (x + 1)*(x + 2);
    }
  };

  class Test19 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("(x+1)/(x+2)");
    }
    Real fr(const Real &x) const {
      return (x + 1) / (x + 2);
    }
  };

  class Test20 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("(x+1)^(x+2)");
    }
    Real fr(const Real &x) const {
      return mypow(x + 1, x + 2);
    }
  };

  class Test21 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("(x+1)*(x-1)/(x^2-1)");
    }
    Real fr(const Real &x) const {
      return (x + 1)*(x - 1) / (x*x - 1);
    }
  };

  class Test22 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x/(x*x)*x");
    }
    Real fr(const Real &x) const {
      return x / (x*x)*x;
    }
  };

  class Test23 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("root(x,3)^3");
    }
    Real fr(const Real &x) const {
      return mypow(root(x, 3), 3);
    }
  };

  class Test24 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("root(x*x,2)");
    }
    Real fr(const Real &x) const {
      return root(x*x, 2);
    }
  };

  class Test25 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("sin(x+1)");
    }
    Real fr(const Real &x) const {
      return sin(x + 1);
    }
  };

  class Test26 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("cos(x+1)");
    }
    Real fr(const Real &x) const {
      return cos(x + 1);
    }
  };

  class Test27 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("tan(x+1)");
    }
    Real fr(const Real &x) const {
      return tan(x + 1);
    }
  };

  class Test28 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("cot(x+1)");
    }
    Real fr(const Real &x) const {
      return cot(x + 1);
    }
  };

  class Test29 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("csc(x+1)");
    }
    Real fr(const Real &x) const {
      return csc(x + 1);
    }
  };

  class Test30 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("sec(x+1)");
    }
    Real fr(const Real &x) const {
      return sec(x + 1);
    }
  };

  class Test31 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("asin(x/3)");
    }
    Real fr(const Real &x) const {
      return asin(x / 3);
    }
  };

  class Test32 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("acos(x/3)");
    }
    Real fr(const Real &x) const {
      return acos(x / 3);
    }
  };

  class Test33 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("atan(x+1)");
    }
    Real fr(const Real &x) const {
      return atan(x + 1);
    }
  };

  class Test34 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("atan2(x+7,5)");
    }
    Real fr(const Real &x) const {
      return atan2(x + 7, 5);
    }
  };

  class Test35 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("acot(x+1)");
    }
    Real fr(const Real &x) const {
      return acot(x + 1);
    }
  };

  class Test36 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("acsc(x+4)");
    }
    Real fr(const Real &x) const {
      return acsc(x + 4);
    }
  };

  class Test37 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("asec(x+4)");
    }
    Real fr(const Real &x) const {
      return asec(x + 4);
    }
  };

  class Test38 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("exp(x+1)");
    }
    Real fr(const Real &x) const {
      return exp(x + 1);
    }
  };

  class Test39 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("exp2(x+1)");
    }
    Real fr(const Real &x) const {
      return exp2(x + 1);
    }
  };

  class Test40 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("exp10(x+1)");
    }
    Real fr(const Real &x) const {
      return exp10(x + 1);
    }
  };

  class Test41 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("ln(x+1)");
    }
    Real fr(const Real &x) const {
      return log(x + 1);
    }
  };

  class Test42 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("log10(x+1)");
    }
    Real fr(const Real &x) const {
      return log10(x + 1);
    }
  };

  class Test43 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("log2(x+1)");
    }
    Real fr(const Real &x) const {
      return log2(x + 1);
    }
  };

  class Test44 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("fac(x+1)");
    }
    Real fr(const Real &x) const {
      return fac(x + 1);
    }
  };

  class Test45 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("sqrt(x+1)");
    }
    Real fr(const Real &x) const {
      return sqrt(x + 1);
    }
  };

  class Test46 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("sqr(x+1)");
    }
    Real fr(const Real &x) const {
      return sqr(x + 1);
    }
  };

  class Test47 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("hypot(x+7, x-5)");
    }
    Real fr(const Real &x) const {
      return hypot(x+7, x-5);
    }
  };


  class Test48 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("sinh(x+1)");
    }
    Real fr(const Real &x) const {
      return sinh(x + 1);
    }
  };

  class Test49 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("cosh(x+1)");
    }
    Real fr(const Real &x) const {
      return cosh(x + 1);
    }
  };

  class Test50 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("tanh(x+1)");
    }
    Real fr(const Real &x) const {
      return tanh(x + 1);
    }
  };

  class Test51 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("asinh(x+1)");
    }
    Real fr(const Real &x) const {
      return asinh(x + 1);
    }
  };

  class Test52 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("acosh(x+2)");
    }
    Real fr(const Real &x) const {
      return acosh(x + 2);
    }
  };

  class Test53 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("atanh((x-0.5)/2)");
    }
    Real fr(const Real &x) const {
      return atanh((x - 0.5) / 2);
    }
  };

  class Test54 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("binomial(x+7,5)");
    }
    Real fr(const Real &x) const {
      return binomial(x + 7, 5);
    }
  };

  class Test55 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("gauss(x*2)");
    }
    Real fr(const Real &x) const {
      return gauss(x * 2);
    }
  };

  class Test56 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("gamma(x+2)");
    }
    Real fr(const Real &x) const {
      return gamma(x + 2);
    }
  };

  class Test57 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("erf(x/2.4)");
    }
    Real fr(const Real &x) const {
      return errorFunction(x / 2.4);
    }
  };

  class Test58 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("inverf(x/3)");
    }
    Real fr(const Real &x) const {
      return inverseErrorFunction(x / 3);
    }
  };

  class Test59 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("norm(x/2.4)");
    }
    Real fr(const Real &x) const {
      return norm(x / 2.4);
    }
  };

  class Test60 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("probit((x+0.2)/5)");
    }
    Real fr(const Real &x) const {
      return probitFunction((x + 0.2) / 5);
    }
  };

  class Test61 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("abs(x+1)");
    }
    Real fr(const Real &x) const {
      return fabs(x + 1);
    }
  };

  class Test62 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("floor(x+1)");
    }
    Real fr(const Real &x) const {
      return floor(x + 1);
    }
  };

  class Test63 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("ceil(x+1)");
    }
    Real fr(const Real &x) const {
      return ceil(x + 1);
    }
  };

  class Test64 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("sign(x+1)");
    }
    Real fr(const Real &x) const {
      return dsign(x + 1);
    }
  };

  class Test65 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("cos(x)*-(x)");
    }
    Real fr(const Real &x) const {
      return cos(x)*-(x);
    }
  };

  class Test66 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("max(x,x+1)");
    }
    Real fr(const Real &x) const {
      return dmax(x, x + 1);
    }
  };

  class Test67 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("max(x,x-1)");
    }
    Real fr(const Real &x) const {
      return dmax(x, x - 1);
    }
  };

  class Test68 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("min(x,x+1)");
    }
    Real fr(const Real &x) const {
      return dmin(x, x + 1);
    }
  };

  class Test69 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("min(x,x-1)");
    }
    Real fr(const Real &x) const {
      return dmin(x, x - 1);
    }
  };

  class Test70 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x+1 == x*2,x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(x + 1 == x * 2, x + 1, x - 1);
    }
  };

  class Test71 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x+1 <> x*2,x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(x + 1 != x * 2, x + 1, x - 1);
    }
  };

  class Test72 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x+1 <  x*2,x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(x + 1 <  x * 2, x + 1, x - 1);
    }
  };

  class Test73 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x+1 <= x*2,x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(x + 1 <= x * 2, x + 1, x - 1);
    }
  };

  class Test74 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x+1 >  x*2,x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(x + 1 >  x * 2, x + 1, x - 1);
    }
  };

  class Test75 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x+1 >= x*2,x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(x + 1 >= x * 2, x + 1, x - 1);
    }
  };

  class Test76 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(not(x+1 >= x*2),x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(NOT(x + 1 >= x * 2), x + 1, x - 1);
    }
  };

  class Test77 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x+1 >= x*2 and x > 0 ,x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(x + 1 >= x * 2 AND x > 0, x + 1, x - 1);
    }
  };

  class Test78 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x+1 >= x*2 and x < 0 ,x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(x + 1 >= x * 2 AND x < 0, x + 1, x - 1);
    }
  };

  class Test79 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x+1 >= x*2 or  x > 0 ,x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(x + 1 >= x * 2 OR  x > 0, x + 1, x - 1);
    }
  };

  class Test80 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x+1 >= x*2 or  x < 0 ,x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(x + 1 >= x * 2 OR  x < 0, x + 1, x - 1);
    }
  };

  class Test81 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(not(x+1 >= x*2 and x > 0),x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(NOT(x + 1 >= x * 2 AND x > 0), x + 1, x - 1);
    }
  };

  class Test82 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(not(x+1 >= x*2 and x < 0),x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(NOT(x + 1 >= x * 2 AND x < 0), x + 1, x - 1);
    }
  };

  class Test83 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(not(x+1 >= x*2 or  x > 0),x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(NOT(x + 1 >= x * 2 OR  x > 0), x + 1, x - 1);
    }
  };

  class Test84 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(not(x+1 >= x*2 or  x < 0),x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(NOT(x + 1 >= x * 2 OR  x < 0), x + 1, x - 1);
    }
  };

  class Test85 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(not(x+1 >= x*2 or  x < 0 or x == 0),x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(NOT(x + 1 >= x * 2 OR  x < 0 OR x == 0), x + 1, x - 1);
    }
  };

  class Test86 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(not(x+1 >= x*2 and x < 0 or x == 0),x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(NOT(x + 1 >= x * 2 AND x < 0 OR x == 0), x + 1, x - 1);
    }
  };

  class Test87 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(not((x < 0 or x == 0) and x+1 >= x*2),x+1,x-1)");
    }
    Real fr(const Real &x) const {
      return IF(NOT((x < 0 OR x == 0) AND x + 1 >= x * 2), x + 1, x - 1);
    }
  };

  class Test88 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x+x");
    }
    Real fr(const Real &x) const {
      return x + x;
    }
  };

  class Test89 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x+2*x");
    }
    Real fr(const Real &x) const {
      return x + 2 * x;
    }
  };

  class Test90 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("3*x-4*x");
    }
    Real fr(const Real &x) const {
      return 3 * x - 4 * x;
    }
  };

  class Test91 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("x/sqrt(x)");
    }
    Real fr(const Real &x) const {
      return x / sqrt(x);
    }
  };

  class Test92 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("sqrt(sqrt(x)) / x / sqrt(x)");
    }
    Real fr(const Real &x) const {
      return sqrt(sqrt(x)) / x / sqrt(x);
    }
  };

  class Test93 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("poly[1,poly[2,-1,3](x),3,4](x)");
    }
    Real fr(const Real &x) const {
      Real c1[3] = { 2,-1,3 };
      Real c2[4] = { 1,0,3,4 };
      c2[1] = poly1(x, 2, c1);
      return poly1(x, 3, c2);
    }
  };

  class Test94 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("sum(i=0 to 10) x^i/fac(i)");
    }
    Real fr(const Real &x) const {
      Real sum = 0;
      for(int i = 0; i <= 10; i++)
        sum += mypow(x, i) / fac(i);
      return sum;
    }
  };

  class Test95 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("product(i=1 to 10) (x+i+0.25)/(x+i+1)");
    }
    Real fr(const Real &x) const {
      Real product = 1;
      for (int i = 1; i <= 10; i++)
        product *= (x + i + 0.25) / (x + i + 1);
      return product;
    }
  };

  class Test96 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("root(x+1,root(2,x))");
    }
    Real fr(const Real &x) const {
      return root(x + 1, root(2, x));
    }
  };

  class Test97 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("a = x^2; a^2");
    }
    Real fr(const Real &x) const {
      Real a = x*x;
      return a*a;
    }
  };

  class Test98 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("[x > 1]");
    }
    Real fr(const Real &x) const {
      return x > 1 ? 1 : 0;
    }
  };

  class Test99 : public BoolExpressionTest {
  public:
    String getExpr() const {
      return _T("x > 1");
    }
    bool fb(const Real &x) const {
      return x > 1;
    }
  };

  class Test100 : public BoolExpressionTest {
  public:
    String getExpr() const {
      return _T("x < 1");
    }
    bool fb(const Real &x) const {
      return x < 1;
    }
  };

  class Test101 : public BoolExpressionTest {
  public:
    String getExpr() const {
      return _T("x != 1");
    }
    bool fb(const Real &x) const {
      return x != 1;
    }
  };

  class Test102 : public BoolExpressionTest {
  public:
    String getExpr() const {
      return _T("x*x < 1");
    }
    bool fb(const Real &x) const {
      return x*x < 1;
    }
  };

  class Test103 : public BoolExpressionTest {
  public:
    String getExpr() const {
      return _T("y=1; x+y < 1");
    }
    bool fb(const Real &x) const {
      double y = 1;
      return x + y < 1;
    }
  };

  class Test104 : public BoolExpressionTest {
  public:
    String getExpr() const {
      return _T("1 < x && x < 2");
    }
    bool fb(const Real &x) const {
      return 1 < x && x < 2;
    }
  };

  class Test105 : public BoolExpressionTest {
  public:
    String getExpr() const {
      return _T("1 < x || x > 2");
    }
    bool fb(const Real &x) const {
      return 1 < x || x > 2;
    }
  };

  class Test106 : public RealExpressionTest {
  public:
    String getExpr() const {
      return _T("if(x==1,sum(i=1 to 10) sin(x)^2+cos(i*2+0.25)/e^(x+i+1)"
                "       ,sum(i=1 to 10) (x+i+0.55)/(x+i+1)"
                "  )"
               );
    }
    Real fr(const Real &x) const {
      Real sum = 0;
      if(x==1) {
        for (int i = 1; i <= 10; i++)
          sum += sqr(sin(x)) + cos(i*2 + 0.25) / exp(x + i + 1);
      } else {
        for (int i = 1; i <= 10; i++)
          sum += (x + i + 0.55) / (x + i + 1);
      }
      return sum;
    }
  };

  static Test00  test00;
  static Test01  test01;
  static Test02  test02;
  static Test03  test03;
  static Test04  test04;
  static Test05  test05;
  static Test06  test06;
  static Test07  test07;
  static Test08  test08;
  static Test09  test09;
  static Test10  test10;
  static Test11  test11;
  static Test12  test12;
  static Test13  test13;
  static Test14  test14;
  static Test15  test15;
  static Test16  test16;
  static Test17  test17;
  static Test18  test18;
  static Test19  test19;
  static Test20  test20;
  static Test21  test21;
  static Test22  test22;
  static Test23  test23;
  static Test24  test24;
  static Test25  test25;
  static Test26  test26;
  static Test27  test27;
  static Test28  test28;
  static Test29  test29;
  static Test30  test30;
  static Test31  test31;
  static Test32  test32;
  static Test33  test33;
  static Test34  test34;
  static Test35  test35;
  static Test36  test36;
  static Test37  test37;
  static Test38  test38;
  static Test39  test39;
  static Test40  test40;
  static Test41  test41;
  static Test42  test42;
  static Test43  test43;
  static Test44  test44;
  static Test45  test45;
  static Test46  test46;
  static Test47  test47;
  static Test48  test48;
  static Test49  test49;
  static Test50  test50;
  static Test51  test51;
  static Test52  test52;
  static Test53  test53;
  static Test54  test54;
  static Test55  test55;
  static Test56  test56;
  static Test57  test57;
  static Test58  test58;
  static Test59  test59;
  static Test60  test60;
  static Test61  test61;
  static Test62  test62;
  static Test63  test63;
  static Test64  test64;
  static Test65  test65;
  static Test66  test66;
  static Test67  test67;
  static Test68  test68;
  static Test69  test69;
  static Test70  test70;
  static Test71  test71;
  static Test72  test72;
  static Test73  test73;
  static Test74  test74;
  static Test75  test75;
  static Test76  test76;
  static Test77  test77;
  static Test78  test78;
  static Test79  test79;
  static Test80  test80;
  static Test81  test81;
  static Test82  test82;
  static Test83  test83;
  static Test84  test84;
  static Test85  test85;
  static Test86  test86;
  static Test87  test87;
  static Test88  test88;
  static Test89  test89;
  static Test90  test90;
  static Test91  test91;
  static Test92  test92;
  static Test93  test93;
  static Test94  test94;
  static Test95  test95;
  static Test96  test96;
  static Test97  test97;
  static Test98  test98;
  static Test99  test99;
  static Test100 test100;
  static Test101 test101;
  static Test102 test102;
  static Test103 test103;
  static Test104 test104;
  static Test105 test105;
  static Test106 test106;

  static ExpressionTest *testCases[] = {
     (ExpressionTest*)&test00
    ,(ExpressionTest*)&test01
    ,(ExpressionTest*)&test02
    ,(ExpressionTest*)&test03
    ,(ExpressionTest*)&test04
    ,(ExpressionTest*)&test05
    ,(ExpressionTest*)&test06
    ,(ExpressionTest*)&test07
    ,(ExpressionTest*)&test08
    ,(ExpressionTest*)&test09
    ,(ExpressionTest*)&test10
    ,(ExpressionTest*)&test11
    ,(ExpressionTest*)&test12
    ,(ExpressionTest*)&test13
    ,(ExpressionTest*)&test14
    ,(ExpressionTest*)&test15
    ,(ExpressionTest*)&test16
    ,(ExpressionTest*)&test17
    ,(ExpressionTest*)&test18
    ,(ExpressionTest*)&test19
    ,(ExpressionTest*)&test20
    ,(ExpressionTest*)&test21
    ,(ExpressionTest*)&test22
    ,(ExpressionTest*)&test23
    ,(ExpressionTest*)&test24
    ,(ExpressionTest*)&test25
    ,(ExpressionTest*)&test26
    ,(ExpressionTest*)&test27
    ,(ExpressionTest*)&test28
    ,(ExpressionTest*)&test29
    ,(ExpressionTest*)&test30
    ,(ExpressionTest*)&test31
    ,(ExpressionTest*)&test32
    ,(ExpressionTest*)&test33
    ,(ExpressionTest*)&test34
    ,(ExpressionTest*)&test35
    ,(ExpressionTest*)&test36
    ,(ExpressionTest*)&test37
    ,(ExpressionTest*)&test38
    ,(ExpressionTest*)&test39
    ,(ExpressionTest*)&test40
    ,(ExpressionTest*)&test41
    ,(ExpressionTest*)&test42
    ,(ExpressionTest*)&test43
    ,(ExpressionTest*)&test44
    ,(ExpressionTest*)&test45
    ,(ExpressionTest*)&test46
    ,(ExpressionTest*)&test47
    ,(ExpressionTest*)&test48
    ,(ExpressionTest*)&test49
    ,(ExpressionTest*)&test50
    ,(ExpressionTest*)&test51
    ,(ExpressionTest*)&test52
    ,(ExpressionTest*)&test53
    ,(ExpressionTest*)&test54
    ,(ExpressionTest*)&test55
    ,(ExpressionTest*)&test56
    ,(ExpressionTest*)&test57
    ,(ExpressionTest*)&test58
    ,(ExpressionTest*)&test59
    ,(ExpressionTest*)&test60
    ,(ExpressionTest*)&test61
    ,(ExpressionTest*)&test62
    ,(ExpressionTest*)&test63
    ,(ExpressionTest*)&test64
    ,(ExpressionTest*)&test65
    ,(ExpressionTest*)&test66
    ,(ExpressionTest*)&test67
    ,(ExpressionTest*)&test68
    ,(ExpressionTest*)&test69
    ,(ExpressionTest*)&test70
    ,(ExpressionTest*)&test71
    ,(ExpressionTest*)&test72
    ,(ExpressionTest*)&test73
    ,(ExpressionTest*)&test74
    ,(ExpressionTest*)&test75
    ,(ExpressionTest*)&test76
    ,(ExpressionTest*)&test77
    ,(ExpressionTest*)&test78
    ,(ExpressionTest*)&test79
    ,(ExpressionTest*)&test80
    ,(ExpressionTest*)&test81
    ,(ExpressionTest*)&test82
    ,(ExpressionTest*)&test83
    ,(ExpressionTest*)&test84
    ,(ExpressionTest*)&test85
    ,(ExpressionTest*)&test86
    ,(ExpressionTest*)&test87
    ,(ExpressionTest*)&test88
    ,(ExpressionTest*)&test89
    ,(ExpressionTest*)&test90
    ,(ExpressionTest*)&test91
    ,(ExpressionTest*)&test92
    ,(ExpressionTest*)&test93
    ,(ExpressionTest*)&test94
    ,(ExpressionTest*)&test95
    ,(ExpressionTest*)&test96
    ,(ExpressionTest*)&test97
    ,(ExpressionTest*)&test98
    ,(ExpressionTest*)&test99
    ,(ExpressionTest*)&test100
    ,(ExpressionTest*)&test101
    ,(ExpressionTest*)&test102
    ,(ExpressionTest*)&test103
    ,(ExpressionTest*)&test104
    ,(ExpressionTest*)&test105
    ,(ExpressionTest*)&test106
  };

	TEST_CLASS(TestExpression) {
    public:

    TEST_METHOD(ExpressionTestEvaluate) {
      ExpressionTest::startEvaluateTest();
      FPU::init();
//      redirectDebugLog();
      try {
        for(int i = 0; i < ARRAYSIZE(testCases); i++) {
          ExpressionTest &test = *testCases[i];
          const String expr = test.getExpr();
//            tcout << _T("Test[") << i << _T("]:") << expr << spaceString(40) << _T("\r");
          Expression compiledExpr, interpreterExpr;
          compiledExpr.compile(expr, true);

//          debugLog(_T("Test %d %s\n%s\n"), i, expr.cstr(), compiledExpr.treeToString().cstr());

          interpreterExpr.compile(expr, false);
          if(!compiledExpr.isOk()) {
            OUTPUT(_T("Error in testcase[%d]<%s>"), i, expr.cstr());
            const StringArray &errors = compiledExpr.getErrors();
            for(size_t i = 0; i < errors.size(); i++) {
              OUTPUT(_T("%s"), errors[i].cstr());
            }
            verify(false);
          } else {
            verify(compiledExpr.getReturnType()    == test.getReturnType());
            verify(interpreterExpr.getReturnType() == test.getReturnType());
            for(Real x = -2; x <= 2; x += 0.5) {
              compiledExpr.setValue(   _T("x"), x);
              interpreterExpr.setValue(_T("x"), x);
              switch(compiledExpr.getReturnType()) {
              case EXPR_RETURN_REAL:
                { const Real cppResult          = test.fr(x);
                  const Real compiledResult     = compiledExpr.evaluate();
                  const Real interpreterResult  = interpreterExpr.evaluate();
                  const bool cppDefined         = !isNan(cppResult);
                  const bool compiledDefined    = !isNan(compiledResult);
                  const bool interpreterDefined = !isNan(interpreterResult);

                  if((compiledDefined != cppDefined) || (compiledDefined && fabs(compiledResult - cppResult) > 3e-15)) {
                    LOG log;
                    log << _T("TestCase[") << i << _T("]:<") << expr << _T(">(x=") << toString(x) << _T(") failed.") << endl
                        << _T("Result(C++          ):") << toString(cppResult                   ) << _T(".") << endl
                        << _T("Result(Compiled     ):") << toString(compiledResult              ) << _T(".") << endl
                        << _T("Result(Interpreter  ):") << toString(interpreterResult           ) << _T(".") << endl
                        << _T("Difference(comp-C++ ):") << toString(compiledResult - cppResult  ) << _T(".") << endl;
                    verify(false);
                  }
                  if((interpreterDefined != cppDefined) || (interpreterDefined && fabs(interpreterResult - cppResult) > 3e-15)) {
                    LOG log;
                    log << _T("TestCase[") << i << _T("]:<") << expr << _T(">(x=") << toString(x)  << _T(") failed.") << endl
                        << _T("Result(C++          ):") << toString(cppResult                    ) << _T(".") << endl
                        << _T("Result(Interpreter  ):") << toString(interpreterResult            ) << _T(".") << endl
                        << _T("Result(Compiled     ):") << toString(compiledResult               ) << _T(".") << endl
                        << _T("Difference(intp-C++ ):") << toString(interpreterResult - cppResult) << _T(".") << endl;
                    verify(false);
                  }
                }
                break;
              case EXPR_RETURN_BOOL:
                { const bool compiledResult    = compiledExpr.evaluateBool();
                  const bool interpreterResult = interpreterExpr.evaluateBool();
                  const bool cppResult         = test.fb(x);
                  if((compiledResult != cppResult) || (interpreterResult != cppResult)) {
                    LOG log;
                    log << _T("TestCase[") << i << _T("]:<") << expr << _T(">(x=") << toString(x) << _T(") failed.") << endl
                        << _T("Result(C++          ):") << toString(cppResult        ) << _T(".") << endl
                        << _T("Result(Compiled     ):") << toString(compiledResult   ) << _T(".") << endl
                        << _T("Result(Interpreter  ):") << toString(interpreterResult) << _T(".") << endl;
                    verify(false);
                  }
                }
                break;
              } // switch
            } // for(x..
            test.setEvaluated();
          } // else
        } // for(i...
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(ExpressionTestPow) {
      FPU::init();
      String str;
      try {
        for(Real p = -70; p <= 70; p += 0.5) {
          str = format(_T("(1+x)^%s"),toString(p).cstr());
          Expression compiledExpr, interpreterExpr;
          compiledExpr.compile(   str, true );
          interpreterExpr.compile(str, false);
          verify(compiledExpr.isOk());
          verify(interpreterExpr.isOk());
          const Real startx = (p == getInt(p)) ? -1.9 : -0.9, step = 0.125;
          for(Real x = startx; x <= 0.5; x += step) {
            compiledExpr.setValue(   _T("x"),x);
            interpreterExpr.setValue(_T("x"),x);
            const Real y1 = mypow((1+x),p);
            const Real y2 = compiledExpr.evaluate();
            const Real y3 = interpreterExpr.evaluate();
            verify(relativeDiff(y2,y1) < 1e-13);
            verify(relativeDiff(y3,y1) < 1e-13);
          }
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        OUTPUT(_T("str:[%s]"    ), str.cstr());
        verify(false);
      }
    }

#ifdef TEST_DERIVATIVES
    TEST_METHOD(ExpressionTestDerive) {
      ExpressionTest::startDeriveTest();
      FPU::init();
//      redirectDebugLog();
      try {
        for(int i = 0; i < ARRAYSIZE(testCases); i++) {
          ExpressionTest &test = *testCases[i];
          if(!test.isDerivable()) {
            continue;
          }
          const String expr = test.getExpr();
          Expression compiledExpr, interpreterExpr;
          compiledExpr.compile(expr, true);

//          OUTPUT(_T("Test %d %s\n%s\n"), i, expr.cstr(), compiledExpr.treeToString().cstr());

          interpreterExpr.compile(expr, false);
          if(!compiledExpr.isOk()) {
            OUTPUT(_T("Error in testcase[%d]<%s>"), i, expr.cstr());
            const StringArray &errors = compiledExpr.getErrors();
            for(size_t i = 0; i < errors.size(); i++) {
              OUTPUT(_T("%s"), errors[i].cstr());
            }
            verify(false);
          }
          verify(compiledExpr.getReturnType()    == EXPR_RETURN_REAL);
          verify(interpreterExpr.getReturnType() == EXPR_RETURN_REAL);
          Expression compiledDFDX    = compiledExpr.getDerived(   _T("x"),false);
          Expression interpreterDFDX = interpreterExpr.getDerived(_T("x"),false);
          verify(compiledDFDX.isOk());
          verify(interpreterDFDX.isOk());
          for(Real x = -2; x <= 2; x += 0.31) {
            const Real y = test.fr(x);
            if(isNan(y)) continue;

            compiledDFDX.setValue(   _T("x"), x);
            interpreterDFDX.setValue(_T("x"), x);
            const Real cppResult          = test.numDFDX(x);
            const Real compiledResult     = compiledDFDX.evaluate();
            const Real interpreterResult  = interpreterDFDX.evaluate();
            const bool cppDefined         = !isNan(cppResult);
            const bool interpreterDefined = !isNan(interpreterResult);
            const bool compiledDefined    = !isNan(compiledResult);
            Real       relDiff            = 0;
#define USE_ABSERROR
#ifdef USE_ABSERROR
#define CALCERROR(x,x0) fabs((x)-(x0))
#else
#define CALCERROR(x,x0) relativeDiff(x,x0)
#endif

            if((compiledDefined != cppDefined) || (compiledDefined && ((relDiff=CALCERROR(compiledResult,cppResult)) > 5e-6))) {
              LOG log;
              log << _T("TestCase[") << i << _T("]:<") << expr << _T("'>(x=") << toString(x) << _T(") failed.") << endl
                  << _T("f'(x) = ")  << compiledDFDX.toString() << endl
                  << _T("Defined(C++         ):") << boolToStr(cppDefined        ) << _T(".") << endl
                  << _T("Defined(Compiled    ):") << boolToStr(compiledDefined   ) << _T(".") << endl
                  << _T("Defined(Interpreter ):") << boolToStr(interpreterDefined) << _T(".") << endl
                  << _T("Result(C++          ):") << toString(cppResult          ) << _T(".") << endl
                  << _T("Result(Compiled     ):") << toString(compiledResult     ) << _T(".") << endl
                  << _T("Result(Interpreter  ):") << toString(interpreterResult  ) << _T(".") << endl
                  << _T("Difference(comp-C++ ):") << toString(relDiff            ) << _T(".") << endl;
              verify(false);
            }
            if((interpreterDefined != cppDefined) || (interpreterDefined && ((relDiff=CALCERROR(interpreterResult,cppResult)) > 5e-6))) {
              LOG log;
              log << _T("TestCase[") << i << _T("]:<") << expr << _T("'>(x=") << toString(x)  << _T(") failed.") << endl
                  << _T("Defined(C++         ):") << boolToStr(cppDefined        ) << _T(".") << endl
                  << _T("Defined(Compiled    ):") << boolToStr(compiledDefined   ) << _T(".") << endl
                  << _T("Defined(Interpreter ):") << boolToStr(interpreterDefined) << _T(".") << endl
                  << _T("Result(C++          ):") << toString(cppResult          ) << _T(".") << endl
                  << _T("Result(Interpreter  ):") << toString(interpreterResult  ) << _T(".") << endl
                  << _T("Result(Compiled     ):") << toString(compiledResult     ) << _T(".") << endl
                  << _T("Difference(intp-C++ ):") << toString(relDiff            ) << _T(".") << endl;
              verify(false);
            }
          } // for(x..
          test.setDerived();
        } // for(i...
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }
#endif // TEST_DERIVATIVES

    TEST_METHOD(TestMachineCode) {
      try {
        generateTestSequence(vprint);
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
      }
    }
    TEST_METHOD(CallAssemblerCode) {
      callAssemblerCode();
    }

  };
}
