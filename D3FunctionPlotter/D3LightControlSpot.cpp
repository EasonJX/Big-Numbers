#include "stdafx.h"
#include "D3LightControl.h"

D3LightControlSpot::D3LightControlSpot(D3Scene &scene, int lightIndex) 
: D3LightControl(scene, lightIndex)
{
  setSize(0.4f);
  m_mesh = createSpotMesh();
  setName(format(_T("Spot light (%d)"), lightIndex));
}

static const Point2D spotMeshProfilePoints[] = {
  Point2D(1.0 ,   0 )
 ,Point2D(1.0 , 0.1 )
 ,Point2D(0.3 , 0.1 )
 ,Point2D(0.25, 0.13)
 ,Point2D(0.0 , 0.17)
 ,Point2D(0.00, 0.00)
};

LPD3DXMESH D3LightControlSpot::createSpotMesh() {
  Profile prof;
  prof.addLineStrip(spotMeshProfilePoints, ARRAYSIZE(spotMeshProfilePoints));

  ProfileRotationParameters param;
  param.m_alignx     = 2;
  param.m_aligny     = 1;
  param.m_rad        = radians(360);
  param.m_edgeCount  = 20;
  param.m_smoothness = ROTATESMOOTH | NORMALSMOOTH;
  param.m_rotateAxis = 2;
  LPD3DXMESH mesh = rotateProfile(getDevice(), prof, param, false);

  return optimizeMesh(mesh);
}

D3DXMATRIX D3LightControlSpot::getWorldMatrix() const {
  LIGHT param = getLightParam();
  return createWorldMatrix(param.Position, &param.Direction);
}
