#pragma once

#include <MFCUtil/Viewport2D.h>
#include <Math/MathLib.h>
#include "GraphParameters.h"

class FunctionGraphParameters : public GraphParameters {
public:
  String            m_expr;
  DoubleInterval    m_interval;
  UINT              m_steps;
  TrigonometricMode m_trigonometricMode;
  FunctionGraphParameters(const String &name=_T("Untitled"), COLORREF color=BLACK, int rollSize=0, GraphStyle style=GSCURVE, TrigonometricMode trigonomtetricMode=RADIANS);
  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  int getType() const {
    return FUNCTIONGRAPH;
  }
};

class FunctionPlotter {
public:
  virtual void plotFunction(Function &f, COLORREF color) = 0;
  virtual void plotFunction(Function &f, const DoubleInterval &interval, COLORREF color) = 0;
  virtual void addFunctionGraph(FunctionGraphParameters &param)      = 0;
};

