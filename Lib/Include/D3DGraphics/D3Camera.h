#pragma once

#include <PropertyContainer.h>
#include <Date.h>
#include <MFCUtil/ColorSpace.h>
#include "D3World.h"
#include "D3Ray.h"
#include "D3SceneObject.h"

class D3Device;
class D3Scene;
class D3SceneObjectVisual;
class D3PickedInfo;

typedef enum {
  CAM_VIEW                      // D3DXMATRIX
 ,CAM_PROJECTION                // D3DXMATRIX
 ,CAM_BACKGROUNDCOLOR           // D3DCOLOR
} D3CameraProperty;

class D3Camera : public D3SceneObject, public PropertyContainer {
private:
  HWND                    m_hwnd;
  bool                    m_rightHanded;
  float                   m_viewAngel;
  float                   m_nearViewPlane;
  D3World                 m_world;
  D3DXMATRIX              m_projMatrix, m_viewMatrix;
  D3DCOLOR                m_backgroundColor;

  // notify listeners with properyId=CAM_PROJECTION
  void           setProjMatrix();
  D3DXMATRIX    &createProjMatrix(D3DXMATRIX &m) const;
  // notify listeners with properyId=CAM_VIEW
  void           setViewMatrix();
  D3DXMATRIX    &createViewMatrix(D3DXMATRIX &m) const;
  D3Camera(           const D3Camera &src); // not implemented
  D3Camera &operator=(const D3Camera &src); // not implemented
public:
  D3Camera(D3Scene &scene, HWND hwnd);
  virtual ~D3Camera();
  SceneObjectType getType() const {
    return SOTYPE_CAMERA;
  }
  void OnSize();

  inline HWND getHwnd() const {
    return m_hwnd;
  }
  // p in screen-coordinates
  inline CPoint screenToWin(CPoint p) const {
    ScreenToClient(getHwnd(), &p);
    return p;
  }

  // p in screen coordinate
  bool ptInRect(CPoint p) const;
  inline CSize getWinSize() const {
    return getClientRect(getHwnd()).Size();
  }
  inline bool isVisible() const {
    return IsWindowVisible(getHwnd());
  }
  // set property m_backgroundColor, and notifies properyChangeListerners. (property-id=DEV_BACKGROUNDCOLOR)
  inline D3Camera &setBackgroundColor(D3DCOLOR color) {
    setProperty(CAM_BACKGROUNDCOLOR, m_backgroundColor, color);
    return *this;
  }
  inline D3PCOLOR getBackgroundColor() const {
    return m_backgroundColor;
  }

  D3Camera &setViewAngel(float angel);
  inline float   getViewAngel() const {
    return m_viewAngel;
  }
  D3Camera &setNearViewPlane(float zn);
  inline float getNearViewPlane() const {
    return m_nearViewPlane;
  }

  D3Camera &setRightHanded(bool rightHanded);
  inline bool getRightHanded() const {
    return m_rightHanded;
  }
  const D3World &getWorld() const {
    return m_world;
  }
  // notify listeners with properyId=CAM_VIEW
  D3Camera &setWorld(const D3World &world);
  inline const D3DXVECTOR3 getPos() const {
    return m_world.getPos();
  }
  inline const D3DXVECTOR3 getDir() const {
    return m_world.getDir();
  }
  inline const D3DXVECTOR3 getUp() const {
    return m_world.getUp();
  }
  inline D3DXVECTOR3 getRight() const {
    return m_world.getRight();
  }
  D3Camera &resetPos();
  D3Camera &setPos(        const D3DXVECTOR3     &pos);
  D3Camera &setOrientation(D3DXQUATERNION        &q  );
  D3Camera &setLookAt(     const D3DXVECTOR3     &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up);
  D3Camera &setLookAt(     const D3DXVECTOR3     &point);

  inline const D3DXMATRIX &getProjMatrix() const {
    return m_projMatrix;
  }
  inline const D3DXMATRIX &getViewMatrix() const {
    return m_viewMatrix;
  }
  // point in window-coordinates (m_hwnd)
  // Return Ray in world-space
  D3Ray getPickedRay(const CPoint &point) const;
  // Point in window-coordinates (m_hwnd)
  D3SceneObjectVisual *getPickedVisual(const CPoint &p, long mask = -1, D3DXVECTOR3 *hitPoint = NULL, D3Ray *ray = NULL, float *dist = NULL, D3PickedInfo *info = NULL) const;
  // call m_scene.render(*this)
  void render();
  String toString() const;
};
