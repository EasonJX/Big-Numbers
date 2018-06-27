#include "stdafx.h"
#include "CppUnitTest.h"
#include <Math/FPU.h>
#include "TestSamples.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static Real relativeDiff(const Real &x, const Real &x0) {
  return (x0 == 0) ? fabs(x) : fabs((x-x0)/x0);
}

#define endl _T("\n")

using namespace Expr;

namespace TestExpression {

#include <UnitTestTraits.h>

  class LOG : public std::wostringstream {
  public:
    ~LOG() {
      OUTPUT(_T("%s"), str().c_str());
    }
  };

//#define TEST_DERIVATIVES

  static void vprint(const TCHAR *format, va_list argptr) {
    OUTPUT(_T("%s"),vformat(format,argptr).cstr());
  }

  String makeFileName(int testCase, const String dirComp1, const String &dirComponent = EMPTYSTRING) {
    const String fileName = Expression::getDefaultListFileName();
    FileNameSplitter fs(fileName);
    DirComponents dc = fs.getDirComponents();
    dc.add(2,dirComp1);
    if(dirComponent.length() > 0) {
      dc.add(3,dirComponent);
    }
    fs.setDir(dc);
    fs.setFileName(format(_T("testCase%04d"), testCase));
    return fs.getFullPath();
  }

  FILE *openEvalListFile(       int testCase) { return MKFOPEN(makeFileName(testCase, _T("eval")                     ), _T("w")); }
  FILE *openEvalReducedListFile(int testCase) { return MKFOPEN(makeFileName(testCase, _T("eval"), _T("reduced"      )), _T("w")); }
  FILE *openEvalDumpFile(       int testCase) { return MKFOPEN(makeFileName(testCase, _T("eval"), _T("dump"         )), _T("w")); }
  FILE *openEvalReducedDumpFile(int testCase) { return MKFOPEN(makeFileName(testCase, _T("eval"), _T("reduced\\dump")), _T("w")); }

  FILE *openBoolListFile(       int testCase) { return MKFOPEN(makeFileName(testCase, _T("bool")                     ), _T("w")); }
  FILE *openBoolReducedListFile(int testCase) { return MKFOPEN(makeFileName(testCase, _T("bool"), _T("reduced"      )), _T("w")); }
  FILE *openBoolDumpFile(       int testCase) { return MKFOPEN(makeFileName(testCase, _T("bool"), _T("dump"         )), _T("w")); }
  FILE *openBoolReducedDumpFile(int testCase) { return MKFOPEN(makeFileName(testCase, _T("bool"), _T("reduced\\dump")), _T("w")); }

  typedef enum {
    CMP_EQUAL
   ,CMP_EQUALMINUS
   ,CMP_NOTEQUAL
  } CompareResult;

  class EqualExpressionSample {
  public:
    const String        m_e1,m_e2;
    const CompareResult m_expected;
    EqualExpressionSample(const String &e1, const String &e2, CompareResult expected)
      : m_e1(e1)
      , m_e2(e2)
      , m_expected(expected)
    {
    }
    void doTest() const;
  };

  void EqualExpressionSample::doTest() const {
    Expression exp1, exp2;
    exp1.compile(m_e1, false);
    verify(exp1.isOk());
    exp2.compile(m_e1, false);
    verify(exp2.isOk());
    switch(m_expected) {
    case CMP_EQUAL     :
      verify(exp1.equal(exp2));
      break;
    case CMP_EQUALMINUS:
      verify(exp1.equalMinus(exp2));
      break;
    case CMP_NOTEQUAL  :
      verify(!exp1.equal(     exp2));
      verify(!exp1.equalMinus(exp2));
      break;
    }
  }

