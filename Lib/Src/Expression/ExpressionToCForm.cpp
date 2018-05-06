#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

#ifdef __NEVER_

Expression &Expression::toCanonicalForm() {
  if((getTreeForm() == TREEFORM_CANONICAL) || (getRoot() == NULL)) {
    return *this;
  }
  initDynamicOperations(TREEFORM_CANONICAL);
  setRoot(toCForm(getRoot()));
  pruneUnusedNodes();
  setTreeForm(TREEFORM_CANONICAL);
  return *this;
}

// ----------------------------------------- toCForm ------------------------------------------------------------------

// Replace PLUS,(binary) MINUS,PROD,QUOT,ROOT,SQRT,SQR,EXP,EXP10,EXP2,SEC,CSC,COT and negative constanst,
// with combinations of SUM, (unary MINUS), PRODUCT, POW, SIN,COS,TAN, and positive constants
SNode Expression::toCForm(ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NUMBER         :
  case NAME           : return n;

  case MINUS          : if(n->isUnaryMinus()) return -toCForm(n->left());
                        // NB continue case

  case PLUS           :
  case SUM            : return toCFormSum(n);

  case PRODUCT        :
  case PROD           :
  case QUOT           :
  case POW            :
  case SQR            :
  case ROOT           :
  case SQRT           :
  case EXP            :
  case EXP10          :
  case EXP2           : return toCFormProduct(n);

  case SEC            : return sec(toCForm(n->left()));
  case CSC            : return csc(toCForm(n->left()));
  case COT            : return cot(toCForm(n->left()));

  case POLY           : return toCFormPoly(n);

  default             : return toCFormTreeNode(n);
  }
}

SNode Expression::toCFormSum(ExpressionNode *n) {
  AddentArray a;
  return getSum(toCFormSum(a, n, true));
}

AddentArray &Expression::toCFormSum(AddentArray &result, ExpressionNode *n, bool positive) {
  switch(n->getSymbol()) {
  case SUM:
    { const AddentArray &a = n->getAddentArray();
      for(size_t i = 0; i < a.size(); i++) {
        SumElement *e = a[i];
        toCFormSum(result, e->getNode(), e->isPositive() == positive);
      }
    }
    break;
  case PLUS:
    toCFormSum(toCFormSum(result, n->left(), positive), n->right(), positive);
    break;
  case MINUS:
    if(n->isUnaryMinus()) {
      toCFormSum(result, n->left() , !positive);
    } else {
      toCFormSum(toCFormSum(result, n->left(), positive), n->right(), !positive);
    }
    break;
  default:
    result.add(toCForm(n), positive);
    break;
  }
  return result;
}

SNode Expression::toCFormProduct(ExpressionNode *n) {
  FactorArray a;
  return getProduct(toCFormProduct(a, n, _1()));
}

FactorArray &Expression::toCFormProduct(FactorArray &result, ExpressionNode *n, SNode &exponent) {
  switch(n->getSymbol()) {
  case PRODUCT:
    { const FactorArray &a = n->getFactorArray();
      if(exponent.isOne()) {
        result.addAll(a);
      } else {
        for(size_t i = 0; i < a.size(); i++) toCFormProduct(result, a[i], exponent);
      }
    }
    break;
  case PROD:
    toCFormProduct(toCFormProduct(result, n->left() , exponent), n->right(), exponent);
    break;
  case QUOT:
    toCFormProduct(toCFormProduct(result, n->left() , exponent), n->right(), -exponent);
    break;
  case POW :
    toCFormPower(result, n, exponent);
    break;
  case SQR:
    toCFormProduct(result, sqr(toCForm(n->left())), exponent);
    break;
  case EXP:
    toCFormProduct(result, exp(toCForm(n->left())), exponent);
    break;
  case EXP10:
    toCFormProduct(result, exp10(toCForm(n->left())), exponent);
    break;
  case EXP2:
    toCFormProduct(result, exp2(toCForm(n->left())), exponent);
    break;
  case ROOT:
    toCFormRoot(result, n, exponent);
    break;
  case SQRT:
    toCFormProduct(result, sqrt(toCForm(n->left())), exponent);
    break;
  case NUMBER:
    result.add(n, exponent);
    break;
  default:
    result.add(toCForm(n), exponent);
    break;
  }
  return result;
}

