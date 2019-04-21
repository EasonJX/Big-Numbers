#pragma once

#include <MyUtil.h>
#include <HashMap.h>
#include <PropertyContainer.h>
#include <Scanner.h>
#include "ParserTreeSymbolTable.h"

namespace Expr {

typedef enum {
  PP_STATE                 // ParserTreeState
 ,PP_REDUCEITERATION       // UINT
 ,PP_ROOT                  // ExpressionNode
 ,PP_OK                    // bool
 ,PP_TREEFORM              // ParserTreeForm
 ,EP_TRIGONOMETRICMODE     // TrigonometricMode    (expressionProperty)
 ,EP_RETURNTYPE            // ExpressionReturnType (expressionProperty)
 ,EP_MACHINECODE           // bool                 (expressionProperty)
} ParserTreeProperties;

typedef enum {
  TREEFORM_STANDARD
 ,TREEFORM_CANONICAL
 ,TREEFORM_NUMERIC
} ParserTreeForm;

typedef enum {
  PS_EMPTY
 ,PS_COMPILED
 ,PS_DERIVED
 ,PS_CANONICALFORM
 ,PS_MAINREDUCTION1
 ,PS_MAINREDUCTION2
 ,PS_STANDARDFORM
 ,PS_REDUCTIONDONE
} ParserTreeState;

class ParserTreeComplexity {
private:
  UINT m_nodeCount;
  UINT m_nameCount;
  UINT m_treeDepth;
  int compare(const ParserTreeComplexity &tc) const;
public:
  ParserTreeComplexity(const ParserTree &tree);
  inline bool operator==(const ParserTreeComplexity &tc) const {
    return compare(tc) == 0;
  }
  inline bool operator!=(const ParserTreeComplexity &tc) const {
    return compare(tc) != 0;
  }
  inline bool operator< (const ParserTreeComplexity &tc) const {
    return compare(tc) <  0;
  }
  inline bool operator<=(const ParserTreeComplexity &tc) const {
    return compare(tc) <= 0;
  }
  inline bool operator> (const ParserTreeComplexity &tc) const {
    return compare(tc) >  0;
  }
  inline bool operator>=(const ParserTreeComplexity &tc) const {
    return compare(tc) >= 0;
  }
  inline UINT getNodeCount() const {
    return m_nodeCount;
  }
  String toString() const {
    return format(_T("#nodes:%3u, #names:%2u, treedepth:%u\n"), m_nodeCount, m_nameCount, m_treeDepth);
  }
};

class ParserTreeTransformer {
public:
  ParserTreeTransformer() {
  }
  virtual SNode           transform(SNode n) = 0;
  virtual ParserTreeState getState() const = 0;
};


// Only an interface....do not save any state in this, or any derived classes
class NodeOperators {
protected:
  static ExpressionNode  *getZero(           ExpressionNode *n);
  static ExpressionNode  *getOne(            ExpressionNode *n);
  static ExpressionNode  *getMinusOne(       ExpressionNode *n);
  static ExpressionNode  *getTwo(            ExpressionNode *n);
  static ExpressionNode  *getHalf(           ExpressionNode *n);
  static ExpressionNode  *getTen(            ExpressionNode *n);
  static ExpressionNode  *numberExpr(        ExpressionNode *n, const Number &v);
  static ExpressionNode  *numberExpr(        ExpressionNode *n, INT64         v);

