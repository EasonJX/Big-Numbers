#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

namespace Expr {

// ------------------------------ toNForm -------------------------------------------------------------------------

class NNode : public SNode {
private:
  NNode toNForm() const;
  NNode toNFormRealExp() const;
  NNode toNFormBoolExp() const;
  NNode toNFormSum() const;
  NNode toNFormProduct() const;
  NNode toNFormPoly() const;
  NNode toNFormTreeNode() const;
  FactorArray &toNFormProduct(FactorArray &result, SNode &exponent) const;
  AddentArray &toNFormSum(    AddentArray &result, bool   positive) const;
  FactorArray &toNFormPower(  FactorArray &result, SNode &exponent) const;
  FactorArray &toNFormRoot(   FactorArray &result, SNode &exponent) const;

  inline NNode(const SNode &n) : SNode(n) {
  }
public:
  inline NNode(ExpressionNode *n) : SNode(n) {
  }
  ExpressionNode *convert() const {
    return toNForm().node();
  }
};

#define N(n)  NNode(n)
#define NV(v) SNode(getTree(),v)

NNode NNode::toNForm() const {
  DEFINEMETHODNAME;

  const SStmtList &stmtList = getChildArray();
  SStmtList       newStmtList;

  const int stmtCount = (int)stmtList.size() - 1;
  for(int i = 0; i < stmtCount; i++) {
    const SNode &stmt = stmtList[i];
    newStmtList.add(assignStmt(stmt.left(), N(stmt.right()).toNFormRealExp()));
  }
  SNode last = stmtList.last();
  switch(last.getReturnType()) {
  case EXPR_RETURN_REAL:
    newStmtList.add(N(last.left()).toNFormRealExp());
    break;
  case EXPR_RETURN_BOOL:
    newStmtList.add(N(last.left()).toNFormBoolExp());
    break;
  default:
    last.throwUnknownSymbolException(method);
  }
  return newStmtList.removeUnusedAssignments();
}

NNode NNode::toNFormRealExp() const {
  DEFINEMETHODNAME;
  if(isConstant()) {
    return NV(evaluateReal());
  }
  switch(getSymbol()) {
  case NUMBER    : throwException(_T("%s:Unexpected node-symbol (NUMBER)"), method);
  case NAME      : return *this;
  case SUM       : return toNFormSum();
  case PRODUCT   : return toNFormProduct();
  case POLY      : return toNFormPoly();
  default        : return toNFormTreeNode();
  }
}

NNode NNode::toNFormBoolExp() const {
  DEFINEMETHODNAME;
  if(isConstant()) {
    return NV(evaluateBool());
  }
  switch(getSymbol()) {
  case NOT  :
    return !N(left()).toNFormBoolExp();
  case AND   :
    return N(left()).toNFormBoolExp() && N(right()).toNFormBoolExp();
  case OR    :
    return N(left()).toNFormBoolExp() || N(right()).toNFormBoolExp();

  case EQ    :
  case NE    :
  case LE    :
  case LT    :
  case GE    :
  case GT    :
    { NNode l = N(left()).toNFormRealExp();
      SNode r = N(right()).toNFormRealExp();
      return binExp(getSymbol(), l, r);
    }
  default    :
    throwUnknownSymbolException(method);
    return NULL;
  }
}

NNode NNode::toNFormSum() const {
  const AddentArray &a = getAddentArray();
  if(a.size() == 0) {
    return _0();
  } else {
    Real constant = 0;
    AddentArray newArray;
    for(size_t i = 0; i < a.size(); i++) {
      SumElement *e = a[i];
      SNode tmp = N(e->getNode()).toNFormRealExp();
      if(tmp.isNumber()) {
        if(e->isPositive()) constant += tmp.getReal(); else constant -= tmp.getReal();
      } else {
        newArray.add(tmp, e->isPositive());
      }
    }
    if(newArray.size() == 0) return NV(constant);
    SNode result = newArray[0]->getNode();
    if(!newArray[0]->isPositive()) result = -result;
    for(size_t i = 1; i < newArray.size(); i++) {
      SumElement *e  = newArray[i];
      SNode       ne = e->getNode();
      if(e->isPositive()) result += ne; else result -= ne;
    }
    return (constant == 0) ? result
         : (constant >  0) ? result + NV(constant)
                           : result - NV(-constant);
  }
}

NNode NNode::toNFormProduct() const {
  Real constant = 1;
  const FactorArray &a = getFactorArray();
  FactorArray newArray;
  for(size_t i = 0; i < a.size(); i++) {
    ExpressionFactor *f        = a[i];
    NNode             base     = N(f->base()).toNFormRealExp();
    NNode             exponent = N(f->exponent()).toNFormRealExp();
    if(base.isNumber() && exponent.isNumber()) {
      constant *= N(f).evaluateReal();
    } else {
      newArray.add(base, exponent);
    }
  }
  if(newArray.size() == 0) return NV(constant);

  FactorArray p = newArray.selectConstantPositiveExponentFactors();
  p.addAll(newArray.selectNonConstantExponentFactors());
  FactorArray q = newArray.selectConstantNegativeExponentFactors();

  SNode result;
  if(p.size() > 0) {
    result = newArray[0];
    for(size_t i = 1; i < p.size(); i++) {
      result *= p[i];
    }
  }
  if(result.isEmpty()) {
    result = NV(constant);
  } else {
    if(constant != 1) result *= NV(constant);
  }
  for(size_t i = 0; i < q.size(); i++) {
    result /= reciprocal(q[i]);
  }
  return result;
}

NNode NNode::toNFormPoly() const {
  const SNodeArray &coefArray = getCoefArray();
  NNode             arg       = getArgument();

  SNodeArray newCoefArray(coefArray.size());
  for(size_t i = 0; i < coefArray.size(); i++) {
    newCoefArray.add(N(coefArray[i]).toNFormRealExp());
  }
  return polyExp(newCoefArray, arg.toNFormRealExp());
}

NNode NNode::toNFormTreeNode() const {
  if(isBooleanOperator()) {
    return toNFormBoolExp();
  }
  switch(getSymbol()) {
  case IIF:
    { const NNode cond   = N(child(0)).toNFormBoolExp();
      const NNode eTrue  = N(child(1)).toNFormRealExp();
      const NNode eFalse = N(child(2)).toNFormRealExp();
      if(cond.isTrue()) return eTrue; else if(cond.isFalse()) return eFalse;
      return condExp(cond, eTrue, eFalse);
    }
  default:
    { const SNodeArray &a = getChildArray();
      SNodeArray        newChildArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(N(a[i]).toNFormRealExp());
      }
      return treeExp(getSymbol(), newChildArray);
    }
  }
}

ExpressionNode *ParserTree::toNumericForm(ExpressionNode *n) {
  if(n == NULL) return n;
  switch(getTreeForm()) {
  case TREEFORM_STANDARD :
    n = toStandardForm(N(toCanonicalForm(n)).convert());
    m_ops = NodeOperators::s_stdNumForm;
    break;

  case TREEFORM_CANONICAL:
    n = toCanonicalForm((N(n).convert()));
    m_ops = NodeOperators::s_canonNumForm;
    break;

  case TREEFORM_NUMERIC  :
    return n;
  }
  return n;
}

}; // namespace Expr
