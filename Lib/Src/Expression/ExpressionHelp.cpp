#include "pch.h"
#include <string.h>
#include <Math/Expression/Expression.h>

namespace Expr {

ExpressionDescription::ExpressionDescription(const TCHAR *syntax, const TCHAR *description) {
  m_syntax      = syntax;
  m_description = description;
}

#define DESCRIPTION(syntax, description) ExpressionDescription(_T(syntax), _T(description))

const ExpressionDescription ExpressionDescription::expressionHelpList[] = {
    DESCRIPTION("x + y"                                , "x plus y")
   ,DESCRIPTION("x - y"                                , "x minus y")
   ,DESCRIPTION("x * y"                                , "x multiplied by y")
   ,DESCRIPTION("x / y"                                , "x divided by y")
   ,DESCRIPTION("x % y"                                , "x modulus y")
   ,DESCRIPTION("x ^ y"                                , "x raised to the power of y. Special case:x^0 = 1 for all x >= 0")
   ,DESCRIPTION("x ** y"                               , "Same as \"x ^ y\"")
   ,DESCRIPTION("abs(x)"                               , "Absolute value of x. abs(x) = x for x >= 0, -x for x < 0")
   ,DESCRIPTION("acos(x)"                              , "The inverse cosine function over the domain [-1;1]")
   ,DESCRIPTION("acosh(x)"                             , "The inverse hyperbolic cosine function over the domain x >= 1")
   ,DESCRIPTION("acot(x)"                              , "The inverse cotangent function")
   ,DESCRIPTION("acsc(x)"                              , "The inverse cosecant function over the domain x <= -1 or x >= 1")
   ,DESCRIPTION("asec(x)"                              , "The inverse secant function over the domain x <= -1 or x >= 1")
   ,DESCRIPTION("asin(x)"                              , "The inverse sine function over the domain [-1;1]")
   ,DESCRIPTION("asinh(x)"                             , "The inverse hyperbolic sine function")
   ,DESCRIPTION("a = x*x;"                             , "Create the variable a and assign the value x*x to it")
   ,DESCRIPTION("atan(x)"                              , "The inverse tangent function")
   ,DESCRIPTION("atan2(y,x)"                           , "The inverse tangent function of \"y/x\", returning the angle for vector(x,y) with x-axes")
   ,DESCRIPTION("atanh(x)"                             , "The inverse hyperbolic tangent function over the domain [-1;1]")
   ,DESCRIPTION("binomial(x,y)"                        , "The binomial coefficient of x and y")
   ,DESCRIPTION("ceil(x)"                              , "The smallest integer greater than or equal to x. ceil(2.4) = 3. ceil(-2.1) = -2")
   ,DESCRIPTION("chi2dens(k,x)"                        , "The chi-square density. Domain: x >= 0")
   ,DESCRIPTION("chi2dist(k,x)"                        , "The chi-square distribution. Domain: x >= 0")
   ,DESCRIPTION("cos(x)"                               , "The cosine function")
   ,DESCRIPTION("cosh(x)"                              , "The hyperbolic cosine function")
   ,DESCRIPTION("cot(x)"                               , "The cotangent function. cot(x) = 1/tan(x)")
   ,DESCRIPTION("csc(x)"                               , "The cosecant function. csc(x) = 1/sin(x)")
   ,DESCRIPTION("e"                                    , "Base of the natural logarithm. e = 2.71828182...")
   ,DESCRIPTION("erf(x)"                               , "The error function. erf(x) = 2*norm(sqrt(2)*x)-1")
   ,DESCRIPTION("exp(x)"                               , "The exponential function with base e")
   ,DESCRIPTION("exp10(x)"                             , "The exponential function with base 10")
   ,DESCRIPTION("exp2(x)"                              , "The exponential function with base 2")
   ,DESCRIPTION("fac(x)"                               , "The factorial function. fac(x) = x! = x*(x-1)*(x-2)*...*1. fac(x) = gamma(x+1)")
   ,DESCRIPTION("floor(x)"                             , "The highest integer less than or equal to x. floor(2.4) = 2. floor(-2.1) = -3")
   ,DESCRIPTION("gamma(x)"                             , "The gamma function. gamma(1) = 1. gamma(x) = x * gamma(x-1)")
   ,DESCRIPTION("gauss(x)"                             , "The standard normal density. Gauss-density function. gauss(x) = norm'(x)")
   ,DESCRIPTION("if(condition,expr1,expr2)"            , "Conditional expression. If condition is true, the value is expr1, else the value is expr2")
   ,DESCRIPTION("[condition]"                          , "Iverson bracket. If condition is true then 1, else 0. [condition] is equivalent to if(condition,1,0)")
   ,DESCRIPTION("inverf(x)"                            , "The inverse error function over the domain ]-1;1[. inverf(erf(x)) == x")
   ,DESCRIPTION("lincgamma(k,x)"                       , "The lowr incomplete gamma function. Domain: x >= 0")
   ,DESCRIPTION("ln(x)"                                , "The natural logarithm with base e. ln'(x) = 1/x")
   ,DESCRIPTION("log10(x)"                             , "The logarithm with base 10. log10(x) = ln(x) / ln(10)")
   ,DESCRIPTION("log2(x)"                              , "The logarithm with base 2. log2(x) = ln(x) / ln(2)")
   ,DESCRIPTION("max(x,y)"                             , "The highest value of x and y")
   ,DESCRIPTION("min(x,y)"                             , "The lowest value of x and y")
   ,DESCRIPTION("norm(x)"                              , "The standard normal distribution function. norm'(x) = gauss(x)")
   ,DESCRIPTION("normrand(m,s)"                        , "Normal distributed random value with mean value m, and standard deviation s")
   ,DESCRIPTION("pi"                                   , "pi = 3.14159265359...")
   ,DESCRIPTION("poly[a,b,c,...](x)"                   , "The polynomial with coefficients a,b,c,.... poly[a,b,c](x) = ax^2+bx+c")
   ,DESCRIPTION("probit(x)"                            , "The inverse normal distribution function over the domain ]0;1[. probit(norm(x)) = x")
   ,DESCRIPTION("product(i = start to end) expression" , "Ex: 10! = product(i=1 to 10) i")
   ,DESCRIPTION("rand(a,b)"                            , "Uniform distributed random value in the range [a;b[")
   ,DESCRIPTION("root(x,y)"                            , "The y'th root of x. root(x,y) = x^(1/y). root(x,2) = square root of x")
   ,DESCRIPTION("sign(x)"                              , "sign(x) = 1 for x > 0, 0 for x = 0, -1 for x < 0")
   ,DESCRIPTION("sec(x)"                               , "The secant function. sec(x) = 1/cos(x)")
   ,DESCRIPTION("sin(x)"                               , "The sine function")
   ,DESCRIPTION("sinh(x)"                              , "The hyperbolic sine function")
   ,DESCRIPTION("sqr(x)"                               , "x squared. sqr(x) = x*x")
   ,DESCRIPTION("sqrt(x)"                              , "The square root of x over the domain x >= 0")
   ,DESCRIPTION("sum(i = start to end) expression"     , "Ex: sum(i=0 to n) x^i/fac(i) -> exp(x) for n -> infinity")
   ,DESCRIPTION("tan(x)"                               , "The tangent function. tan(x) = sin(x)/cos(x)")
   ,DESCRIPTION("tanh(x)"                              , "The hyperbolic tangent function")
   ,DESCRIPTION("not condition"                        , "Logical negation. True if condition is false, false if condition is true")
   ,DESCRIPTION("!condition"                           , "Same as \"not condition\"")
   ,DESCRIPTION("condition1 and condition2"            , "Logical and. True iff condition1 is true and condition2 is true")
   ,DESCRIPTION("condition1 &&&& condition2"           , "Same as \"condition1 and condition2\"")
   ,DESCRIPTION("condition1 or condition2"             , "Logical or. True iff condition1 is true or condition2 is true or both are true")
   ,DESCRIPTION("condition1 || condition2"             , "Same as \"condition1 or condition2\"")
   ,DESCRIPTION("expression1 == expression2"           , "True iff expression1 equals expression2")
   ,DESCRIPTION("expression1 <> expression2"           , "True iff expression1 not equals expression2")
   ,DESCRIPTION("expression1 >  expression2"           , "True iff expression1 is greater than expression2")
   ,DESCRIPTION("expression1 <  expression2"           , "True iff expression1 is less than expression2")
   ,DESCRIPTION("expression1 >= expression2"           , "True iff expression1 is greater than or equals expression2")
   ,DESCRIPTION("expression1 <= expression2"           , "True iff expression1 is less than or equals expression2")
};

const int ExpressionDescription::helpListSize = ARRAYSIZE(expressionHelpList);

}; // namespace Expr