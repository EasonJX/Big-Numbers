#pragma once

#include <MFCUtil/CoordinateSystem/AxisType.h>
#include "D3DXCube.h"
#include "D3SceneObject.h"

class D3Scene;
class D3SceneObjectLineArrow;

class D3SceneObjectCoordinateSystem : public D3SceneObject {
private:
  friend class D3CoordinateSystemFrameObject;
  D3CoordinateSystemFrameObject *m_frameObject;
  D3DXCube3                      m_cube;
  D3SceneObjectLineArrow        *m_axis[3];
  int                            m_axisMaterialId[3];
  D3SceneObjectCoordinateSystem(           const D3SceneObjectCoordinateSystem &src); // not implemented
  D3SceneObjectCoordinateSystem &operator=(const D3SceneObjectCoordinateSystem &src); // not implemented
public:
  D3SceneObjectCoordinateSystem(D3Scene &scene, const D3DXCube3 *cube=NULL);
  ~D3SceneObjectCoordinateSystem();
  void setRange(const D3DXCube3 &cube);
  inline const D3DXCube3 &getRange() const {
    return m_cube;
  }
  D3DXMATRIX &getWorld() {
    return *D3DXMatrixIdentity(&m_world);
  }
  LPD3DXMESH getMesh() const;
  void draw(D3Device &device);
};
