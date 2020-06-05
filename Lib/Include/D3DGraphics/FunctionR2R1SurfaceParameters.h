#pragma once

#include <MFCUtil/AnimationParameters.h>
#include "D3SurfaceVertexParameters.h"

class FunctionR2R1SurfaceParameters {
public:
  DoubleInterval            m_xInterval;
  DoubleInterval            m_yInterval;
  UINT                      m_pointCount;
  bool                      m_doubleSided;
  D3SurfaceVertexParameters m_vertexParameters;
  AnimationParameters       m_animation;
  FunctionR2R1SurfaceParameters();
  const DoubleInterval &getXInterval() const {
    return m_xInterval;
  }
  const DoubleInterval &getYInterval() const {
    return m_yInterval;
  }
  inline bool isAnimated() const {
    return m_animation.includeTime();
  }
};
