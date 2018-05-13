#pragma once

#include <MyUtil.h>
#include <CompactHashMap.h>
#include <Scanner.h>
#include <Math/Number.h>
#include "PragmaLib.h"
#include "ExpressionSymbol.h"

namespace Expr {

class ExpressionVariable {
private:
  const String m_name;
  // Index into parserTree.m_valueTable
  int          m_valueIndex;
  UINT         m_occurrenceCount;
  UINT         m_constant : 1;
  UINT         m_defined  : 1;
  UINT         m_loopVar  : 1;
  UINT         m_marked   : 1;

public:
  ExpressionVariable(const String &name, bool isConstant, bool isDefined, bool isLoopVar);
  inline const String &getName()       const    { return m_name;                    }
  inline       int     getValueIndex() const    { return m_valueIndex;              }
  inline void          setValueIndex(int index) { m_valueIndex = index;             }
  inline void          setDefined()             { m_defined = 1;                    }
  inline void          incrOccurrence()         { m_occurrenceCount++;              }
  inline bool          isConstant()    const    { return m_constant ? true : false; }
  inline bool          isDefined()     const    { return m_defined  ? true : false; }
  inline bool          isLoopVar()     const    { return m_loopVar  ? true : false; }
  inline bool          isInput()       const    { return (m_constant | m_defined | m_loopVar) == 0; }
  inline bool          isMarked()      const    { return m_marked   ? true : false; }
  inline void          unMark()                 { m_marked = 0; }
  inline void          mark()                   { m_marked = 1; }
  String toString() const;
};

class ExpressionVariableWithValue : public ExpressionVariable {
private:
  const Real m_value;
public:
  inline ExpressionVariableWithValue(const ExpressionVariable &var, Real value)
    : ExpressionVariable(var)
    , m_value(value)
  {
  }
  inline ExpressionVariableWithValue(const String &name, bool isConstant, bool isDefined, bool isLoopVar, Real value)
    : ExpressionVariable(name, isConstant, isDefined, isLoopVar)
    , m_value(value)
  {
  }

  inline Real getValue() const {
    return m_value;
  }
};

typedef Array<ExpressionVariableWithValue> ExpressionVariableArray;

class ExpressionNode;
class SumElement;
class ExpressionFactor;
class ExpressionNodeSelector;
class ParserTree;
class Expression;

class ExpressionNodeArray : public CompactArray<ExpressionNode*> {
public:
  ExpressionNodeArray() {
  }
  explicit ExpressionNodeArray(size_t capacity) : CompactArray<ExpressionNode*>(capacity) {
  }
  ExpressionNode *toTree(ExpressionInputSymbol delimiter) const;
  bool isConstant() const;
  BitSet getNonConstantNodes() const;
  String toString() const;
};

class AddentArray : public CompactArray<SumElement*> {
public:
  AddentArray() {
  }
  explicit AddentArray(size_t capacity) : CompactArray<SumElement*>(capacity) {
  }
  void add(ExpressionNode *n, bool positive);
  inline void add(SumElement *e) {
    __super::add(e);
  }
  inline AddentArray &operator+=(ExpressionNode *n) {
    add(n, true);
    return *this;
  }
  inline AddentArray &operator-=(ExpressionNode *n) {
    add(n, false);
    return *this;
  }
  String toString() const;
};

class FactorArray : public CompactArray<ExpressionFactor*> {
public:
  FactorArray() {
  }
  explicit FactorArray(size_t capacity) : CompactArray<ExpressionFactor*>(capacity) {
  }

  FactorArray selectConstantPositiveExponentFactors() const;
  FactorArray selectConstantNegativeExponentFactors() const;
  FactorArray selectNonConstantExponentFactors() const;
  int findFactorWithChangeableSign() const;

