#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

DEFINECLASSNAME(ParserTree);

using namespace std;

ParserTree::ParserTree() {
  resetSimpleConstants();
  m_indexNameCounter = 0;
  m_root             = NULL;
  m_treeForm         = TREEFORM_STANDARD;
  initDynamicOperations(m_treeForm);
  m_ok               = false;
}

ParserTree::ParserTree(const ParserTree &src) {
  resetSimpleConstants();
  m_errors           = src.m_errors;
  m_indexNameCounter = 0;
  m_root             = src.m_root ? src.m_root->clone(this) : NULL;
  m_treeForm         = src.m_treeForm;
  initDynamicOperations(m_treeForm);
  m_ok               = src.m_ok;
}

ParserTree &ParserTree::operator=(const ParserTree &rhs) {
  if(&rhs == this) {
    return *this;
  }
  releaseAll();
  m_errors           = rhs.m_errors;
  m_indexNameCounter = 0;
  setRoot(rhs.m_root ? rhs.m_root->clone(this) : NULL);
  setTreeForm(rhs.getTreeForm());
  setOk(rhs.m_ok);
  return *this;
}

ParserTree::~ParserTree() {
  releaseAll();
}

void ParserTree::setRoot(ExpressionNode *n) {
  const ExpressionNode *oldRoot = m_root;
  if(n != oldRoot) {
    m_root = n;
    notifyPropertyChanged(EXPR_ROOT, oldRoot, m_root);
  }
}

void ParserTree::setOk(bool ok) {
  const bool oldOk = m_ok;
  if(ok != m_ok) {
    m_ok = ok;
    notifyPropertyChanged(EXPR_OK, &oldOk, &m_ok);
  }
}

void ParserTree::setTreeForm(ParserTreeForm form) {
  const ParserTreeForm oldForm = m_treeForm;
  if(form != oldForm) {
    m_treeForm = form;
    initDynamicOperations(m_treeForm);
    notifyPropertyChanged(EXPR_TREEFORM, &oldForm, &m_treeForm);
  }
}


String ParserTree::getTreeFormName(ParserTreeForm treeForm) { // static
  switch(treeForm) {
  case TREEFORM_STANDARD : return _T("standard");
  case TREEFORM_CANONICAL: return _T("canonical");
  case TREEFORM_NUMERIC  : return _T("numeric");
  default                : return format(_T("Unknown treeForm:%d"), treeForm);
  }
}

void ParserTree::initDynamicOperations(ParserTreeForm treeForm) {
  DEFINEMETHODNAME;
  switch(treeForm) {
  case TREEFORM_STANDARD :
    pminus      = &ParserTree::minusS;
    preciprocal = &ParserTree::reciprocalS;
    psqr        = &ParserTree::sqrS;
    psqrt       = &ParserTree::sqrtS;
    psum        = &ParserTree::sumS;
    pdiff       = &ParserTree::differenceS;
    pprod       = &ParserTree::productS;
    pquot       = &ParserTree::quotientS;
    pmod        = &ParserTree::modulusS;
    ppower      = &ParserTree::powerS;
    proot       = &ParserTree::rootS;
    pexp        = &ParserTree::expS;
    pcot        = &ParserTree::cotS;
    pcsc        = &ParserTree::cscS;
    psec        = &ParserTree::secS;
    break;

  case TREEFORM_CANONICAL:
    pminus      = &ParserTree::minusC;
    preciprocal = &ParserTree::reciprocalC;
    psqr        = &ParserTree::sqrC;
    psqrt       = &ParserTree::sqrtC;
    psum        = &ParserTree::sumC;
    pdiff       = &ParserTree::differenceC;
    pprod       = &ParserTree::productC;
    pquot       = &ParserTree::quotientC;
    pmod        = &ParserTree::modulusC;
    ppower      = &ParserTree::powerC;
    proot       = &ParserTree::rootC;
    pexp        = &ParserTree::expC;
    pcot        = &ParserTree::cotC;
    pcsc        = &ParserTree::cscC;
    psec        = &ParserTree::secC;
    break;

  case TREEFORM_NUMERIC:
    break;

  default                :
    throwMethodInvalidArgumentException(s_className, method, _T("Unknown treeForm:%d"), treeForm);
  }
}

