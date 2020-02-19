#pragma once

#include <MyString.h>
#include "D3Math.h"
#include "Material.h"

class D3Scene;
class D3Device;
class D3Ray;
class D3PickedInfo;

typedef enum {
  SOTYPE_NULL
 ,SOTYPE_VISUALOBJECT
 ,SOTYPE_LIGHTCONTROL
 ,SOTYPE_ANIMATEDOBJECT
} SceneObjectType;

class D3SceneObject {
protected:
  D3Scene     &m_scene;
  String       m_name;
  bool         m_visible;
  void        *m_userData;
protected:
  D3DXMATRIX   m_world;
public:
  D3SceneObject(D3Scene &scene, const String &name=_T("Untitled"))
    : m_scene(scene)
    , m_name(name)
    , m_visible(true)
    , m_userData(NULL)
  {
    m_world = D3World();
  }
  virtual ~D3SceneObject() {
  }
  virtual SceneObjectType getType() const {
    return SOTYPE_VISUALOBJECT;
  }
  virtual void draw(D3Device &device) = 0;
  virtual LPD3DXMESH getMesh() const {
    return NULL;
  }
 
  virtual int getMaterialId() const {
    return 0;
  }
  virtual void modifyContextMenu(CMenu &menu) {
  }
  virtual String toString() const {
    return getName();
  }
  inline D3Scene &getScene() const {
    return m_scene;
  }
  inline void *getUserData() {
    return m_userData;
  }
  inline void setUserData(void *p) {
    m_userData = p;
  }
  inline void setVisible(bool visible) {
    m_visible = visible;
  }
  inline bool isVisible() const {
    return m_visible;
  }
  virtual bool hasFillMode() const {
    return false;
  }
  virtual void setFillMode(D3DFILLMODE fillMode) {
  }
  virtual D3DFILLMODE getFillMode() const {
    return D3DFILL_SOLID;
  }
  virtual bool hasShadeMode() const {
    return false;
  }
  virtual void setShadeMode(D3DSHADEMODE shadeMode) {
  }
  virtual D3DSHADEMODE getShadeMode() const {
    return D3DSHADE_GOURAUD;
  }
  const MATERIAL &getMaterial() const;
  inline bool hasMaterial() const {
    return getMaterialId() >= 0;
  }
  void resetWorld() {
    getWorld() = D3World();
  }
  virtual D3DXMATRIX &getWorld() {
    return m_world;
  }
  inline void setPos(const D3DXVECTOR3 &pos) {
    getWorld() = D3World(getWorld()).setPos(pos);
  }
  inline D3DXVECTOR3 getPos() const {
    return D3World(((D3SceneObject*)this)->getWorld()).getPos();
  }
  inline void setScale(const D3DXVECTOR3 &scale) {
    getWorld() = D3World(getWorld()).setScale(scale);
  }
  inline void setScaleAll(float scale) {
    setScale(D3DXVECTOR3(scale, scale, scale));
  }
  inline D3DXVECTOR3 getScale() const {
    return D3World(((D3SceneObject*)this)->getWorld()).getScale();
  }
  inline void setOrientation(const D3DXQUATERNION &q) {
    getWorld() = D3World(getWorld()).setOrientation(q);
  }
  inline D3DXQUATERNION getOrientation() const {
    return D3World(((D3SceneObject*)this)->getWorld()).getOrientation();
  }
  virtual bool intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info = NULL) const {
    return false;
  }

  inline const String &getName() const {
    return m_name;
  }
  inline void setName(const String &name) {
    m_name = name;
  }
};
