#pragma once

#include "D3SceneObjectVisual.h"
#include "D3DXCube.h"

class D3SceneObjectWithMesh : public D3SceneObjectVisual {
private:
  D3DFILLMODE  m_fillMode;
  D3DSHADEMODE m_shadeMode;
protected:
  LPD3DXMESH m_mesh;
  void createMesh(DWORD faceCount, DWORD vertexCount, DWORD fvf);
  void *lockVertexBuffer();
  void *lockIndexBuffer();
  void unlockVertexBuffer();
  void unlockIndexBuffer();
  void releaseMesh();
  inline void drawSubset(DWORD attribId) {
    FV(m_mesh->DrawSubset(attribId));
  }
public:
  // if mesh != NULL, it will be released when Object is destroyed
  D3SceneObjectWithMesh(D3Scene &scene, LPD3DXMESH mesh = NULL);
  ~D3SceneObjectWithMesh();
  LPD3DXMESH getMesh() const {
    return m_mesh;
  }
  inline bool hasMesh() const {
    return m_mesh != NULL;
  }

  virtual bool hasFillMode() const {
    return true;
  }
  void setFillMode(D3DFILLMODE fillMode) {
    m_fillMode = fillMode;
  }
  D3DFILLMODE getFillMode() const {
    return m_fillMode;
  }
  virtual bool hasShadeMode() const {
    return true;
  }
  void setShadeMode(D3DSHADEMODE shadeMode) {
    m_shadeMode = shadeMode;
  }
  D3DSHADEMODE getShadeMode() const {
    return m_shadeMode;
  }
  D3DXCube3 getBoundingBox() const;
  void draw();
  String toString() const;
};