  void add(ExpressionFactor *f);
  void add(ExpressionNode *base, ExpressionNode *exponent = NULL);
  String toString() const;
};

class ExpressionSymbolSet : public BitSet {
public:
  ExpressionSymbolSet();
};

class ExpressionNodeHandler {
public:
  virtual bool handleNode(ExpressionNode *n, int level) = 0;
};

typedef CompactKeyType<ExpressionInputSymbol> ExpressionSymbolKey;

template<class E> class CompactSymbolHashMap : public CompactHashMap<ExpressionSymbolKey, E> {
};

class SymbolOrderMap : private CompactSymbolHashMap<int> {
private:
  bool m_initDone;
  void init();
public:
  SymbolOrderMap() {
    m_initDone = false;
  };
  int compare(ExpressionInputSymbol s1, ExpressionInputSymbol s2);
};

class InverseFunctionMap : private CompactSymbolHashMap<ExpressionInputSymbol> {
private:
  bool m_initDone;
  void init();
public:
  InverseFunctionMap() {
    m_initDone = false;
  };
  ExpressionInputSymbol getInverse(ExpressionInputSymbol symbol);
};

typedef enum {
  EXPRESSIONNODENUMBER
 ,EXPRESSIONNODEBOOLEAN
 ,EXPRESSIONNODEVARIABLE
 ,EXPRESSIONNODEPOLYNOMIAL
 ,EXPRESSIONNODETREE
 ,EXPRESSIONNODESUM
 ,EXPRESSIONNODEPRODUCT
 ,EXPRESSIONNODEFACTOR
} ExpressionNodeType;

typedef enum {
  EXPR_NORETURNTYPE
 ,EXPR_RETURN_REAL
 ,EXPR_RETURN_BOOL
} ExpressionReturnType;

class PackedSyntaxNodeInfo {
public:
  const ExpressionInputSymbol m_symbol         : 15;
  // used for garbage-collection
          UINT          m_marked               : 1;
  // used for DebugThread
          UINT          m_breakPoint           : 1;
  // used by polynomials
  mutable UINT          m_coefficientsConstant : 1;
  // used by polynomials
  mutable UINT          m_coefChecked          : 1;
  PackedSyntaxNodeInfo(ExpressionInputSymbol symbol)
    : m_symbol(symbol)
    , m_marked(0)
    , m_breakPoint(0)
    , m_coefficientsConstant(0)
    , m_coefChecked(0)
  {
  }
};

class ExpressionNode {
private:
  static SymbolOrderMap      s_orderMap;
  static InverseFunctionMap  s_inverseFunctionMap;
         ParserTree         &m_tree;
  PackedSyntaxNodeInfo       m_info;

  Real evaluatePow()               const;
  Real evaluateRoot()              const;
  ExpressionNode &operator=(const ExpressionNode &src); // not implemented
  ExpressionNode(           const ExpressionNode &src); // not implemented
protected:
  static String &addLeftMargin(String &s, int level);

  Exception createAttributeNotSupportedException(const char *attribute) const;
#define UNSUPPORTEDOP() throw createAttributeNotSupportedException(__FUNCTION__)

public:
  ExpressionNode(ParserTree *tree, ExpressionInputSymbol symbol);
  virtual ~ExpressionNode() {}