// n.symbol == POW,
FactorArray &Expression::toCFormPower(FactorArray &result, ExpressionNode *n, SNode &exponent) {
  DEFINEMETHODNAME;
  SNode base = toCForm(n->left());
  SNode expo = toCForm(n->right());

  switch(base.getSymbol()) {
  case POW :
    result.add(base.left(), base.right() * exponent * expo);
    break;
  case ROOT:
    throwInvalidSymbolForTreeMode(method, n);
    break;
  case PRODUCT:
    { const FactorArray &factors = base.getFactorArray();
      SNode              newExpo = exponent * expo;
      for(size_t i = 0; i < factors.size(); i++) toCFormPower(result, factors[i], newExpo);
    }
    break;
  default                   :
    result.add(base, exponent * expo);
    break;
  }
  return result;
}

// n.symbol = ROOT
FactorArray &Expression::toCFormRoot(FactorArray &result, ExpressionNode *n, SNode &exponent) {
  DEFINEMETHODNAME;
  SNode rad  = toCForm(n->left());
  SNode root = toCForm(n->right());

  switch(rad.getSymbol()) {
  case POW :
    result.add(rad.left(), rad.right() * exponent / root);
    break;
  case ROOT:
    throwInvalidSymbolForTreeMode(method, n);
    break;
  case PRODUCT:
    { const FactorArray &factors = rad.getFactorArray();
      SNode              newExpo = exponent / root;
      for(size_t i = 0; i < factors.size(); i++) toCFormPower(result, factors[i], newExpo);
    }
    break;
  default                   :
    result.add(rad, exponent / root);
    break;
  }
  return result;
}

SNode Expression::toCFormPoly(ExpressionNode *n) {
  const ExpressionNodeArray &coefficientArray = n->getCoefficientArray();
  ExpressionNodeArray newCoefficientArray(coefficientArray.size());
  for(size_t i = 0; i < coefficientArray.size(); i++) {
    newCoefficientArray.add(toCForm(coefficientArray[i]));
  }
  ExpressionNode *newArgument = toCForm(n->getArgument());
  return getPoly(n, newCoefficientArray, newArgument);
}

SNode Expression::toCFormTreeNode(ExpressionNode *n) {
  const ExpressionNodeArray &a = n->getChildArray();
  ExpressionNodeArray newChildArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(toCForm(a[i]));
  }
  return getTree(n, newChildArray);
}

// -------------------------------------------------------------------------------------------------------

class CanonicalFormChecker : public ExpressionNodeHandler {
private:
  BitSet m_illegalSymbolSet;
  String m_error;
  bool   m_ok;
public:
  CanonicalFormChecker();
  bool handleNode(ExpressionNode *n, int level);
  bool isOk() const {
    return m_ok;
  }
  const String &getErrorMessage() const {
    return m_error;
  }
};

CanonicalFormChecker::CanonicalFormChecker() : m_illegalSymbolSet(ExpressionTables->getTerminalCount() + 1) {
  static const ExpressionInputSymbol table[] = {
    PLUS
   ,PROD
   ,QUOT
   ,ROOT
   ,SQRT
   ,SQR
   ,EXP
   ,SEC
   ,CSC
   ,COT
  };
  for(int i = 0; i < ARRAYSIZE(table); i++) {
    m_illegalSymbolSet.add(table[i]);
  }
  m_ok = true;
}

bool CanonicalFormChecker::handleNode(ExpressionNode *n, int level) {
  if(m_illegalSymbolSet.contains(n->getSymbol()) || n->isBinaryMinus()) {
    m_error = format(_T("Illegal symbol in canonical form:<%s>. node=<%s>"), n->getSymbolName().cstr(), n->toString().cstr());
    m_ok = false;
    return false;
  }
  return true;
}

void Expression::checkIsCanonicalForm() {
  CanonicalFormChecker checker;
  traverseTree(checker);
  if(!checker.isOk()) {
    throwException(checker.getErrorMessage());
  }
}

#endif
