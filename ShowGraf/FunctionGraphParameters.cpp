#include "stdafx.h"
#include <XMLUtil.h>
#include "FunctionGraph.h"

FunctionGraphParameters::FunctionGraphParameters(const String &name, COLORREF color, int rollAvgSize, GraphStyle style, TrigonometricMode trigonomtetricMode)
: GraphParameters(name,color,rollAvgSize,style) {

  m_trigonometricMode = trigonomtetricMode;
  m_interval          = DoubleInterval(0,1);
  m_steps             = 500;
}

void FunctionGraphParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("Function"));
  doc.setValue(     root, _T("expr"       ), m_expr             );
  setValue(    doc, root, _T("interval"   ), m_interval         );
  doc.setValue(     root, _T("steps"      ), m_steps            );
  setValue(    doc, root                   , m_trigonometricMode);
  setStdValues(doc, root);
}

void FunctionGraphParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  checkTag(         root, _T("Function"));
  doc.getValueLF(   root, _T("expr"      ), m_expr             );
  getValue(    doc, root, _T("interval"  ), m_interval         );
  doc.getValue(     root, _T("steps"     ), m_steps            );
  getValue(    doc, root                  , m_trigonometricMode);
  getStdValues(doc, root);
}