  inline ExpressionInputSymbol       getSymbol()                    const   { return (ExpressionInputSymbol)m_info.m_symbol;                   }
  inline  const ParserTree          *getTree()                      const   { return &m_tree;                                                  }
  inline        ParserTree          *getTree()                              { return &m_tree;                                                  }
          const Expression          *getExpr()                      const;
                Expression          *getExpr();
  inline String                      getSymbolName()                const   { return getSymbolName(getSymbol());  }
  static String                      getSymbolName(ExpressionInputSymbol symbol);
  inline  void                       mark()                                 { m_info.m_marked = true;                                          }
  inline  void                       unMark()                               { m_info.m_marked = false;                                         }
  inline  bool                       isMarked()                     const   { return m_info.m_marked;                                          }
  inline  void                       setBreakPoint()                        { m_info.m_breakPoint = 1;                                         }
  inline  void                       clearBreakPoint()                      { m_info.m_breakPoint = 0;                                         }
  inline  bool                       isBreakPoint()                 const   { return m_info.m_breakPoint;                                      }
  virtual bool                       hasPos()                       const   { return false;    }
  virtual const SourcePosition      &getPos()                       const   { UNSUPPORTEDOP(); }
  virtual       ExpressionNode      *left()                                 { return child(0); }
  virtual const ExpressionNode      *left()                         const   { return child(0); }
  virtual       ExpressionNode      *right()                                { return child(1); }
  virtual const ExpressionNode      *right()                        const   { return child(1); }
  virtual       ExpressionNode      *getArgument()                          { UNSUPPORTEDOP(); }
  virtual const ExpressionNode      *getArgument()                  const   { UNSUPPORTEDOP(); }
  virtual       ExpressionNode      *child(UINT i)                          { UNSUPPORTEDOP(); }
  virtual const ExpressionNode      *child(UINT i)                  const   { UNSUPPORTEDOP(); }
  virtual int                        getChildCount()                const   { UNSUPPORTEDOP(); }
  virtual       ExpressionNodeArray &getChildArray()                        { UNSUPPORTEDOP(); }
  virtual const ExpressionNodeArray &getChildArray()                const   { UNSUPPORTEDOP(); }
  virtual       FactorArray         &getFactorArray()                       { UNSUPPORTEDOP(); }
  virtual const FactorArray         &getFactorArray()               const   { UNSUPPORTEDOP(); }
  virtual       AddentArray         &getAddentArray()                       { UNSUPPORTEDOP(); }
  virtual const AddentArray         &getAddentArray()               const   { UNSUPPORTEDOP(); }
  virtual       ExpressionNodeArray &getCoefficientArray()                  { UNSUPPORTEDOP(); }
  virtual const ExpressionNodeArray &getCoefficientArray()          const   { UNSUPPORTEDOP(); }
  virtual int                        getFirstCoefIndex()            const   { UNSUPPORTEDOP(); }
  virtual void                       setFirstCoefIndex(int index)           { UNSUPPORTEDOP(); }
  virtual int                        getDegree()                    const   { UNSUPPORTEDOP(); }
  virtual const String              &getName()                      const   { UNSUPPORTEDOP(); }
  virtual void                       setVariable(ExpressionVariable *var)   { UNSUPPORTEDOP(); }
  virtual       ExpressionVariable  &getVariable()                  const   { UNSUPPORTEDOP(); }
  virtual const Number              &getNumber()                    const   { UNSUPPORTEDOP(); }
  virtual       int                  getValueIndex()                const   { UNSUPPORTEDOP(); }
  virtual       void                 setValueIndex(int index)               { UNSUPPORTEDOP(); }
  virtual       bool                 getBool()                      const   { UNSUPPORTEDOP(); }
  virtual ExpressionNode            *expand()                               { UNSUPPORTEDOP(); }
  virtual bool                       isExpandable()                         { return false;    }

  virtual int                        compare(ExpressionNode *n);

  virtual ExpressionNode            *clone(ParserTree *tree)        const = 0;

  virtual bool                       isConstant()                   const = 0;
  virtual bool                       isBooleanOperator()            const { return false; }
  virtual Real                       evaluateReal()                 const;
  virtual bool                       evaluateBool()                 const { UNSUPPORTEDOP(); }

  virtual ExpressionNodeType         getNodeType()                  const = 0;
  virtual ExpressionReturnType       getReturnType()                const   { return EXPR_RETURN_REAL; }
  virtual bool                       hasVariable()                  const   { return false;    }
  virtual bool                       traverseExpression(ExpressionNodeHandler &handler, int level) = 0;

  virtual void                       dumpNode(String &s, int level) const = 0;
  virtual String                     toString()                     const = 0;

  inline  bool                       isName()                       const   { return getSymbol() == NAME;                                      }
  inline  bool                       isNumber()                     const   { return getSymbol() == NUMBER;                                    }
  inline  bool                       isBoolean()                    const   { return getSymbol() == TYPEBOOL;                                  }
  inline  bool                       isRational()                   const   { return isNumber() && getNumber().isRational();                   }
  inline  bool                       isInteger()                    const   { return isNumber() && getNumber().isInteger();                    }
  inline  bool                       isNameOrNumber()               const   { return isName() || isNumber();                                   }
  inline  bool                       isEven()                       const   { return isRational() && getRational().isEven();                   }
  inline  bool                       isOdd()                        const   { return isRational() && getRational().isOdd();                    }
  inline  bool                       isMultiplyable()               const   { return !isNameOrNumber();                                        }
  inline  Real                       getReal()                      const   { return getNumber().getRealValue();                               }
  inline  Rational                   getRational()                  const   { return getNumber().getRationalValue();                           }
  inline  bool                       isUnaryMinus()                 const   { return (getSymbol() == MINUS) && (getChildCount() == 1);         }
  inline  bool                       isBinaryMinus()                const   { return (getSymbol() == MINUS) && (getChildCount() == 2);         }
  inline  bool                       isEulersConstant()             const   { return isName()     && getName()  == _T("e");                    }
  inline  bool                       isPi()                         const   { return isName()     && getName()  == _T("pi");                   }
  inline  bool                       isZero()                       const   { return isNumber()   && getReal()  == 0;                          }
  inline  bool                       isOne()                        const   { return isNumber()   && getReal()  == 1;                          }
  inline  bool                       isTwo()                        const   { return isNumber()   && getReal()  == 2;                          }
  inline  bool                       isTen()                        const   { return isNumber()   && getReal()  == 10;                         }
  inline  bool                       isMinusOne()                   const   { return isNumber()   && getReal()  == -1;                         }
  inline  bool                       isMinusTwo()                   const   { return isNumber()   && getReal()  == -2;                         }
  inline  bool                       isNegative()                   const   { return isNumber()   && getReal()  < 0;                           }
  inline  bool                       isPositive()                   const   { return isNumber()   && getReal()  > 0;                           }
  inline  bool                       isTrue()                       const   { return isBoolean()  && getBool();                                }
  inline  bool                       isFalse()                      const   { return isBoolean()  && !getBool();                               }
  inline  ExpressionInputSymbol      getInverseFunction() const {
    return s_inverseFunctionMap.getInverse(getSymbol());
  }

