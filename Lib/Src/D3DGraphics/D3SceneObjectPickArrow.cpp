#include "pch.h"
#include <D3DGraphics/D3Ray.h>
#include <D3DGraphics/D3SceneObjectPickArrow.h>

D3PickRayArrow::D3PickRayArrow(D3Scene &scene, const D3Ray &ray) : D3SceneObjectLineArrow(scene, ray.m_orig, ray.m_orig + 2 * ray.m_dir) {
  D3DXMatrixIdentity(&m_world);
}
