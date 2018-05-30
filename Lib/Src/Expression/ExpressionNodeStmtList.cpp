#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNodeStmtList::ExpressionNodeStmtList(ParserTree *tree, const SNodeArray &childArray) 
  : ExpressionNodeTree(tree, STMTLIST, childArray)
  , m_returnType(childArray.last().getReturnType())
{
  SETDEBUGSTRING();
}
ExpressionNodeStmtList::ExpressionNodeStmtList(ParserTree *tree, const ExpressionNodeStmtList *src)
  : ExpressionNodeTree(tree, src)
  , m_returnType(src->getReturnType())
{
  SETDEBUGSTRING();
}

ExpressionNode *ExpressionNodeStmtList::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeStmtList(tree, this); TRACE_NEW(n);
  return n;
}

Real ExpressionNodeStmtList::evaluateReal() const {
  if(m_returnType != EXPR_RETURN_REAL) UNSUPPORTEDOP();
  SNode last = doAssignments();
  return last.evaluateReal();
}

bool ExpressionNodeStmtList::evaluateBool() const {
  if(m_returnType != EXPR_RETURN_BOOL) UNSUPPORTEDOP();
  SNode last = doAssignments();
  return last.evaluateBool();
}

SNode ExpressionNodeStmtList::doAssignments() const {
  const SNodeArray &list = getChildArray();
  const size_t      n    = list.size() - 1;
  for(size_t i = 0; i < n; i++) {
    list[i].doAssignment();
  }
  return list.last();
}

String ExpressionNodeStmtList::toString() const {
  String result;
  const SNodeArray &list = getChildArray();
  const size_t      n    = list.size() - 1;
  for(size_t i = 0; i < n; i++) {
    result += list[i].toString();
    result += _T(";\n");
  }
  result += list.last().toString();
  return result;
}

}; // namespace Expr