  bool    isCoefficientArrayConstant()  const;
  bool    dependsOn(const String &name) const;
  // If selector specified, only nodes where selector.select(n) is true will be counted.
  // If not specified, all nodes is counted
  int     getNodeCount(ExpressionNodeSelector *selector = NULL) const;
  bool    containsFunctionCall()        const;
  Real   &doAssignment()                const; // return reference to lvalue
  Real   &getValueRef()                 const;
  bool    isBinaryOperator()            const;
  bool    isTrigonomtricFunction()      const;
  bool    isSymmetricFunction()         const;
  bool    isAsymmetricFunction()        const;
  bool    isSymmetricExponent()         const;
  bool    isAsymmetricExponent()        const;
  TrigonometricMode getTrigonometricMode() const;
  int     getPrecedence()               const;
  bool    reducesToRationalConstant(Rational *r) const;
  bool    reducesToRational(        Rational *r) const;

  String  parenthesizedExpressionToString(const ExpressionNode  *parent)  const;
  bool    needParentheses(                const ExpressionNode  *parent)  const;
  int     getMaxTreeDepth()                                               const;
  void    getListFromTree(int delimiterSymbol, ExpressionNodeArray &list);

  void throwInvalidSymbolForTreeMode(const TCHAR *method) const;
  void throwUnknownSymbolException(  const TCHAR *method) const;
};

typedef CompactKeyType<const ExpressionNode*>  ExpressionNodeKey;

template<class E> class CompactNodeHashMap : public CompactHashMap<ExpressionNodeKey, E> {
};

ExpressionNodeArray getExpressionList(ExpressionNode *n);

class ExpressionNodeNumber : public ExpressionNode {
private:
  Number m_number;
  int    m_valueIndex;
public:
  inline ExpressionNodeNumber(ParserTree *tree, const Real &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
  }

  inline ExpressionNodeNumber(ParserTree *tree, const Rational &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
  }

  inline ExpressionNodeNumber(ParserTree *tree, const Number &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
  }

  int getValueIndex() const {
    return m_valueIndex;
  }

  void setValueIndex(int index) {
    m_valueIndex = index;
  }

  const Number &getNumber() const {
    return m_number;
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const {
    return true;
  }
  Real evaluateReal() const {
    return m_number.getRealValue();
  }

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODENUMBER;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const {
    return m_number.toString();
  }
};

class ExpressionNodeBoolConst : public ExpressionNode {
private:
  const bool m_value;
public:
  inline ExpressionNodeBoolConst(ParserTree *tree, bool b) : ExpressionNode(tree, TYPEBOOL), m_value(b) {
  }
  bool getBool() const {
    return m_value;
  }
  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const {
    return true;
  }
  ExpressionReturnType getReturnType() const   {
    return EXPR_RETURN_BOOL;
  }
  Real evaluateReal() const { UNSUPPORTEDOP(); }
  bool evaluateBool() const { return m_value;  }

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEBOOLEAN;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const {
    return boolToStr(m_value);
  }
};

class ExpressionNodeVariable : public ExpressionNode {
private:
  String              m_name;
  ExpressionVariable *m_var;
public:
  ExpressionNodeVariable(ParserTree *tree, const String &name);
  inline ExpressionNodeVariable(ParserTree *tree, const String &name, ExpressionVariable &var)
    : ExpressionNode(tree, NAME)
    , m_name(name)
  {
    m_var  = &var;
  }

