#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ParserTreeComplexity::ParserTreeComplexity(const ParserTree &tree) {
  m_nodeCount = tree.getNodeCount(true);
  m_nameCount = tree.getNodeCount(true, &ExpressionSymbolSet(NAME,EOI));
  m_treeDepth = tree.getRoot() ? tree.getRoot()->getMaxTreeDepth() : 0;
}

int ParserTreeComplexity::compare(const ParserTreeComplexity &tc) const {
  int c = (m_nodeCount + 2 * m_nameCount) - (tc.m_nodeCount + 2 * tc.m_nameCount);
  if(c) return c;
  return m_treeDepth - tc.m_treeDepth;
}

ParserTreeComplexity ParserTree::getComplexity() const {
  return ParserTreeComplexity(*this);
}

}; // namespace Expr