void ParserTree::releaseAll() {
  for(size_t i = 0; i < m_nodeTable.size(); i++) {
    delete m_nodeTable[i];
  }
  for(size_t i = 0; i < m_addentTable.size(); i++) {
    delete m_addentTable[i];
  }
  resetSimpleConstants();
  setRoot(NULL);
  setOk(false);
  m_errors.clear();
  m_nodeTable.clear();
  m_addentTable.clear();
  clearSymbolTable();
}

void ParserTree::addError(ExpressionNode *n, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  if((n == NULL) || !n->hasPos()) {
    vAddError(NULL, format, argptr);
  } else {
    vAddError(&n->getPos(), format, argptr);
  }
}

void ParserTree::addError(const SourcePosition &pos, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  vAddError(&pos, format, argptr);
  va_end(argptr);
}

void ParserTree::addError(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  vAddError(NULL, format, argptr);
  va_end(argptr);
}

void ParserTree::vAddError(const SourcePosition *pos, const TCHAR *format, va_list argptr) {
  String tmp2;
  String tmp = vformat(format, argptr);
  if(pos != NULL) {
    tmp2 = ::format(_T("%s:%s"), pos->toString().cstr(), tmp.cstr());
  } else {
    tmp2 = ::format(_T("%s"), tmp.cstr());
  }
  tmp2.replace('\n',' ');
  m_errors.add(tmp2);
  setOk(false);
}

int ParserTree::decodeErrorString(const String &expr, String &error) { // static
  Tokenizer tok(error, ":");
  String posStr = tok.next();
  int line, col;
  String tmp;
  if(_stscanf(posStr.cstr(), _T("(%d,%d)"), &line, &col) == 2) {
    error = tok.getRemaining();
  } else {
    throwException(_T("No sourceposition"));
  }
  return SourcePosition::findCharIndex(expr.cstr(), SourcePosition(line, col));
}

void ParserTree::listErrors(FILE *f) const {
  for(size_t i = 0; i < m_errors.size();i++) {
    _ftprintf(f,_T("%s\n"), m_errors[i].cstr());
  }
  fflush(f);
}

void ParserTree::listErrors(tostream &out) const {
  for(size_t i = 0; i < m_errors.size();i++) {
    out << m_errors[i] << endl;
  }
  out.flush();
}

void ParserTree::listErrors(const TCHAR *fname) const {
  FILE *f = fopen(fname,_T("w"));
  if(f == NULL) {
    _ftprintf(stdout,_T("Cannot open %s\n"), fname);
    listErrors(stdout);
  } else {
    listErrors(f);
    fclose(f);
  }
}

static void getListFromTree(ExpressionNode *n, int delimiterSymbol, ExpressionNodeArray &list) {
  if(n->getSymbol() == delimiterSymbol) {
    getListFromTree(n->left(),delimiterSymbol,list);
    list.add(n->right());
  } else {
    list.add(n);
  }
}

ExpressionNodeArray getExpressionList(ExpressionNode *n) {
  ExpressionNodeArray result(10);
  getListFromTree(n, COMMA, result);
  return result;
}

ExpressionNodeArray getStatementList(ExpressionNode *n) {
  ExpressionNodeArray result(10);
  getListFromTree(n, SEMI, result);
  return result;
}

int ParserTree::getNodeCount(ExpressionNodeSelector *selector) {
  if(getRoot() == NULL) {
    return 0;
  } else {
    if(selector) {
      unmarkAll();
      markPow1Nodes();
    }
    return getRoot()->getNodeCount(selector);
  }
}