  const String &getName() const {
    return m_name;
  }

  void setVariable(ExpressionVariable *var) {
    m_var = var;
  }

  ExpressionVariable &getVariable() const {
    return *m_var;
  }

  bool hasVariable() const {
    return m_var != NULL;
  }

  int getValueIndex() const {
    return m_var->getValueIndex();
  }

  void setValueIndex(int index) {
    m_var->setValueIndex(index);
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const {
    return m_var->isConstant();
  }
  Real evaluateReal() const {
    return getValueRef();
  }

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEVARIABLE;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const {
    return getName();
  }
};

class ExpressionNodeTree : public ExpressionNode {
private:
  ExpressionNodeArray m_childArray;
  void initChildArray(va_list argptr);
protected:
  // Terminate arguemnt list with NULL
  ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, ...);

public:
  ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, va_list argptr);
  ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, const ExpressionNodeArray &childArray);
  ExpressionNodeTree(ParserTree *tree, const ExpressionNodeTree *src);

  ExpressionNode *child(UINT i) {
    return m_childArray[i];
  }
  const ExpressionNode *child(UINT i) const {
    return m_childArray[i];
  }

  int getChildCount() const {
    return (int)m_childArray.size();
  }

  ExpressionNodeArray &getChildArray() {
    return m_childArray;
  }
  const ExpressionNodeArray &getChildArray() const {
    return m_childArray;
  }

  ExpressionNode            *expand();
  bool                       isExpandable();

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODETREE;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodeBoolExpr : public ExpressionNodeTree {
public:
  inline ExpressionNodeBoolExpr(ParserTree *tree, ExpressionInputSymbol symbol, ExpressionNode *bexpr)
    : ExpressionNodeTree(tree,symbol, bexpr,NULL)
  {
  }
  inline ExpressionNodeBoolExpr(ParserTree *tree, ExpressionInputSymbol symbol, ExpressionNode *left, ExpressionNode *right)
    : ExpressionNodeTree(tree,symbol, left, right, NULL)
  {
  }
  inline ExpressionNodeBoolExpr(ParserTree *tree, const ExpressionNodeBoolExpr *src)
    : ExpressionNodeTree(tree,src)
  {
  }
  inline ExpressionNodeBoolExpr(ParserTree *tree, ExpressionInputSymbol symbol, va_list argptr)
    : ExpressionNodeTree(tree, symbol, argptr)
  {
  }
  ExpressionNode *clone(ParserTree *tree) const {
    ExpressionNode *n = new ExpressionNodeBoolExpr(tree, this); TRACE_NEW(n);
    return n;
  }

  bool isConstant() const;
  ExpressionReturnType getReturnType() const {
    return EXPR_RETURN_BOOL;
  }
  Real evaluateReal() const { UNSUPPORTEDOP(); }
  bool evaluateBool() const;

  bool isBooleanOperator() const {
    return true;
  }

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODETREE; // HERTIL.....HVAD HER??
  }
};

class ExpressionNodePoly : public ExpressionNode {
private:
  ExpressionNodeArray  m_coefArray;
  ExpressionNode      *m_arg;
  int                  m_firstCoefIndex;

public:
  ExpressionNodePoly(ParserTree *tree, va_list argptr);
  ExpressionNodePoly(ParserTree *tree, const ExpressionNodeArray &coefArray, ExpressionNode *arg);
  ExpressionNodePoly(ParserTree *tree, const ExpressionNodePoly *src);

  int getDegree() const {
    return (int)m_coefArray.size() - 1;
  }

  ExpressionNodeArray &getCoefficientArray() {
    return m_coefArray;
  }
  const ExpressionNodeArray &getCoefficientArray() const {
    return m_coefArray;
  }

  ExpressionNode *getArgument() {
    return m_arg;
  }

  const ExpressionNode *getArgument() const {
    return m_arg;
  }

  int getFirstCoefIndex() const {
    return m_firstCoefIndex;
  }

  void setFirstCoefIndex(int index) {
    m_firstCoefIndex = index;
  }

