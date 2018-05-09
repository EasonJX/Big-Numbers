#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

// ---------------------------------------- Operators for StandardForm ----------------------------------------------------
// Not using SUM and PRODUCT nodes

class NodeOperatorsStdForm : public NodeOperators {
public:
  ExpressionNode *minus(     ExpressionNode *n) const;
  ExpressionNode *reciprocal(ExpressionNode *n) const;
  ExpressionNode *sqr(       ExpressionNode *n) const;
  ExpressionNode *sqrt(      ExpressionNode *n) const;
  ExpressionNode *exp(       ExpressionNode *n) const;
  ExpressionNode *exp10(     ExpressionNode *n) const;
  ExpressionNode *exp2(      ExpressionNode *n) const;
  ExpressionNode *cot(       ExpressionNode *n) const;
  ExpressionNode *csc(       ExpressionNode *n) const;
  ExpressionNode *sec(       ExpressionNode *n) const;

  ExpressionNode *sum(       ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *diff(      ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *prod(      ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *quot(      ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *mod(       ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *power(     ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *root(      ExpressionNode *n1, ExpressionNode *n2) const;
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_STANDARD;
  }
};

class NodeOperatorsStdNumForm : public NodeOperatorsStdForm {
public:
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_NUMERIC;
  }
};

ExpressionNode *NodeOperatorsStdForm::minus(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER:
    if(n->isZero()) {
      return n;
    } else if(n->isOne()) {
      return getMinusOne(n);
    } else if(n->isMinusOne()) {
      return getOne(n);
    } else {
      return numberExpression(n,-n->getNumber());
    }
  case MINUS:
    if(n->isUnaryMinus()) {
      return n->left();
    } else {
      return diff(n->right(), n->left());
    }
  default:
    return unaryExpr(MINUS, n);
  }
}

ExpressionNode *NodeOperatorsStdForm::reciprocal(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER:
    { const Number &v = n->getNumber();
      if(v.isRational()) {
        return numberExpression(n,::reciprocal(v.getRationalValue()));
      }
    }
    break;

  case QUOT: return quot(n->right(), n->left());
  case EXP : return functionExpr(EXP      , minus(n->left()));
  case POW : return power(   n->left() , minus(n->right()));
  case ROOT: return root(    n->left() , minus(n->right()));
  case COS : return functionExpr(SEC, n->left());
  case SIN : return functionExpr(CSC, n->left());
  case TAN : return functionExpr(COT, n->left());
  case COT : return functionExpr(TAN, n->left());
  case SEC : return functionExpr(COS, n->left());
  case CSC : return functionExpr(SIN, n->left());
  }
  return quot(getOne(n), n);
}

ExpressionNode *NodeOperatorsStdForm::sum(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() + n2->getRational());
  }
  if(n1->isZero()) {
    return n2;
  } else if(n2->isZero()) {
    return n1;
  } else if(n1->isUnaryMinus()) {
    return diff(n2, n1->left());
  } else if(n2->isUnaryMinus()) {
    return diff(n1, n2->left());
  } else {
    return binaryExpr(PLUS, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::diff(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() - n2->getRational());
  }
  if(n2->isZero()) {
    return n1;
  } else if(n1->isZero()) {
    return minus(n2);
  } else if(n2->isUnaryMinus()) {
    return sum(n1, n2->left());
  } else {
    return binaryExpr(MINUS, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::prod(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() * n2->getRational());
  }
  if(n1->isZero() || n2->isZero()) {
    return getZero(n1);
  } else if(n1->isOne()) {
    return n2;
  } else if(n2->isOne()) {
    return n1;
  } else if(n1->isMinusOne()) {
    return minus(n2);
  } else if(n2->isMinusOne()) {
    return minus(n1);
  } else {
    return binaryExpr(PROD, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::quot(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() / n2->getRational());
  }
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return minus(n1);
  } else if(n1->isZero()) {
    return n1;
  } else {
    return binaryExpr(QUOT, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::mod(ExpressionNode *n1, ExpressionNode *n2) const {
  return binaryExpr(MOD, n1, n2);
}

ExpressionNode *NodeOperatorsStdForm::sqr(ExpressionNode *n) const {
  return functionExpr(SQR, n);
}

ExpressionNode *NodeOperatorsStdForm::sqrt(ExpressionNode *n) const {
  return functionExpr(SQRT, n);
}

ExpressionNode *NodeOperatorsStdForm::power(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isInteger()) {
    return numberExpression(n1, pow(n1->getRational(), n2->getNumber().getIntValue()));
  }
  if(n1->isEulersConstant()) {
    if(n2->isZero()) {
      return getOne(n1);
    } else if(n2->isOne()) {
      return n1;
    }
    return exp(n2);
  } else if(n1->getSymbol() == POW) {
    return binaryExpr(POW, n1->left(), prod(n1->right(), n2));
  }

  if(n2->isNumber()) {
    const Number &e = n2->getNumber();
    if(e.isRational()) {
      const Rational eR = e.getRationalValue();
      if(eR.isZero()) {
        return getOne(n1);
      } else if(eR == 1) {
        return n1;
      } else if(eR == -1) {
        return reciprocal(n1);
      } else if(eR.getNumerator() == 1) {
        return root(n1, numberExpression(n1,eR.getDenominator()));
      } else if(eR.getNumerator() == -1) {
        return reciprocal(root(n1, numberExpression(n1,eR.getDenominator())));
      }
    }
  }
  return binaryExpr(POW, n1, n2);
}

ExpressionNode *NodeOperatorsStdForm::root(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocal(n1);
  }
  return binaryExpr(ROOT, n1, n2);
}

ExpressionNode *NodeOperatorsStdForm::exp(ExpressionNode *n) const {
  return functionExpr(EXP, n);
}

ExpressionNode *NodeOperatorsStdForm::exp10(ExpressionNode *n) const {
  return functionExpr(EXP10, n);
}

ExpressionNode *NodeOperatorsStdForm::exp2(ExpressionNode *n) const {
  return functionExpr(EXP2, n);
}

ExpressionNode *NodeOperatorsStdForm::cot(ExpressionNode *n) const {
  return functionExpr(COT, n);
}

ExpressionNode *NodeOperatorsStdForm::csc(ExpressionNode *n) const {
  return functionExpr(CSC, n);
}

ExpressionNode *NodeOperatorsStdForm::sec(ExpressionNode *n) const {
  return functionExpr(SEC, n);
}

static const NodeOperatorsStdForm    stdFormOps;
static const NodeOperatorsStdNumForm stdNumFormOps;

const NodeOperators *NodeOperators::s_stdForm    = &stdFormOps;
const NodeOperators *NodeOperators::s_stdNumForm = &stdNumFormOps;

// ------------------------------- tree conversion classes/functions ------------------
// ----------------------------------------- toStandardForm ------------------------------------------------------------------

class StdNode : public SNode {
private:
  StdNode toSForm() const;
  StdNode toSFormSum() const;
  StdNode toSFormProduct() const;
  StdNode toSFormPow() const;
  StdNode toSFormPoly() const;
  StdNode toSFormTreeNode() const;
  StdNode toSFormFactorArray(FactorArray &a, bool changeExponentSign) const;
  inline StdNode(const SNode &n) : SNode(n) {
  }
public:
  inline StdNode(ExpressionNode *n) : SNode(n) {
  }
  ExpressionNode *convert() const {
    return toSForm();
  }
};

// Eliminate all Product- and Sum nodes
StdNode StdNode::toSForm() const {
  switch(getSymbol()) {
  case NUMBER :
  case NAME   : return *this;
  case SUM    : return toSFormSum();
  case PRODUCT: return toSFormProduct();
  case POW    : return toSFormPow();
  case POLY   : return toSFormPoly();
  default     : return toSFormTreeNode();
  }
}

#define N(n)  StdNode(n)
#define NV(v) SNode(getTree(),v)

static int compare2(SumElement * const &e1, SumElement * const &e2) {
  const bool p1 = e1->isPositive();
  const bool p2 = e2->isPositive();
  int c = p2 - p1;
  if(c) return c;
  return e1->compare(e2);
}

static int compareMany(SumElement * const &e1, SumElement * const &e2) {
  return e1->compare(e2);
}

static void sortAddentArrayStdForm(AddentArray &a) {
  if(a.size() == 2) {
    a.sort(compare2);
  } else {
    a.sort(compareMany);
  }
}

StdNode StdNode::toSFormSum() const {
  AddentArray a = getAddentArray();
  if(a.size() == 0) {
    return _0();
  } else {
    sortAddentArrayStdForm(a);
    StdNode result = N(a[0]->getNode()).toSForm(); // not createExpressionNode here. We'll get infinite recursion
    if(!a[0]->isPositive()) result = -result;
    for(size_t i = 1; i < a.size(); i++) {
      SumElement *e = a[i];
      SNode ne = N(e->getNode()).toSForm();
      if(e->isPositive()) result += ne; else result -= ne;
    }
    return result;
  }
}

StdNode StdNode::toSFormProduct() const {
  Rational constant = 1;

  const FactorArray &a = getFactorArray();
  FactorArray newArray;
  for(size_t i = 0; i < a.size(); i++) {
    ExpressionFactor *f = a[i];
    Rational r;
    if(f->reducesToRationalConstant(&r)) {
      constant *= r;
    } else {
      newArray.add(f);
    }
  }
  FactorArray p,q;
  if(constant.getNumerator() != 1) {
    p.add(NV(constant.getNumerator()));
  }
  if(constant.getDenominator() != 1) {
    q.add(NV(constant.getDenominator()), _m1());
  }

  p.addAll(newArray.selectConstantPositiveExponentFactors());
  p.addAll(newArray.selectNonConstantExponentFactors());
  q.addAll(newArray.selectConstantNegativeExponentFactors());

  return toSFormFactorArray(p, false) / toSFormFactorArray(q, true);
}

StdNode StdNode::toSFormFactorArray(FactorArray &a, bool changeExponentSign) const {
  SNodeArray a1;
  for(size_t i = 0; i < a.size(); i++) {
    ExpressionFactor *f = a[i];
    if(changeExponentSign) {
      a1.add(reciprocal(N(f).toSFormPow()));
    } else {
      a1.add(N(f).toSFormPow());
    }
  }
  if(a1.size() == 0) {
    return _1();
  } else {
    SNode result = a1[0];
    for(size_t i = 1; i < a1.size(); i++) result *= a1[i];
    return result;
  }
}

StdNode StdNode::toSFormPow() const {
  const SNode base     = N(left()).toSForm();
  const SNode exponent = N(right()).toSForm();

  Rational expoR;
  if(exponent.reducesToRationalConstant(&expoR)) {
    const __int64 &num = expoR.getNumerator();
    if(expoR.isInteger()) {
      switch(num) {
      case  0 : return _1();
      case  1 : return base;
      case -1:
        switch(base.getSymbol()) {
        case COS: return sec(base.left());
        case SIN: return csc(base.left());
        case TAN: return cot(base.left());
        default : return reciprocal(base);
        }
      case  2: return sqr(base);
      case -2: return reciprocal(sqr(base));
      default:
        if(num > 0) {
          return pow(base, NV(expoR.getNumerator()));
        } else {
          return reciprocal(pow(base, NV(-num)));
        }
      }
    } else { // r not integer
      const __int64 &den = expoR.getDenominator(); // > 0
      if(abs(num) == 1) {
        if(den == 2) {
          if(num == 1) {
            return sqrt(base);
          } else { // num == -1
            return reciprocal(sqrt(base));
          }
        } else {
          if(num == 1) {
            return root(base, NV(den));
          } else { // num == -1
            return reciprocal(root(base, NV(den)));
          }
        }
      } else { // num != 1 and num != -1 and r not integer
        if(num > 0) {
          return root(pow(base, NV(num)), NV(den));
        } else {
          return reciprocal(root(pow(base, NV(-num)), NV(den)));
        }
      }
    }
  } else { // exponent not rational constant
    if(exponent.getSymbol() == QUOT) {
      return root(pow(base, exponent.left()), exponent.right());
    } else {
      return pow(base, exponent);
    }
  }
}

StdNode StdNode::toSFormPoly() const {
  const ExpressionNodeArray &coefArray = getCoefficientArray();
  StdNode                    arg       = getArgument();

  ExpressionNodeArray newCoefArray(coefArray.size());
  for(size_t i = 0; i < coefArray.size(); i++) {
    newCoefArray.add(N(coefArray[i]).toSForm());
  }
  return polyExp(newCoefArray, arg.toSForm());
}

StdNode StdNode::toSFormTreeNode() const {
  const ExpressionNodeArray &a = getChildArray();
  ExpressionNodeArray        newChildArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(N(a[i]).toSForm());
  }
  return treeExp(getSymbol(), newChildArray);
}

// -------------------------------------------------------------------------------------------------------

class StandardFormChecker : public ExpressionNodeHandler {
private:
  BitSet m_illegalSymbolSet;
  String m_error;
  bool   m_ok;
public:
  StandardFormChecker();
  bool handleNode(ExpressionNode *n, int level);
  bool isOk() const {
    return m_ok;
  }
  const String &getErrorMessage() const {
    return m_error;
  }
};

StandardFormChecker::StandardFormChecker() : m_illegalSymbolSet(ParserTree::getTerminalCount() + 1) {
  static const ExpressionInputSymbol table[] = {
    PRODUCT
   ,SUM
  };
  for(int i = 0; i < ARRAYSIZE(table); i++) {
    m_illegalSymbolSet.add(table[i]);
  }
  m_ok = true;
}

bool StandardFormChecker::handleNode(ExpressionNode *n, int level) {
  if(m_illegalSymbolSet.contains(n->getSymbol())) {
    m_error = format(_T("Illegal symbol in standard form:<%s>. node=<%s>"), n->getSymbolName().cstr(), n->toString().cstr());
    m_ok = false;
    return false;
  }
  return true;
}

void ParserTree::checkIsStandardForm() {
  StandardFormChecker checker;
  traverseTree(checker);
  if(!checker.isOk()) {
    throwException(checker.getErrorMessage());
  }
}

ExpressionNode *ParserTree::toStandardForm(ExpressionNode *n) {
  if((getTreeForm() == TREEFORM_STANDARD) || (n == NULL)) {
    return n;
  }
  m_ops = NodeOperators::s_stdForm;
  return StdNode(n).convert();
}