int ParserTree::getNodeCount(bool ignoreMarked, ExpressionInputSymbol s1,...) { // terminate symbolset with 0. Only specified symbols will be counted
  ExpressionSymbolSet legalSymbols;
  bool                setIsEmpty = true;
  if(s1) {
    legalSymbols.add(s1);
    va_list argptr;
    va_start(argptr,s1);
    for(ExpressionInputSymbol s = va_arg(argptr, ExpressionInputSymbol); s; s = va_arg(argptr, ExpressionInputSymbol)) {
      legalSymbols.add(s);
    }
    va_end(argptr);
    setIsEmpty = false;
  }

  ExpressionNodeSymbolSelector selector(setIsEmpty ? NULL : &legalSymbols, ignoreMarked);

  if(setIsEmpty && !ignoreMarked) {
    return getNodeCount();
  } else {
    return getNodeCount(&selector);
  }
}

class Pow1NodeMarker : public ExpressionNodeHandler {
public:
  bool handleNode(ExpressionNode *n, int level);
};

bool Pow1NodeMarker::handleNode(ExpressionNode *n, int level) {
  if((n->getSymbol() == POW) && (n->right()->isOne())) {
    n->mark();
    n->right()->mark();
  }
  return true;
}

void ParserTree::markPow1Nodes() {
  traverseTree(Pow1NodeMarker());
}

int ParserTree::getTreeDepth() const {
  return getRoot() ? getRoot()->getMaxTreeDepth() : 0;
}

ExpressionNode *ParserTree::traverseSubstituteNodes(ExpressionNode *n, CompactNodeHashMap<ExpressionNode*> &nodeMap) {
  DEFINEMETHODNAME;
  ExpressionNode **n1 = nodeMap.get(n);
  if(n1) {
    return *n1;
  }
  // not found;
  switch(n->getNodeType()) {
  case EXPRESSIONNODENUMBER    :
  case EXPRESSIONNODEBOOLEAN   :
  case EXPRESSIONNODEVARIABLE  : return n;
  case EXPRESSIONNODEFACTOR    :
    { ExpressionFactor *oldFactor = (ExpressionFactor*)n;
      ExpressionNode   *newBase   = traverseSubstituteNodes(oldFactor->base()    , nodeMap);
      ExpressionNode   *newExpo   = traverseSubstituteNodes(oldFactor->exponent(), nodeMap);
      return getFactor(oldFactor, newBase, newExpo);
    }
  case EXPRESSIONNODETREE      :
    { const ExpressionNodeArray  &a = n->getChildArray();
      ExpressionNodeArray         newChildArray(a.size());
      for(size_t i = 0; i < a.size(); i++) newChildArray.add(traverseSubstituteNodes(a[i], nodeMap));
      return getTree(n, newChildArray);
    }
  case EXPRESSIONNODESUM       :
    { const AddentArray &a = n->getAddentArray();
      AddentArray newAddentArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        SumElement     *e       = a[i];
        ExpressionNode *oldNode = e->getNode();
        ExpressionNode *newNode = traverseSubstituteNodes(oldNode, nodeMap);
        if(newNode == oldNode) {
          newAddentArray.add(e);
        } else {
          newAddentArray.add(newNode, e->isPositive());
        }
      }
      return getSum(n, newAddentArray);
    }
  case EXPRESSIONNODEPRODUCT   :
    { const FactorArray           &a = n->getFactorArray();
      FactorArray                  newFactorArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        ExpressionNode *newNode = traverseSubstituteNodes(a[i], nodeMap);
        if(newNode->getNodeType() == EXPRESSIONNODEFACTOR) {
          newFactorArray.add((ExpressionFactor*)newNode);
        } else {
          newFactorArray.add(newNode);
        }
      }
      return getProduct(n, newFactorArray);
    }
  case EXPRESSIONNODEPOLYNOMIAL:
    { const ExpressionNodeArray  &coef = n->getCoefficientArray();
      ExpressionNode             *arg  = n->getArgument();
      ExpressionNodeArray         newCoef(coef.size());
      ExpressionNode             *newArg  = traverseSubstituteNodes(arg, nodeMap);
      for(size_t i = 0; i < coef.size(); i++) newCoef.add(traverseSubstituteNodes(coef[i], nodeMap));
      return getPoly(n, newCoef, newArg);
    }
  default:
    throwMethodInvalidArgumentException(s_className, method, _T("Unknown nodeType:%d"), n->getNodeType());
    return NULL;
  }
}

