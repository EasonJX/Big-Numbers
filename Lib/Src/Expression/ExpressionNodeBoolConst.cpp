#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

int ExpressionNodeBoolConst::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return __super::compare(n);
  }
  return ordinal(getBool()) - ordinal(n->getBool());
}

ExpressionNode *ExpressionNodeBoolConst::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeBoolConst(tree, getBool()); TRACE_NEW(n);
  return n;
}

bool ExpressionNodeBoolConst::traverseExpression(ExpressionNodeHandler &handler, int level) {
  return handler.handleNode(this, level);
}

void ExpressionNodeBoolConst::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("BOOLEAN:%s\n"), boolToStr(getBool()));
}

}; // namespace Expr
