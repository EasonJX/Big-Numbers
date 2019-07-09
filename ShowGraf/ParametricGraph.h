#pragma once

#include <MFCUtil/ExpressionGraphics.h>
#include "PointGraph.h"
#include "ParametricGraphParameters.h"

class ParametricGraph : public PointGraph {
private:
//  ExpressionImage m_image;
public:
  ParametricGraph(CCoordinateSystem &system, const ParametricGraphParameters &param);
  void calculate();
  void paint(CDC &dc);
  void setTrigoMode(TrigonometricMode mode);
  inline GraphType getType() const {
    return PARAMETRICGRAPH;
  }
  GraphZeroesResultArray findZeroes(const DoubleInterval &i) {
    return GraphZeroesResultArray(*this);
  }
};
