#pragma once

#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/ExpressionGraphics.h>
#include "Graph.h"
#include "FunctionGraphParameters.h"

class FunctionGraph : public PointGraph {
private:
//  ExpressionImage m_image;
public:
  FunctionGraph(const FunctionGraphParameters &param);
  void calculate();
  void paint(CCoordinateSystem &cs);
  void setTrigoMode(TrigonometricMode mode);
  inline GraphType getType() const {
    return FUNCTIONGRAPH;
  }
  GraphZeroesResultArray  findZeroes( const DoubleInterval &interval) const;
  GraphExtremaResultArray findExtrema(const DoubleInterval &interval, ExtremaType extremaType) const;
};