  static ExpressionNode  *unaryExpr( ExpressionInputSymbol symbol, ExpressionNode *n);
  static ExpressionNode  *binaryExpr(ExpressionInputSymbol symbol, ExpressionNode *n1, ExpressionNode *n2);
  static ExpressionNode  *funcExpr(  ExpressionInputSymbol symbol, ExpressionNode *n);
public:
  virtual ExpressionNode *sum(       ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *diff(      ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *prod(      ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *quot(      ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  // Return a division node without reduction to rational. to be used with exponents ie sqrt(x^2) != x
  virtual ExpressionNode *quot(      ParserTree *tree, INT64 num, INT64 den) const = NULL;
  virtual ExpressionNode *mod(       ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *power(     ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *root(      ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *minus(     ExpressionNode *n) const = NULL;
  virtual ExpressionNode *reciprocal(ExpressionNode *n) const = NULL;
  virtual ExpressionNode *sqr(       ExpressionNode *n) const = NULL;
  virtual ExpressionNode *sqrt(      ExpressionNode *n) const = NULL;
  virtual ExpressionNode *exp(       ExpressionNode *n) const = NULL;
  virtual ExpressionNode *exp10(     ExpressionNode *n) const = NULL;
  virtual ExpressionNode *exp2(      ExpressionNode *n) const = NULL;
  virtual ExpressionNode *cot(       ExpressionNode *n) const = NULL;
  virtual ExpressionNode *csc(       ExpressionNode *n) const = NULL;
  virtual ExpressionNode *sec(       ExpressionNode *n) const = NULL;

  virtual ParserTreeForm  getTreeForm() const = NULL;
  virtual void            checkTreeFormConsistent(const ParserTree *tree) const = NULL;
  static const NodeOperators *s_stdForm, *s_canonForm, *s_stdNumForm, *s_canonNumForm;
};

class RationalConstantMap : public CompactHashMap<Rational, ExpressionNodeNumber*> {
private:
  RationalConstantMap(           const RationalConstantMap &src); // not implemented
  RationalConstantMap &operator=(const RationalConstantMap &src); // not implemented
public:
  RationalConstantMap() {}
  void removeUnmarked();
};

class ParserTree : public PropertyContainer {
private:
  ExpressionNode               *m_root;
  CompactArray<ExpressionNode*> m_nodeTable, m_nonRootNodes;
  ParserTreeSymbolTable         m_symbolTable;
  StringArray                   m_errors;
  bool                          m_ok;
  const NodeOperators          *m_ops;
  TrigonometricMode             m_trigonometricMode;
  mutable ParserTreeState       m_state;
  mutable UINT                  m_reduceIteration;
  RationalConstantMap           m_rationalConstantMap;
  ExpressionNodeBoolConst      *m_false, *m_true;

  DEFINEREDUCTIONSTACK;
#ifdef TRACE_REDUCTION_CALLSTACK
  inline ParserTree &getTree() {
    return *this;
  }
#endif // TRACE_REDUCTION_CALLSTACK
private:
  inline void resetSimpleConstants() {
    m_false = m_true = NULL;
  }
  void markSimpleConstants();
  ExpressionNodeNumber *getRationalConstant(const Rational &r);
  void init(TrigonometricMode    mode
           ,ParserTreeState      state
           ,UINT                 reduceIteration);
  friend class ExpressionNode;
  friend class ExpressionNodeName;
  friend class NodeOperators;
  friend class NodeOperatorsCanonForm;
  friend class NodeOperatorsStdForm;
  friend class SNode;
  friend class ParserTreeSymbolTable;
  friend class FactorArray;
  friend class MarkedNodeTransformer;
  friend class ExpressionPainter;

#ifdef CHECK_CONSISTENCY
  void checkIsConsistent() const;
#endif // CHECK_CONSISTENCY
protected:
  ParserTree(TrigonometricMode mode);
  // if root!=NULL, it must be a node in src. If root=NULL, then src.root is used
  ParserTree(           const ParserTree &src, const ExpressionNode *root=NULL);
  ParserTree &operator=(const ParserTree &src);
  void setTrigonometricMode(TrigonometricMode mode) { // no notification from here.
    m_trigonometricMode = mode;
  }
  void setOk(             bool            ok      );
  void setTreeForm(       ParserTreeForm  form    );
  void setState(          ParserTreeState newState);
  // Reduction
  void setReduceIteration(UINT            it      );
  void iterateTransformation(ParserTreeTransformer &transformer);
  void checkIsStandardForm() const;
  void checkIsCanonicalForm() const;
  SNode toStandardForm( SNode n);
  SNode toCanonicalForm(SNode n);
  SNode toNumericForm(  SNode n);
  void releaseAll();
  void pruneUnusedNodes();
  void markPow1Nodes() const;
  void markNonRootNodes();
  void addNonRootNode(ExpressionNode *n);
  void deleteUnmarked();
  void buildSymbolTable(const ExpressionVariableArray *oldVariables = NULL) {
    m_symbolTable.create(this, oldVariables);
  }
  ExpressionVariable *getVariableByName(const String &name) {
    return m_symbolTable.getVariable(name);
  }
  inline void setValueByIndex(UINT valueIndex, Real value) const {
    getValueRef(valueIndex) = value;
  }
  inline Real getValueByIndex(UINT valueIndex) const {
    return getValueRef(valueIndex);
  }
  SNode traverseSubstituteNodes(SNode n, CompactNodeHashMap<ExpressionNode*> &nodeMap);

  ExpressionNode *allocateLoopVarNode(const String &prefix) {
    ExpressionNodeName *result = fetchNameNode(m_symbolTable.getNewLoopName(prefix));
    m_symbolTable.allocateSymbol(result, false, true, true);
    return result;
  }

  ExpressionNodeName     *fetchNameNode(  const String               &name    );
  // terminate argumentlist with NULL
  ExpressionNodeTree     *fetchTreeNode(  ExpressionInputSymbol       symbol, ...                );

  inline ExpressionNodeNumber *numberExpr(const Rational &v) {
    return getRationalConstant(v);
  }
  inline ExpressionNodeNumber *numberExpr(int             v) {
    return numberExpr(Rational(v));
  }
  inline ExpressionNodeNumber *numberExpr(UINT            v) {
    return numberExpr(Rational(v));
  }
  inline ExpressionNodeNumber *numberExpr(INT64           v) {
    return numberExpr(Rational(v));
  }
  inline ExpressionNodeNumber *numberExpr(const Number   &v) {
    if(isRational(v)) return numberExpr(getRational(v));
    ExpressionNodeNumber *n = new ExpressionNodeNumber(this, v); TRACE_NEW(n);
    return n;
  }
  inline ExpressionNodeNumber *numberExpr(const Real     &v) {
    ExpressionNodeNumber *n = new ExpressionNodeNumber(this, v); TRACE_NEW(n);
    return n;
  }
  inline ExpressionNodeBoolConst *boolConstExpr(bool b, bool checkIsSimple=true) {
    if(checkIsSimple) return b ? getTrue() : getFalse();
    ExpressionNodeBoolConst *n = new ExpressionNodeBoolConst(this, b); TRACE_NEW(n);
    return n;
  }
  ExpressionNode                 *constExpr(const String &name);

  void expandMarkedNodes();
  void multiplyMarkedNodes();

public:
  virtual ~ParserTree();

  inline const ExpressionNode *getRoot() const {
    return m_root;
  }

  inline ExpressionNode *getRoot() {
    return m_root;
  }

  void setRoot(ExpressionNode *n);

  // Do we have a root
  inline bool isEmpty() const {
    return m_root == NULL;
  }

  bool equal(const ParserTree &tree) const {
    return Expr::equal(getRoot(), tree.getRoot());
  }

  bool equalMinus(const ParserTree &tree) const {
    return Expr::equalMinus(getRoot(), tree.getRoot());
  }

  inline TrigonometricMode getTrigonometricMode() const {
    return m_trigonometricMode;
  }
  void reduce();
  static UINT getTerminalCount();

  void unmarkAll() const;
  inline bool isOk() const {
    return m_ok;
  }

  inline ParserTreeForm getTreeForm() const {
    return m_ops->getTreeForm();
  }
  inline String getTreeFormName() const {
    return getTreeFormName(getTreeForm());
  }

  static String getTreeFormName(ParserTreeForm treeForm);

  inline ParserTreeState getState() const {
    return m_state;
  }

  inline String getStateName() const {
    return getStateName(getState());
  }

  static String getStateName(ParserTreeState state);

  inline UINT getReduceIteration() const {
    return m_reduceIteration;
  }

  inline ExpressionNodeBoolConst *getFalse() {
    if(!m_false   ) m_false    = boolConstExpr(false,false);
    return m_false;
  }
  inline ExpressionNodeBoolConst *getTrue() {
    if(!m_true    ) m_true     = boolConstExpr(true,false);
    return m_true;
  }
  inline Real &getValueRef(UINT valueIndex) const {
    return m_symbolTable.getValueRef(valueIndex);
  }
  inline Real &getValueRef(const ExpressionVariable &var) const {
    return getValueRef(var.getValueIndex());
  }

  inline ExpressionNode  *sum(       ExpressionNode *n1, ExpressionNode *n2) { return m_ops->sum(  n1,n2); }
  inline ExpressionNode  *diff(      ExpressionNode *n1, ExpressionNode *n2) { return m_ops->diff( n1,n2); }
  inline ExpressionNode  *prod(      ExpressionNode *n1, ExpressionNode *n2) { return m_ops->prod( n1,n2); }
  inline ExpressionNode  *quot(      ExpressionNode *n1, ExpressionNode *n2) { return m_ops->quot( n1,n2); }
  // Return a division node without reduction to rational. to be used with exponents ie sqrt(x^2) != x
  inline ExpressionNode  *quot(      INT64 num, INT64 den) { return m_ops->quot(this,num,den); }
  inline ExpressionNode  *mod(       ExpressionNode *n1, ExpressionNode *n2) { return m_ops->mod(  n1,n2); }
  inline ExpressionNode  *power(     ExpressionNode *n1, ExpressionNode *n2) { return m_ops->power(n1,n2); }
  inline ExpressionNode  *root(      ExpressionNode *n1, ExpressionNode *n2) { return m_ops->root( n1,n2); }
  inline ExpressionNode  *minus(     ExpressionNode *n) { return m_ops->minus(n);      }
  inline ExpressionNode  *reciprocal(ExpressionNode *n) { return m_ops->reciprocal(n); }
  inline ExpressionNode  *abs(       ExpressionNode *n) { return funcExpr(ABS,n);      }
  inline ExpressionNode  *sqr(       ExpressionNode *n) { return m_ops->sqr(n);        }
  inline ExpressionNode  *sqrt(      ExpressionNode *n) { return m_ops->sqrt(n);       }
  inline ExpressionNode  *exp(       ExpressionNode *n) { return m_ops->exp(n);        }
  inline ExpressionNode  *exp10(     ExpressionNode *n) { return m_ops->exp10(n);      }
  inline ExpressionNode  *exp2(      ExpressionNode *n) { return m_ops->exp2(n);       }
  inline ExpressionNode  *cot(       ExpressionNode *n) { return m_ops->cot(n);        }
  inline ExpressionNode  *csc(       ExpressionNode *n) { return m_ops->csc(n);        }
  inline ExpressionNode  *sec(       ExpressionNode *n) { return m_ops->sec(n);        }

  ExpressionNode         *getStmtList(  SNode oldStmtList           , const SNodeArray     &newChildArray                   );
  ExpressionNode         *getAssignStmt(SNode oldAssign             , const SNodeArray     &newChildArray                   );
  ExpressionNode         *getTreeNode(  SNode oldTree               , const SNodeArray     &newChildArray                   );
  ExpressionNode         *getAnd(       SNode oldAnd                , SNode                 left            , SNode right   );
  ExpressionNode         *getOr(        SNode oldOr                 , SNode                 left            , SNode right   );
  ExpressionNode         *getNot(       SNode oldNot                , SNode                 left                            );
  ExpressionNode         *getPoly(      SNode oldPoly               , const CoefArray      &newCoefArray    , SNode newArg  );
  ExpressionNode         *getSum(       SNode oldSum                , const AddentArray    &newAddentArray                  );
  ExpressionNode         *getProduct(   SNode oldProduct            , const FactorArray    &newFactorArray                  );
  ExpressionNode         *getPower(     SNode oldPower              , SNode                 newBase         , SNode newExpo );

  ExpressionNode         *stmtList(                                   const SNodeArray     &stmtArray                       );
  ExpressionNode         *assignStmt(                                 const SNodeArray     &a                               );
  ExpressionNode         *assignStmt(                                 ExpressionNode *leftSide, ExpressionNode *expr        );
  ExpressionNode         *treeExpr(     ExpressionInputSymbol symbol, const SNodeArray     &a                               );
  ExpressionNode         *boolExpr(     ExpressionInputSymbol symbol, const SNodeArray     &a                               );
  ExpressionNode         *boolExpr(     ExpressionInputSymbol symbol, ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode         *polyExpr(                                   const CoefArray      &coefArray       , SNode arg     );
  ExpressionNode         *sumExpr(                                    const AddentArray    &addentArray);
  ExpressionNode         *productExpr(                                const FactorArray    &factorArray);
  ExpressionNode         *addentExpr(                                 SNode           child, bool positive);
  ExpressionNode         *powerExpr(                                  SNode           base);
  ExpressionNode         *powerExpr(                                  SNode           base , SNode exponent);
  ExpressionNode         *powerExpr(                                  SNode           base , const Rational &exponent);
  // symbol must be INDEXEDSUM/INDEXPRODUCT
  ExpressionNode         *indexedExpr(  ExpressionInputSymbol symbol, SNode           assign, SNode endExpr, SNode expr);
  ExpressionNode         *indexedSum(                                 SNode           assign, SNode endExpr, SNode expr);
  ExpressionNode         *indexedProduct(                             SNode           assign, SNode endExpr, SNode expr);
  ExpressionNode         *condExpr(                                   SNode           condition, SNode exprTrue,SNode exprFalse);

  ExpressionNode         *expandPower(  ExpressionNode *base, const Rational &exponent);

  // used by parser
  ExpressionNode         *vFetchNode(const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr);

  ExpressionNode *and(                       ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *or(                        ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *not(                       ExpressionNode *n);

  ExpressionNode *funcExpr(                  ExpressionInputSymbol symbol
                                            ,ExpressionNode *child);
  ExpressionNode *unaryMinus(                ExpressionNode *child);
  ExpressionNode *unaryExpr(                 ExpressionInputSymbol symbol
                                            ,ExpressionNode *child);
  ExpressionNode *binaryExpr(                ExpressionInputSymbol symbol
                                            ,ExpressionNode *left
                                            ,ExpressionNode *right);
  ExpressionNode *ternaryExpr(               ExpressionInputSymbol symbol
                                            ,ExpressionNode *child0
                                            ,ExpressionNode *child1
                                            ,ExpressionNode *child2);

  // if both n1 and n2 are rational constants, they will be reduced as much as possible
  // without loosing symmetry with even exponents. if one or both are not rational
  // these 2 functions will just return as normal (prod/quot)
  ExpressionNode *multiplyExponents(         ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *divideExponents(           ExpressionNode *n1, ExpressionNode *n2);

  void addError( ExpressionNode *n              , _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void addError( const SourcePosition       &pos, _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void addError(                                  _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void vAddError(const SourcePosition       *pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);

  const StringArray &getErrors() const {
    return m_errors;
  }

  // Error should be an element from StringArray returned by getErrors().
  // Will return sourcePosition specified in error as "(line,col):errorText"
  // and modify error to be text after "(line,col):"
  // If no leading "(line,col):" an Exception is thrown
  static SourcePosition decodeErrorString(String &error);

  // Error should be an element from StringArray returned by getErrors().
  // Will return textposition in expr, remove the textposition "(line,column)" from error
  // If no leading "(line,col):" an Exception is thrown
  static inline UINT decodeErrorString(const String &expr, String &error) {
    return decodeErrorString(error).findCharIndex(expr);
  }

  void listErrors(FILE *f = stdout) const;
  void listErrors(tostream &out) const;
  void listErrors(const TCHAR *fname) const;
  inline void setValue(const String &name, const Real &value) {
    m_symbolTable.setValue(name,value);
  }
  inline const ExpressionVariable *getVariable(const String &name) const {
    return m_symbolTable.getVariable(name);
  }

  UINT getNodeCount(ExpressionNodeSelector *selector = NULL) const;
  // if(validSymbolSet != NULL, only node with symbols contained in set will be counted
  UINT getNodeCount(bool ignoreMarked, const ExpressionSymbolSet *validSymbolSet = NULL) const;
  UINT getTreeDepth() const;
  ParserTreeComplexity getComplexity() const;
#ifdef TRACE_REDUCTION_CALLSTACK
  inline ReductionStack &getReductionStack() {
    return m_reductionStack;
  }
  void clearAllBreakPoints();
#endif // TRACE_REDUCTION_CALLSTACK

  int getNodeTableSize() const {
    return (int)m_nodeTable.size();
  }
  void substituteNodes(CompactNodeHashMap<ExpressionNode*> &nodeMap);
  void traverseTree(ExpressionNodeHandler &handler);
  const ParserTreeSymbolTable &getSymbolTable() const {
    return m_symbolTable;
  }
  const RationalConstantMap &getRationalConstantMap() const {
    return m_rationalConstantMap;
  }
  String treeToString() const;
};

}; // namespace Expr