void ParserTree::substituteNodes(CompactNodeHashMap<ExpressionNode*> &nodeMap) {
  ExpressionNode *root = getRoot();
  if(root) {
    setRoot(traverseSubstituteNodes(root, nodeMap));
  }
}

void ParserTree::traverseTree(ExpressionNodeHandler &handler) {
  if(m_root) {
    m_root->traverseExpression(handler, 0);
  }
}


// ---------------------------------------- only used by parser --------------------------------------------------

ExpressionNode *ParserTree::vFetchNode(const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr) {
  switch(symbol) {
  case NUMBER: return new ExpressionNodeNumberWithPos(   this, pos, va_arg(argptr,Real  ));
  case NAME  : return new ExpressionNodeVariableWithPos( this, pos, va_arg(argptr,TCHAR*));
  case POLY  : return new ExpressionNodePolyWithPos(     this, pos, argptr);
  default    : return new ExpressionNodeTreeWithPos(     this, pos, symbol, argptr);
  }
}

// Used by derivate, reduce, graphics
ExpressionNodeVariable *ParserTree::fetchVariableNode(const String &name) {
  return new ExpressionNodeVariable(this, name);
}

ExpressionNode *ParserTree::constExpression(const String &name) {
  DEFINEMETHODNAME;

  const ExpressionVariable *v = getVariable(name);
  if(v == NULL) {
    throwMethodInvalidArgumentException(s_className, method, _T("%s not found in symbol table"), name.cstr());
  } else if(!v->isConstant()) {
    throwMethodInvalidArgumentException(s_className, method, _T("%s is not a constant"), name.cstr());
  }
  return new ExpressionNodeVariable(this, name, *(ExpressionVariable*)v);
}

String ParserTree::treeToString() const {
  ((ParserTree*)this)->unmarkAll();
  String result;
  if(m_root) m_root->dumpNode(result, 0);
  result += variablesToString();
  return result;
}

String ParserTree::variablesToString() const {
  String result = _T("SymbolTable:\n");
  if(m_variableTable.size() > 0) {
    for(size_t i = 0; i < m_variableTable.size(); i++) {
      const ExpressionVariable &var = m_variableTable[i];
      result += format(_T("  %2d: %s\n"), (int)i, var.toString().cstr());
    }
  }
  result += _T("ValueTable:\n");
  for (size_t i = 0; i < m_valueTable.size(); i++) {
    result += format(_T("  %2d: %s\n"), (int)i, ::toString(m_valueTable[i]).cstr());
  }
  return result;
}

#ifdef TRACE_REDUCTION_CALLSTACK

void ParserTree::pushReductionMethod(const TCHAR *method, const String &s, const ExpressionNode *n) const {
  const int oldHeight = m_reductionStack.getHeight();
  m_reductionStack.push(ReductionStackElement(method, s,n));
  const int newHeight = m_reductionStack.getHeight();
  notifyPropertyChanged(REDUCTION_STACKHIGHT, &oldHeight, &newHeight);
}

void ParserTree::popReductionMethod(const TCHAR *method) const {
  const int oldHeight = m_reductionStack.getHeight();
  m_reductionStack.pop();
  const int newHeight = m_reductionStack.getHeight();
  notifyPropertyChanged(REDUCTION_STACKHIGHT, &oldHeight, &newHeight);
}

void ParserTree::resetReductionStack() {
  const int oldHeight = m_reductionStack.getHeight();
  m_reductionStack.clear();
  const int newHeight = m_reductionStack.getHeight();
  if(newHeight != oldHeight) {
    notifyPropertyChanged(REDUCTION_STACKHIGHT, &oldHeight, &newHeight);
  }
}

void ParserTree::clearAllBreakPoints() {
  for (size_t i = 0; i < m_nodeTable.size(); i++) {
    m_nodeTable[i]->clearBreakPoint();
  }
}

#endif