  ExpressionNode       *expand();
  bool                  isExpandable() {
    return true;
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;
  Real evaluateReal() const;

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEPOLYNOMIAL;
  }

  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodeStmtList : public ExpressionNodeTree {
private:
  const ExpressionReturnType m_returnType;
  const ExpressionNode *doAssignments() const; // return last expression
public:
  ExpressionNodeStmtList(ParserTree *tree, const ExpressionNodeArray &childArray);
  ExpressionNodeStmtList(ParserTree *tree, const ExpressionNodeStmtList *src);

//  ExpressionNode            *expand();
//  bool                       isExpandable();

//  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

//  bool isConstant() const;
  ExpressionReturnType getReturnType() const {
    return m_returnType;
  }
  Real evaluateReal() const;
  bool evaluateBool() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODETREE;
  }
  String toString() const;
};

// ----------------------------------- Used by parser to save sourceposition in text -------------------------------------
class SourcePositionAttribute {
private:
  SourcePosition m_pos;
protected:
  SourcePositionAttribute(const SourcePosition &pos) : m_pos(pos) {
  }
public:
  inline const SourcePosition &getPos() const   {
    return m_pos;
  }
};

class ExpressionNodeNumberWithPos : public ExpressionNodeNumber, private SourcePositionAttribute {
public:
  inline ExpressionNodeNumberWithPos(ParserTree *tree, const SourcePosition &pos, const Real &value)
    : ExpressionNodeNumber(tree, value), SourcePositionAttribute(pos) {
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

class ExpressionNodeVariableWithPos : public ExpressionNodeVariable, private SourcePositionAttribute {
public:
  ExpressionNodeVariableWithPos(ParserTree *tree, const SourcePosition &pos, const String &name)
    : ExpressionNodeVariable(tree, name), SourcePositionAttribute(pos) {
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

class ExpressionNodeTreeWithPos : public ExpressionNodeTree, private SourcePositionAttribute {
public:
  ExpressionNodeTreeWithPos(ParserTree *tree, const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr)
    : ExpressionNodeTree(tree, symbol, argptr), SourcePositionAttribute(pos) {
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

class ExpressionNodePolyWithPos : public ExpressionNodePoly, private SourcePositionAttribute {
public:
  ExpressionNodePolyWithPos(ParserTree *tree, const SourcePosition &pos, va_list argptr)
    : ExpressionNodePoly(tree, argptr), SourcePositionAttribute(pos) {
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

class ExpressionNodeBoolExprWithPos : public ExpressionNodeBoolExpr, private SourcePositionAttribute {
public:
  ExpressionNodeBoolExprWithPos(ParserTree *tree, const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr)
    : ExpressionNodeBoolExpr(tree, symbol, argptr), SourcePositionAttribute(pos) {
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

// ------------------------------------------------------------------------------------------------------

class ExpressionNodeSum : public ExpressionNode {
private:
  AddentArray m_elements;
public:
  ExpressionNodeSum(ParserTree *tree, const AddentArray &elements);

  AddentArray &getAddentArray() {
    return m_elements;
  }
  const AddentArray &getAddentArray() const {
    return m_elements;
  }

  static ExpressionNodeSum *multiply(ExpressionNodeSum *n1, ExpressionNodeSum *n2);

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;
  Real evaluateReal() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODESUM;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodeProduct : public ExpressionNode {
private:
  FactorArray m_factors;
public:
  ExpressionNodeProduct(ParserTree *tree, FactorArray &factors);

  FactorArray &getFactorArray() {
    return m_factors;
  }
  const FactorArray &getFactorArray() const {
    return m_factors;
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;
  Real evaluateReal() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEPRODUCT;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodeSelector : public Selector<const ExpressionNode*> {
};

class ExpressionNodeSymbolSelector : public ExpressionNodeSelector {
private:
  const ExpressionSymbolSet *m_symbolSet;
  const bool                 m_ignoreMarked;
public:
  ExpressionNodeSymbolSelector(const ExpressionSymbolSet *symbolSet, bool ignoreMarked=false) : m_symbolSet(symbolSet), m_ignoreMarked(ignoreMarked) {
  }
  bool select(const ExpressionNode* const &n) {
    return (!m_ignoreMarked || !n->isMarked()) && ((m_symbolSet == NULL) || m_symbolSet->contains(n->getSymbol()));
  }
  AbstractSelector *clone() const {
    return new ExpressionNodeSymbolSelector(m_symbolSet, m_ignoreMarked);
  }
};

}; // namespace Expr
