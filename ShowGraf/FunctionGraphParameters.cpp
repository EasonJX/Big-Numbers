#include "stdafx.h"
#include <XMLUtil.h>
#include "FunctionGraph.h"

FunctionGraphParameters::FunctionGraphParameters(const String &name, COLORREF color, const RollingAvg &rollingAvg, GraphStyle style, TrigonometricMode trigonometricMode)
: ExprGraphParameters(name,color,rollingAvg,style,trigonometricMode) {
  m_interval          = DoubleInterval(0,1);
  m_steps             = 500;
}

void FunctionGraphParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr    root = doc.createRoot(_T("Function"));
  setValue(doc, root, _T("expr"       ), m_expr             );
  setValue(doc, root, _T("interval"   ), m_interval         );
  setValue(doc, root, _T("steps"      ), m_steps            );
  __super::putDataToDoc(doc);
}

void FunctionGraphParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr        root = doc.getRoot();
  XMLDoc::checkTag( root, _T("Function"));
  getValueLF(  doc, root, _T("expr"      ), m_expr          );
  getValue(    doc, root, _T("interval"  ), m_interval      );
  getValue(    doc, root, _T("steps"     ), m_steps         );
  __super::getDataFromDoc(doc);
}
