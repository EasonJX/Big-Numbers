/* DO NOT EDIT THIS FILE - it is machine generated */
#line 47 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
#include "pch.h"
#include <Math/Expression/ExpressionParser.h>
#include <Math/MathFunctions.h>

#pragma warning(disable:4312)


#line 21 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"

int ExpressionParser::reduceAction(unsigned int prod) {

  switch(prod) {
  case 0: /* start -> function */
#line 56 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_tree.setRoot(getStackTop(0));                                       }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 2: /* function -> assignStmtList final_expr */
#line 60 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), SEMI   , getStackTop(1), getStackTop(0), NULL );         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 3: /* final_expr -> expr */
#line 63 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), RETURNREAL, getStackTop(0), NULL );          }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 4: /* final_expr -> boolExpr */
#line 64 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), RETURNBOOL, getStackTop(0), NULL );          }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 6: /* assignStmtList -> assignStmtList assignStmt */
#line 68 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), SEMI   , getStackTop(1), getStackTop(0), NULL );         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 7: /* assignStmt -> assign SEMI */
#line 71 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = getStackTop(1); }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 8: /* assign -> name ASSIGN expr */
#line 74 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), ASSIGN , getStackTop(2), getStackTop(0), NULL );         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 9: /* expr -> expr PLUS expr */
#line 77 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), PLUS   , getStackTop(2), getStackTop(0), NULL);          }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 10: /* expr -> expr MINUS expr */
#line 78 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), MINUS  , getStackTop(2), getStackTop(0), NULL);          }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 11: /* expr -> expr PROD expr */
#line 79 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), PROD   , getStackTop(2), getStackTop(0), NULL);          }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 12: /* expr -> expr QUOT expr */
#line 80 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), QUOT   , getStackTop(2), getStackTop(0), NULL);          }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 13: /* expr -> expr MOD expr */
#line 81 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), MOD    , getStackTop(2), getStackTop(0), NULL);          }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 14: /* expr -> expr POW expr */
#line 82 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), POW    , getStackTop(2), getStackTop(0), NULL);          }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 18: /* unary -> function1 LPAR expr RPAR */
#line 88 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getStackTop(3)->getPos(),getStackTop(3)->getSymbol(), getStackTop(1), NULL);    }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 19: /* unary -> function2 LPAR expr COMMA expr RPAR */
#line 89 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getStackTop(5)->getPos(),getStackTop(5)->getSymbol(), getStackTop(3), getStackTop(1), NULL);}
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 20: /* unary -> LPAR expr RPAR */
#line 90 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = getStackTop(1);                                                  }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 21: /* unary -> MINUS expr */
#line 91 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), MINUS, getStackTop(0), NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 22: /* unary -> PLUS expr */
#line 92 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = getStackTop(0);                                                  }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 27: /* unary -> LB boolExpr RB */
#line 97 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), IIF
                                                                    , getStackTop(1), newNode(getPos(1), NUMBER  , ator("1"))
                                                                    , newNode(getPos(1), NUMBER  , ator("0"))
                                                                    , NULL);                                      }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 28: /* conditionalExpr -> IIF LPAR boolExpr COMMA expr COMMA expr RPAR */
