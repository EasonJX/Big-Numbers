#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNodeProduct::ExpressionNodeProduct(ParserTree *tree, FactorArray &factors)
: ExpressionNodeTree(tree, PRODUCT, factors)
{
#ifdef _DEBUG
  validateFactorArray(getFactorArray());
#endif
  getFactorArray().sort();
  SETDEBUGSTRING();
}

void ExpressionNodeProduct::validateFactorArray(const FactorArray &factors) const {
  const size_t sz = factors.size();
  for(size_t i = 0; i < sz; i++) {
    const SNode &n = factors[i];
    CHECKNODETYPE(n,NT_POWER);
  }
}

ExpressionNodeProduct::ExpressionNodeProduct(ParserTree *tree, const ExpressionNodeProduct *src)
: ExpressionNodeTree(tree, src)
{
}

int ExpressionNodeProduct::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  const FactorArray &a            = getFactorArray();
  const FactorArray &na           = n->getFactorArray();
  const size_t       count        = min(a.size(), na.size());
  int                constCompare = 0;
  size_t             i;
  for(i = 0; i < count; i++) {
    ExpressionNode *ai = a[i].node(), *nai = na[i].node();
    const int c = ai->compare(nai);
    if(c) {
      if(nai->isConstant() && ai->isConstant()) {
        if(constCompare == 0) {
          constCompare = c;
        }
        continue;
      }
      return c;
    }
  }
  if(i == a.size()) {
    return (i == na.size()) ? constCompare : 1;
  } else {
    return -1;
  }
}

ExpressionNode *ExpressionNodeProduct::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeProduct(tree, this); TRACE_NEW(n);
  return n;
}

Real ExpressionNodeProduct::evaluateReal() const {
  Real prod = 1;
  const FactorArray &a = getFactorArray();
  const size_t       n = a.size();
  for(size_t i = 0; i < n; i++) {
    prod *= a[i].evaluateReal();
  }
  return prod;
}

String ExpressionNodeProduct::toString() const {
  return _T("PROD") + getFactorArray().toString();
}

}; // namespace Expr