	TEST_CLASS(TestExpression) {
    public:

    TEST_METHOD(SNodeEqual) {
      static const EqualExpressionSample testData[] = {
        EqualExpressionSample("x"        ,"x"                       ,CMP_EQUAL     )
       ,EqualExpressionSample("x"        ,"-x"                      ,CMP_EQUALMINUS)
       ,EqualExpressionSample("1"        ,"1"                       ,CMP_EQUAL     )
       ,EqualExpressionSample("1"        ,"-1"                      ,CMP_EQUALMINUS)
       ,EqualExpressionSample("abs(x)"   ,"abs(-x)"                 ,CMP_EQUAL     )
       ,EqualExpressionSample("cos(x)"   ,"cos(-x)"                 ,CMP_EQUAL     )
       ,EqualExpressionSample("cosh(x)"  ,"cosh(-x)"                ,CMP_EQUAL     )
       ,EqualExpressionSample("gauss(x)" ,"gauss(-x)"               ,CMP_EQUAL     )
       ,EqualExpressionSample("sec(x)"   ,"sec(-x)"                 ,CMP_EQUAL     )
       ,EqualExpressionSample("sqr(x)"   ,"sqr(-x)"                 ,CMP_EQUAL     )
       ,EqualExpressionSample("acsc(x)"  ,"acsc(-x)"                ,CMP_EQUALMINUS)
       ,EqualExpressionSample("asin(x)"  ,"asin(-x)"                ,CMP_EQUALMINUS)
       ,EqualExpressionSample("asinh(x)" ,"asinh(-x)"               ,CMP_EQUALMINUS)
       ,EqualExpressionSample("atan(x)"  ,"atan(-x)"                ,CMP_EQUALMINUS)
       ,EqualExpressionSample("atanh(x)" ,"atanh(-x)"               ,CMP_EQUALMINUS)
       ,EqualExpressionSample("cot(x)"   ,"cot(-x)"                 ,CMP_EQUALMINUS)
       ,EqualExpressionSample("csc(x)"   ,"csc(-x)"                 ,CMP_EQUALMINUS)
       ,EqualExpressionSample("erf(x)"   ,"erf(-x)"                 ,CMP_EQUALMINUS)
       ,EqualExpressionSample("inverf(x)","inverf(-x)"              ,CMP_EQUALMINUS)
       ,EqualExpressionSample("sign(x)"  ,"sign(-x)"                ,CMP_EQUALMINUS)
       ,EqualExpressionSample("sin(x)"   ,"sin(-x)"                 ,CMP_EQUALMINUS)
       ,EqualExpressionSample("sinh(x)"  ,"sinh(-x)"                ,CMP_EQUALMINUS)
       ,EqualExpressionSample("tan(x)"   ,"tan(-x)"                 ,CMP_EQUALMINUS)
       ,EqualExpressionSample("tanh(x)"  ,"tanh(-x)"                ,CMP_EQUALMINUS)

      };
      for(size_t i = 0; i < ARRAYSIZE(testData); i++) {
        const EqualExpressionSample &sample = testData[i];
      }
    };

    TEST_METHOD(ExpressionTestEvaluate) {
      ExpressionTest::startEvaluateTest(OUTPUT);
      FPU::init();
#ifdef TRACE_MEMORY
//redirectDebugLog();
#endif
      try {
        const CompactArray<ExpressionTest*> &testArray = ExpressionTest::getAllSamples();
        const size_t                         n         = testArray.size();
        for(UINT i = 0; i < n; i++) {
          ExpressionTest &test = *testArray[i];
          const String    expr = test.getExpr();
#ifdef TRACE_MEMORY
          debugLog(_T("testcase %3d:<%-50s>\n"),i,expr.cstr());
#endif
          OUTPUT(_T("Test[%d]:%s"), i, expr.cstr());
          FILE      * listFile   = openEvalListFile(       i);
          FILE      *RlistFile   = openEvalReducedListFile(i);
          FILE      * dumpFile   = openEvalDumpFile(       i);
          FILE      *RdumpFile   = openEvalReducedDumpFile(i);
          Expression compiledExpr, interpreterExpr, reducedExpr;
          compiledExpr.compile(expr, true,false, listFile);
          reducedExpr.compile( expr, true,true ,RlistFile);

          _ftprintf( dumpFile, _T("%s\n%s\n"), expr.cstr(),compiledExpr.treeToString().cstr());
          _ftprintf(RdumpFile, _T("%s\n%s\n"), expr.cstr(),reducedExpr.treeToString().cstr());

          fclose(RdumpFile);
          fclose( dumpFile);
          fclose(RlistFile);
          fclose( listFile);

//          debugLog(_T("Test %d %s\n%s\n"), i, expr.cstr(), compiledExpr.treeToString().cstr());

          interpreterExpr.compile(expr, false,false);
          if(!compiledExpr.isOk()) {
            OUTPUT(_T("Error in testcase[%d]<%s>"), i, expr.cstr());
            const StringArray &errors = compiledExpr.getErrors();
            for(size_t i = 0; i < errors.size(); i++) {
              OUTPUT(_T("%s"), errors[i].cstr());
            }
            verify(false);
          } else {
            verify(reducedExpr.isOk());
            verify(interpreterExpr.isOk());
            verify(compiledExpr.getReturnType()    == test.getReturnType());
            verify(reducedExpr.getReturnType()     == test.getReturnType());
            verify(interpreterExpr.getReturnType() == test.getReturnType());
            for(Real x = -1.91; x <= 2; x += 0.12489) {
              compiledExpr.setValue(   _T("x"), x);
              reducedExpr.setValue(    _T("x"), x);
              interpreterExpr.setValue(_T("x"), x);
              switch(compiledExpr.getReturnType()) {
              case EXPR_RETURN_REAL:
                { const Real cppResult          = test.fr(x);
                  const Real compiledResult     = compiledExpr.evaluate();
                  const Real reducedResult      = reducedExpr.evaluate();
                  const Real interpreterResult  = interpreterExpr.evaluate();
                  const bool cppDefined         = !isNan(cppResult        );
                  const bool compiledDefined    = !isNan(compiledResult   );
                  const bool reducedDefined     = !isNan(reducedResult    );
                  const bool interpreterDefined = !isNan(interpreterResult);

#define LOGERROR()                                                                                         \
{ LOG log;                                                                                                 \
  log << _T("TestCase[") << i << _T("]:<") << expr << _T(">(x=") << toString(x) << _T(") failed.") << endl \
      << _T("Result(C++          ):") << toString(cppResult                   ) << _T(".") << endl         \
      << _T("Result(Compiled     ):") << toString(compiledResult              ) << _T(".") << endl         \
      << _T("Result(Reduced      ):") << toString(reducedResult               ) << _T(".") << endl         \
      << _T("Result(Interpreter  ):") << toString(interpreterResult           ) << _T(".") << endl         \
      << _T("Difference(comp-C++ ):") << toString(compiledResult - cppResult  ) << _T(".") << endl;        \
}

                  if((compiledDefined    != cppDefined) || (compiledDefined && fabs(compiledResult - cppResult) > 3e-15)) {
                    LOGERROR();
                    verify(false);
                  }
                  if((reducedDefined     != cppDefined) || (reducedDefined  && fabs(reducedResult  - cppResult) > 3e-15)) {
                    LOGERROR();
                    verify(false);
                  }
                  if((interpreterDefined != cppDefined) || (interpreterDefined && fabs(interpreterResult - cppResult) > 3e-15)) {
                    LOGERROR();
                    verify(false);
                  }
                }
                break;
              case EXPR_RETURN_BOOL:
                { const bool cppResult         = test.fb(x);
                  const bool compiledResult    = compiledExpr.evaluateBool();
                  const bool reducedResult     = reducedExpr.evaluateBool();
                  const bool interpreterResult = interpreterExpr.evaluateBool();
                  if((compiledResult    != cppResult)
                  || (reducedResult     != cppResult)
                  || (interpreterResult != cppResult)) {
                    LOG log;
                    log << _T("TestCase[") << i << _T("]:<") << expr << _T(">(x=") << toString(x) << _T(") failed.") << endl
                        << _T("Result(C++          ):") << toString(cppResult        ) << _T(".") << endl
                        << _T("Result(Compiled     ):") << toString(compiledResult   ) << _T(".") << endl
                        << _T("Result(Reduced      ):") << toString(reducedResult    ) << _T(".") << endl
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
          compiledExpr.compile(   str, true);
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

    TEST_METHOD(ExpressionTestAndOrReduction) {
      FPU::init();
      const TCHAR *relOp[] = {
        _T("=="), _T("!="), _T("<"), _T("<="), _T(">"), _T(">=")
      };
      const TCHAR *relOpr[] = {
        _T("=="), _T("!="), _T(">"), _T(">="), _T("<"), _T("<=")
      };
      const TCHAR *boolOp[] = { _T("&&"), _T("||") };
      StringArray aAndbb,aAndbr,aAndrb,aAndrr;
      StringArray aOrbb,aOrbr,aOrrb,aOrrr;
      for(int rop1 = 0; rop1 < ARRAYSIZE(relOp); rop1++) {
        for(int rop2 = 0; rop2 < ARRAYSIZE(relOp); rop2++) {
          for(int c1 = 1; c1 <= 3; c1++) {
            for(int c2 = 1; c2 <= 3; c2++) {
              const String op1  = format(_T("x %s %d"), relOp[rop1], c1);
              const String op2  = format(_T("x %s %d"), relOp[rop2], c2);
              const String op1r = format(_T("%d %s x"), c1, relOpr[rop1]);
              const String op2r = format(_T("%d %s x"), c2, relOpr[rop2]);
              aAndbb.add(format(_T("%s && %s"), op1.cstr() , op2.cstr()));
              aAndbr.add(format(_T("%s && %s"), op1.cstr() , op2r.cstr()));
              aAndrb.add(format(_T("%s && %s"), op1r.cstr(), op2.cstr()));
              aAndrr.add(format(_T("%s && %s"), op1r.cstr(), op2r.cstr()));
              aOrbb.add( format(_T("%s || %s"), op1.cstr() , op2.cstr()));
              aOrbr.add( format(_T("%s || %s"), op1.cstr() , op2r.cstr()));
              aOrrb.add( format(_T("%s || %s"), op1r.cstr(), op2.cstr()));
              aOrrr.add( format(_T("%s || %s"), op1r.cstr(), op2r.cstr()));
            }
          }
        }
      }
      StringArray exprArray;
      exprArray.addAll(aAndbb); aAndbb.clear();
      exprArray.addAll(aAndbr); aAndbr.clear();
      exprArray.addAll(aAndrb); aAndrb.clear();
      exprArray.addAll(aAndrr); aAndrr.clear();
      exprArray.addAll(aOrbb ); aOrbb.clear();
      exprArray.addAll(aOrbr ); aOrbr.clear();
      exprArray.addAll(aOrrb ); aOrrb.clear();
      exprArray.addAll(aOrrr ); aOrrr.clear();

      for(int rop1 = 0; rop1 < ARRAYSIZE(relOp); rop1++) {
        for(int rop2 = 0; rop2 < ARRAYSIZE(relOp); rop2++) {
          const String op1  = format(_T("2*x %s x+1")  , relOp[rop1] );
          const String op2  = format(_T("2*x %s x+1")  , relOp[rop2] );
          const String op1r = format(_T("x+1 %s 2*x")  , relOpr[rop1]);
          const String op2r = format(_T("x+1 %s 2*x")  , relOpr[rop2]);
          aAndbb.add(format(_T("%s && %s"), op1.cstr() , op2.cstr()) );
          aAndbr.add(format(_T("%s && %s"), op1.cstr() , op2r.cstr()));
          aAndrb.add(format(_T("%s && %s"), op1r.cstr(), op2.cstr()) );
          aAndrr.add(format(_T("%s && %s"), op1r.cstr(), op2r.cstr()));
          aOrbb.add( format(_T("%s || %s"), op1.cstr() , op2.cstr()) );
          aOrbr.add( format(_T("%s || %s"), op1.cstr() , op2r.cstr()));
          aOrrb.add( format(_T("%s || %s"), op1r.cstr(), op2.cstr()) );
          aOrrr.add( format(_T("%s || %s"), op1r.cstr(), op2r.cstr()));
        }
      }
      exprArray.addAll(aAndbb); aAndbb.clear();
      exprArray.addAll(aAndbr); aAndbr.clear();
      exprArray.addAll(aAndrb); aAndrb.clear();
      exprArray.addAll(aAndrr); aAndrr.clear();
      exprArray.addAll(aOrbb ); aOrbb.clear();
      exprArray.addAll(aOrbr ); aOrbr.clear();
      exprArray.addAll(aOrrb ); aOrrb.clear();
      exprArray.addAll(aOrrr ); aOrrr.clear();

      try {
        const size_t n = exprArray.size();
        for(UINT i = 0; i < n; i++) {
          const String  expr = exprArray[i];

          OUTPUT(_T("Test[%d]:%s"), i, expr.cstr());
          FILE      * listFile = openBoolListFile(       i);
          FILE      *RlistFile = openBoolReducedListFile(i);
          FILE      * dumpFile = openBoolDumpFile(       i);
          FILE      *RdumpFile = openBoolReducedDumpFile(i);
          Expression compiledExpr, interpreterExpr, reducedExpr;
          compiledExpr.compile(expr, true,false, listFile);
          reducedExpr.compile( expr, true,true ,RlistFile);
          _ftprintf( dumpFile, _T("%s\n%s\n"), expr.cstr(),compiledExpr.treeToString().cstr());
          _ftprintf(RdumpFile, _T("%s\n%s\n"), expr.cstr(),reducedExpr.treeToString().cstr());
          fclose(RdumpFile);
          fclose( dumpFile);
          fclose(RlistFile);
          fclose( listFile);

//          debugLog(_T("Test %d %s\n%s\n"), i, expr.cstr(), compiledExpr.treeToString().cstr());

          interpreterExpr.compile(expr, false,false);
          if(!compiledExpr.isOk()) {
            OUTPUT(_T("Error in testcase[%d]<%s>"), i, expr.cstr());
            const StringArray &errors = compiledExpr.getErrors();
            for(size_t i = 0; i < errors.size(); i++) {
              OUTPUT(_T("%s"), errors[i].cstr());
            }
            verify(false);
          } else {
            verify(reducedExpr.isOk());
            verify(interpreterExpr.isOk());
            verify(compiledExpr.getReturnType()    == EXPR_RETURN_BOOL);
            verify(reducedExpr.getReturnType()     == EXPR_RETURN_BOOL);
            verify(interpreterExpr.getReturnType() == EXPR_RETURN_BOOL);
            for(Real x = 0; x <= 4; x++) {
              compiledExpr.setValue(   _T("x"), x);
              reducedExpr.setValue(    _T("x"), x);
              interpreterExpr.setValue(_T("x"), x);
              const bool compiledResult    = compiledExpr.evaluateBool();
              const bool reducedResult     = reducedExpr.evaluateBool();
              const bool interpreterResult = interpreterExpr.evaluateBool();
              if((compiledResult    != interpreterResult)
              || (reducedResult     != interpreterResult)
              ) {
                LOG log;
                log << _T("TestCase[") << i << _T("]:<") << expr << _T(">(x=") << toString(x) << _T(") failed.") << endl
                    << _T("Result(Compiled     ):") << toString(compiledResult   ) << _T(".") << endl
                    << _T("Result(Reduced      ):") << toString(reducedResult    ) << _T(".") << endl
                    << _T("Result(Interpreter  ):") << toString(interpreterResult) << _T(".") << endl;
                verify(false);
              }
            } // for(x..
          } // else
        } // for(i...
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

#ifdef TEST_DERIVATIVES
    TEST_METHOD(ExpressionTestDerive) {
      ExpressionTest::startDeriveTest(OUTPUT);
      FPU::init();
//      redirectDebugLog();
      try {
        const CompactArray<ExpressionTest*> &testArray = ExpressionTest::getAllSamples();
        const size_t                         n         = testArray.size();
        for(size_t i = 0; i < n; i++) {
          ExpressionTest &test = *testArray[i];
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
          Expression compiledDFDX    = compiledExpr.getDerived(   _T("x"),true);
          Expression interpreterDFDX = interpreterExpr.getDerived(_T("x"),true);
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

  };
}
