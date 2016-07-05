#pragma once

#include <Math/MathLib.h>
#include <Math/Rectangle2D.h>
#include <Math/Expression/Expression.h>
#include "GraphParameters.h"

class IsoCurveGraphParameters : public GraphParameters {
public:
  String            m_expr;
  Rectangle2D       m_boundingBox;
  double            m_cellSize;
  TrigonometricMode m_trigonometricMode;
  IsoCurveGraphParameters(const String &name=_T("Untitled"), COLORREF color=BLACK, GraphStyle style=GSCURVE, TrigonometricMode trigonomtetricMode=RADIANS);
  void read(const String &fileName);
  void write(FILE *f);
  void read( FILE *f);
};
