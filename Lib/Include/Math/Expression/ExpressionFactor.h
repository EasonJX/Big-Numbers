#pragma once

#include "ExpressionNode.h"

namespace Expr {

class ExpressionFactor : public ExpressionNodeTree {
public:
  ExpressionFactor(SNode base);
  ExpressionFactor(SNode base, SNode exponent);

  inline SNode base() const {
    return child(0);
  }

  inline SNode exponent() const {
    return child(1);
  }

  bool hasOddExponent() const {
    return exponent().isNumber() && exponent().getNumber().isOdd();
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;

  ExpressionNodeType getNodeType() const {
    return NT_FACTOR;
  }

  void dumpNode(String &s, int level) const;
};

}; // namespace Expr
