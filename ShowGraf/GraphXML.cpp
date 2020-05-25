#include "StdAfx.h"
#include "RollingAvg.h"
#include "GraphXML.h"

void setValue(XMLDoc &doc, XMLNodePtr n, GraphStyle style) {
  String str = toLowerCase(GraphParameters::graphStyleToStr(style));
  doc.setValue(n, _T("style"), str);
}

void getValue(XMLDoc &doc, XMLNodePtr n, GraphStyle &style) {
  String str;
  doc.getValue(n, _T("style"), str);
  style = GraphParameters::graphStyleFromString(str);
}

static const TCHAR *styleName[] = {
  _T("Curve")
 ,_T("Point")
 ,_T("Cross")
};

const TCHAR *GraphParameters::graphStyleToStr(GraphStyle style) {  // static
  if(style < 0 || style > 2) {
    throwInvalidArgumentException(__TFUNCTION__, _T("style=%d. Must be [0..2]"), style);
  }
  return styleName[style];
}

GraphStyle GraphParameters::graphStyleFromString(const String &s) {  // static
  for(int i = 0; i < ARRAYSIZE(styleName); i++) {
    if(s.equalsIgnoreCase(styleName[i])) {
      return (GraphStyle)i;
    }
  }
  return GSCURVE;
}

void setValue(XMLDoc &doc, XMLNodePtr n, const RollingAvg &rollingAvg) {
  doc.setValue(n, _T("enable"   ), rollingAvg.isEnabled());
  doc.setValue(n, _T("queuesize"), rollingAvg.getQueueSize());
}

void getValue(XMLDoc &doc, XMLNodePtr n, RollingAvg &rollingAvg) {
  bool enabled;
  UINT queueSize;
  doc.getValue(n, _T("enable"   ), enabled);
  doc.getValue(n, _T("queuesize"), queueSize);
  rollingAvg = RollingAvg(enabled, queueSize);
}
