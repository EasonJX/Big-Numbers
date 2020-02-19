#include "pch.h"
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectSolidBox.h>

void D3SceneObjectSolidBox::makeSquareFace(MeshBuilder &mb, int v0, int v1, int v2, int v3) {
  Face &f = mb.addFace();
  const int nIndex = mb.addNormal(mb.calculateNormal(v0, v1, v2));
  f.addVertexNormalIndex(v0, nIndex);
  f.addVertexNormalIndex(v1, nIndex);
  f.addVertexNormalIndex(v2, nIndex);
  f.addVertexNormalIndex(v3, nIndex);
}

void D3SceneObjectSolidBox::init(const Vertex &p1, const Vertex &p2) {
  MeshBuilder mb;
  const D3DXVECTOR3 pmin = p1, pmax = p2;
  const int  lbn = mb.addVertex(pmin.x, pmin.y, pmin.z); // left  bottom near corner
  const int  lbf = mb.addVertex(pmin.x, pmin.y, pmax.z); // left  bottom far  corner
  const int  ltn = mb.addVertex(pmin.x, pmax.y, pmin.z); // left  top    near corner
  const int  ltf = mb.addVertex(pmin.x, pmax.y, pmax.z); // left  top    far  corner
  const int  rbn = mb.addVertex(pmax.x, pmin.y, pmin.z); // right bottom near corner
  const int  rbf = mb.addVertex(pmax.x, pmin.y, pmax.z); // right bottom far  corner
  const int  rtn = mb.addVertex(pmax.x, pmax.y, pmin.z); // right top    near corner
  const int  rtf = mb.addVertex(pmax.x, pmax.y, pmax.z); // right top    far  corner

  makeSquareFace(mb, lbn, lbf, rbf, rbn);                // bottom
  makeSquareFace(mb, ltn, rtn, rtf, ltf);                // top
  makeSquareFace(mb, lbn, ltn, ltf, lbf);                // left side
  makeSquareFace(mb, lbf, ltf, rtf, rbf);                // back side
  makeSquareFace(mb, rbf, rtf, rtn, rbn);                // right side
  makeSquareFace(mb, rbn, rtn, ltn, lbn);                // front side

  m_mesh = mb.createMesh(getScene(), false);
}