#line 105 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), IIF, getStackTop(5), getStackTop(3), getStackTop(1), NULL);          }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 29: /* sumExpr -> INDEXEDSUM LPAR assign TO expr RPAR expr */
#line 109 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), INDEXEDSUM, getStackTop(4), getStackTop(2), getStackTop(0), NULL);   }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 30: /* productExpr -> INDEXEDPRODUCT LPAR assign TO expr RPAR expr */
#line 113 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), INDEXEDPRODUCT, getStackTop(4), getStackTop(2), getStackTop(0),NULL);}
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 31: /* polyExpr -> POLY LB exprList RB LPAR expr RPAR */
#line 116 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), POLY, getStackTop(4), getStackTop(1), NULL);             }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 32: /* exprList -> exprList COMMA expr */
#line 119 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), COMMA, getStackTop(2), getStackTop(0), NULL);            }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 34: /* function1 -> ABS */
#line 123 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ABS     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 35: /* function1 -> ACOS */
#line 124 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ACOS    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 36: /* function1 -> ACOSH */
#line 125 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ACOSH   , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 37: /* function1 -> ACOT */
#line 126 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ACOT    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 38: /* function1 -> ACSC */
#line 127 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ACSC    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 39: /* function1 -> ASEC */
#line 128 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ASEC    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 40: /* function1 -> ASIN */
#line 129 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ASIN    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 41: /* function1 -> ASINH */
#line 130 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ASINH   , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 42: /* function1 -> ATAN */
#line 131 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ATAN    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 43: /* function1 -> ATANH */
#line 132 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ATANH   , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 44: /* function1 -> CEIL */
#line 133 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), CEIL    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 45: /* function1 -> COS */
#line 134 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), COS     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 46: /* function1 -> COSH */
#line 135 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), COSH    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 47: /* function1 -> COT */
#line 136 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), COT     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 48: /* function1 -> CSC */
#line 137 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), CSC     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 49: /* function1 -> ERF */
#line 138 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ERF     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 50: /* function1 -> EXP */
#line 139 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), EXP     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 51: /* function1 -> FAC */
#line 140 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), FAC     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 52: /* function1 -> FLOOR */
#line 141 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), FLOOR   , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 53: /* function1 -> GAMMA */
#line 142 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), GAMMA   , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 54: /* function1 -> GAUSS */
#line 143 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), GAUSS   , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 55: /* function1 -> INVERF */
#line 144 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), INVERF  , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 56: /* function1 -> LN */
#line 145 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), LN      , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 57: /* function1 -> LOG10 */
#line 146 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), LOG10   , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 58: /* function1 -> NORM */
#line 147 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), NORM    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 59: /* function1 -> PROBIT */
#line 148 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), PROBIT  , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 60: /* function1 -> SEC */
#line 149 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), SEC     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 61: /* function1 -> SIGN */
#line 150 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), SIGN    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 62: /* function1 -> SIN */
#line 151 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), SIN     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 63: /* function1 -> SINH */
#line 152 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), SINH    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 64: /* function1 -> SQR */
#line 153 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), SQR     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 65: /* function1 -> SQRT */
#line 154 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), SQRT    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 66: /* function1 -> TAN */
#line 155 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), TAN     , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 67: /* function1 -> TANH */
#line 156 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), TANH    , NULL);                 }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 68: /* boolExpr -> expr EQ expr */
#line 159 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), EQ      , getStackTop(2), getStackTop(0), NULL);         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 69: /* boolExpr -> expr NE expr */
#line 160 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), NE      , getStackTop(2), getStackTop(0), NULL);         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 70: /* boolExpr -> expr LE expr */
#line 161 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), LE      , getStackTop(2), getStackTop(0), NULL);         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 71: /* boolExpr -> expr LT expr */
#line 162 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), LT      , getStackTop(2), getStackTop(0), NULL);         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 72: /* boolExpr -> expr GE expr */
#line 163 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), GE      , getStackTop(2), getStackTop(0), NULL);         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 73: /* boolExpr -> expr GT expr */
#line 164 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), GT      , getStackTop(2), getStackTop(0), NULL);         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 74: /* boolExpr -> boolExpr AND boolExpr */
#line 165 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), AND     , getStackTop(2), getStackTop(0), NULL);         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 75: /* boolExpr -> boolExpr OR boolExpr */
#line 166 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), OR      , getStackTop(2), getStackTop(0), NULL);         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 76: /* boolExpr -> NOT boolExpr */
#line 167 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(2), NOT     , getStackTop(0), NULL);         }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 77: /* boolExpr -> LPAR boolExpr RPAR */
#line 168 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = getStackTop(1);                                                  }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 78: /* function2 -> MAX */
#line 171 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), MAX      , NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 79: /* function2 -> MIN */
#line 172 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), MIN      , NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 80: /* function2 -> NORMRAND */
#line 173 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), NORMRAND , NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 81: /* function2 -> RAND */
#line 174 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), RAND     , NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 82: /* function2 -> ROOT */
#line 175 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ROOT     , NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 83: /* function2 -> BINOMIAL */
#line 176 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), BINOMIAL , NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 84: /* function2 -> CHI2DENS */
#line 177 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), CHI2DENS , NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 85: /* function2 -> CHI2DIST */
#line 178 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), CHI2DIST , NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 86: /* function2 -> LINCGAMMA */
#line 179 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), LINCGAMMA, NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 87: /* function2 -> ATAN2 */
#line 180 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), ATAN2    , NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 88: /* function2 -> HYPOT */
#line 181 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), HYPOT    , NULL);                }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 89: /* name -> NAME */
#line 184 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), NAME    , getText());            }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  case 90: /* number -> NUMBER */
#line 187 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
    { m_dollardollar = newNode( getPos(1), NUMBER  , ttor(getText()));      }
#line 26 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
    break;
  }
  return 0;
}

#line 192 "C:\\mytools2015\\Lib\\Src\\Math\\Expression\\Expression.y"
ExpressionNode *ExpressionParser::newNode(const SourcePosition &pos, ExpressionInputSymbol symbol, ...) {
  va_list argptr;
  va_start(argptr, symbol);
  ExpressionNode *p = m_tree.vFetchNode(pos, symbol, argptr);
  va_end(argptr);
  return p;
}

void ExpressionParser::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  m_tree.vAddError(&pos, format, argptr);
}

#line 31 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
