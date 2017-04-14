#pragma once

#include <CompactArray.h>
#include <Thread.h>
#include <BitSet.h>
#include <Date.h>
#include <PropertyContainer.h>
#include <NumberInterval.h>
#include <MFCUtil/D3DeviceFactory.h>
#include "D3TextureFactory.h"
#include "MeshBuilder.h"
#include "MeshArray.h"
#include "D3Math.h"

class D3LightControl;
class D3SceneObject;

typedef enum {
  SP_BACKGROUNDCOLOR
 ,SP_AMBIENTLIGHT
 ,SP_CAMERAPDUS
 ,SP_PROJECTIONTRANSFORMATION
 ,SP_LIGHTPARAMETERS
 ,SP_LIGHTCOUNT
 ,SP_MATERIALPARAMETERS
 ,SP_MATERIALCOUNT
 ,SP_RENDERTIME
 ,SP_OBJECTCOUNT
 ,SP_ANIMATIONFRAMEINDEX
} D3SceneProperty;

#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }

inline bool operator==(const D3DMATERIAL &m1, const D3DMATERIAL &m2) {
  return memcmp(&m1, &m2, sizeof(D3DMATERIAL)) == 0;
}

inline bool operator!=(const D3DMATERIAL &m1, const D3DMATERIAL &m2) {
  return !(m1 == m2);
}

class MATERIAL : public D3DMATERIAL { // sent to listener for id=SP_MATERIALPARAMETERS
public:
  int m_index;   // if < 0, material is undefined.
  inline MATERIAL() : m_index(-1) {
  }
  inline bool isDefined() const {
    return m_index >= 0;
  }
};

class LIGHT : public D3DLIGHT9 { // sent to listener for id=SP_LIGHTPARAMETERS
public:
  int   m_index;
  bool  m_enabled;

  // only valid for spot lights
  void setInnerAngle(float rad); // maintain invariant:0 <= Theta <= Phi <= pi
  void setOuterAngle(float rad);
  inline float getInnerAngle() const {
    return Theta;
  }
  inline float getOuterAngle() const {
    return Phi;
  }
  inline bool isEnabled() const {
    return m_enabled;
  }
  void setDefaultColors();
};

inline bool operator==(const LIGHT &l1, const LIGHT &l2) {
  return memcmp(&l1, &l2, sizeof(LIGHT)) == 0;
}

inline bool operator!=(const LIGHT &l1, const LIGHT &l2) {
  return !(l1 == l2);
}

class D3PCOLOR {
public:
  const D3DCOLOR m_color;
  inline D3PCOLOR(D3DCOLOR c) : m_color(c) {}
  inline operator D3DCOLOR() const { return m_color; }
};

typedef enum {
  SOTYPE_VISUALOBJECT
 ,SOTYPE_LIGHTCONTROL
 ,SOTYPE_ANIMATEDOBJECT
} SceneObjectType;

#define PICK_MASK(type) (1 << (type))

#define PICK_VISUALOBJECT   PICK_MASK(SOTYPE_VISUALOBJECT  )
#define PICK_LIGHTCONTROL   PICK_MASK(SOTYPE_LIGHTCONTROL  )
#define PICK_ANIMATEDOBJECT PICK_MASK(SOTYPE_ANIMATEDOBJECT)
#define PICK_ALL            (-1)

class D3PickedInfo {
public:
  int           m_faceIndex;
  int           m_i1, m_i2, m_i3;           // Indices into vertexArray
  TextureVertex m_tv;
  inline D3PickedInfo() : m_faceIndex(-1) {
  }
  inline void clear() {
    m_faceIndex = -1;
  }
  inline bool isSet() const {
    return m_faceIndex >= 0;
  }
  inline String toString(int dec=3) const {
    return isSet() 
         ? format(_T("Face:%d (%5d,%5d,%5d) %s")
                 ,m_faceIndex
                 ,m_i1, m_i2, m_i3
                 ,::toString(m_tv, dec).cstr())
         : EMPTYSTRING;
  }
};

String toString(const LIGHT                &light    );
String toString(const MATERIAL             &material );
String toString(      D3DFILLMODE           mode     );
String toString(      D3DSHADEMODE          mode     );
String toString(      D3DFORMAT             f        );
int    formatToSize(  D3DFORMAT             f        );
String toString(      D3DPOOL               pool     );
String toString(      D3DRESOURCETYPE       type     );
String usageToString( DWORD                 usage    );
String FVFToString(   DWORD                 fvf      );
int    FVFToSize(     DWORD                 fvf      );
String toString(const D3DDISPLAYMODE       &mode     );
String toString(      D3PCOLOR              c        ,bool showAlpha=false);
String toString(const D3DCOLORVALUE        &c        ,bool showAlpha=false);
String toString(const D3DVERTEXBUFFER_DESC &desc     );
String toString(const D3DINDEXBUFFER_DESC  &desc     );

String vertexToString(const char *v, DWORD FVF  , int dec=3);

class D3SceneRenderState {
public:
  D3DFILLMODE  m_fillMode;
  D3DSHADEMODE m_shadeMode;
  D3DCOLOR     m_backgroundColor;
  D3DCOLOR     m_ambientColor;
  DWORD        m_fvf;
  bool         m_zEnable                 : 1;
  bool         m_normalizeNormals        : 1;
  bool         m_alphaBlendEnable        : 1;
  bool         m_lighting                : 1;
  bool         m_specularHighLightEnable : 1;
  int          m_selectedMaterialIndex;

  D3SceneRenderState() {
    setDefault();
  }
  void getValuesFromDevice(LPDIRECT3DDEVICE device);
  void setValuesToDevice(  LPDIRECT3DDEVICE device);
  void setDefault();
};

class D3Scene : public PropertyContainer
              , public AbstractMeshFactory
              , public AbstractTextureFactory
{
private:
  static LIGHT getDefaultDirectionalLight();
  static LIGHT getDefaultPointLight();
  static LIGHT getDefaultSpotLight();
  static const D3PosDirUpScale s_pdusOrigo;

  HWND                              m_hwnd;
  LPDIRECT3DDEVICE                  m_device;
  D3SceneRenderState                m_renderState;
  int                               m_maxLightCount;
  static int                        s_textureCoordCount;
  BitSet                           *m_lightsEnabled;
  BitSet                           *m_lightsDefined;
  CompactArray<MATERIAL>            m_materials;
  CompactArray<D3SceneObject*>      m_objectArray;
  int                               m_oldObjectCount;
  D3PosDirUpScale                   m_cameraPDUS, m_objectPDUS;
  float                             m_viewAngel;
  float                             m_nearViewPlane;
  Timestamp                         m_renderTime;
  bool                              m_initDone;
  
  void notifyPropertyChanged(int id, const void *oldValue, const void *newValue);
  void notifyIfObjectArrayChanged();
  void initCameraTrans(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up);
  void initObjTrans(const D3DXVECTOR3 &pos   = D3DXVECTOR3(0,0, 0)
                   ,const D3DXVECTOR3 &dir   = D3DXVECTOR3(0,0,-1)
                   ,const D3DXVECTOR3 &up    = D3DXVECTOR3(0,1, 0)
                   ,const D3DXVECTOR3 &scale = D3DXVECTOR3(1,1, 1));
  void updateViewMatrix();
  void updateProjMatrix();
  void setProjMatrix(       const D3DXMATRIX &m);
  inline void setViewMatrix(const D3DXMATRIX &m) {
    setTransformation(D3DTS_VIEW, m);
  }
  template<class T> void setRenderState(D3DRENDERSTATETYPE id, T value) {
    FV(m_device->SetRenderState(id, (DWORD)value));
  }

  void setTransformation(D3DTRANSFORMSTATETYPE id, const D3DXMATRIX &m);
  D3DXMATRIX getTransformation(D3DTRANSFORMSTATETYPE id) const;
  int getFirstFreeLightIndex() const; // return -1 if none exist
  int getFirstFreeMaterialIndex();
  D3LightControl *findLightControlByLightIndex(int lightIndex);
  D3LightControl *addLightControl(    UINT lightIndex);
  void destroyLightControl(           UINT lightIndex);
public:
  D3Scene();
  ~D3Scene();
  void init(HWND hwnd);
  void initTrans();
  void render();
  void OnSize();
  inline HWND getHwnd() {
    return m_hwnd;
  }
  void addSceneObject(   D3SceneObject *obj);
  void removeSceneObject(D3SceneObject *obj);
  void removeAllSceneObjects();
  void stopAllAnimations();
  inline int getObjectCount() const {
    return (int)m_objectArray.size();
  }

  D3Scene &setFillMode(D3DFILLMODE fillMode) {
    if(fillMode != getFillMode()) {
      setRenderState(D3DRS_FILLMODE, fillMode);
      m_renderState.m_fillMode = fillMode;
    }
    return *this;
  }
  inline D3DFILLMODE getFillMode() const {
    return m_renderState.m_fillMode;
  }
  D3Scene &setShadeMode(D3DSHADEMODE shadeMode) {
    if(shadeMode != getShadeMode()) {
      setRenderState(D3DRS_SHADEMODE, shadeMode);
      m_renderState.m_shadeMode = shadeMode;
    }
    return *this;
  }
  inline D3DSHADEMODE getShadeMode() const {
    return m_renderState.m_shadeMode;
  }
  D3Scene &setBackgroundColor(D3DCOLOR color) {
    setProperty(SP_BACKGROUNDCOLOR, m_renderState.m_backgroundColor, color);
    return *this;
  }
  inline D3PCOLOR getBackgroundColor() const {
    return m_renderState.m_backgroundColor;
  }
  D3Scene &setAmbientColor(D3DCOLOR color) {
    if(color != getAmbientColor()) {
      setRenderState(D3DRS_AMBIENT, color);
      setProperty(SP_AMBIENTLIGHT, m_renderState.m_ambientColor,color);
    }
    return *this;
  }
  inline D3PCOLOR getAmbientColor() const {
    return m_renderState.m_ambientColor;
  }
  D3Scene &D3Scene::setFVF(DWORD fvf) {
    if(fvf != getFVF()) {
      FV(m_device->SetFVF(fvf));
      m_renderState.m_fvf = fvf;
    }
    return *this;
  }
  inline DWORD getFVF() const {
    return m_renderState.m_fvf;
  }
  D3Scene &setZEnable(bool enabled) {
    if(enabled != isZEnable()) {
      setRenderState(D3DRS_AMBIENT, enabled ? TRUE : FALSE);
      m_renderState.m_zEnable = enabled;
    }
    return *this;
  }
  bool isZEnable() const {
    return m_renderState.m_zEnable;
  }
  D3Scene &setNormalizeNormalsEnable(bool enabled) {
    if(enabled != isNormalizeNormalsEnable()) {
      setRenderState(D3DRS_NORMALIZENORMALS, enabled ? TRUE : FALSE);
      m_renderState.m_normalizeNormals = enabled;
    }
    return *this;
  }
  bool isNormalizeNormalsEnable() const {
    return m_renderState.m_normalizeNormals;
  }
  D3Scene &D3Scene::setAlphaBlendEnable(bool enabled) {
    if(enabled != isAlphaBlendEnable()) {
      setRenderState(D3DRS_ALPHABLENDENABLE, enabled ? TRUE : FALSE);
      m_renderState.m_alphaBlendEnable = enabled;
    }
    return *this;
  }
  bool isAlphaBlendEnable() const {
    return m_renderState.m_alphaBlendEnable;
  }
  D3Scene &setLightingEnable(bool enabled) {
    if(enabled != isLightingEnable()) {
      setRenderState(D3DRS_LIGHTING, enabled ? TRUE : FALSE);
      m_renderState.m_lighting = enabled;
    }
    return *this;
  }
  inline bool isLightingEnable() const {
    return m_renderState.m_lighting;
  }
  D3Scene &setSpecularEnable(bool enabled) {
    if(enabled != isSpecularEnabled()) {
      setRenderState(D3DRS_SPECULARENABLE, enabled ? TRUE : FALSE);
      m_renderState.m_specularHighLightEnable = enabled;
    }
    return *this;
  }
  inline bool isSpecularEnabled() const {
    return m_renderState.m_specularHighLightEnable;
  }
  D3Scene &selectMaterial(int materialIndex) {
    if(materialIndex != m_renderState.m_selectedMaterialIndex) {
      if((UINT)materialIndex < m_materials.size()) {
        m_device->SetMaterial(&getMaterial(materialIndex));
      }
      m_renderState.m_selectedMaterialIndex = materialIndex;
    }
    return *this;
  }
  inline D3Scene &unselectMaterial() {
    m_renderState.m_selectedMaterialIndex = -1;
    return *this;
  }
  inline D3Scene &setSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value) {
    FV(m_device->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
    return *this;
  }
  inline D3Scene &setTexture(DWORD stage, LPDIRECT3DTEXTURE texture) {
    FV(m_device->SetTexture(stage, texture));
    return *this;
  }

  inline D3PosDirUpScale &getObjPDUS() {
    return m_objectPDUS;
  }
  inline String getObjString() const {
    return format(_T("Object:%s"), m_objectPDUS.toString().cstr());
  }

  inline const D3PosDirUpScale &getCameraPDUS() const {
    return m_cameraPDUS;
  }
  inline const D3DXVECTOR3 &getCameraPos() const {
    return m_cameraPDUS.getPos();
  }
  inline const D3DXVECTOR3 &getCameraDir() const {
    return m_cameraPDUS.getDir();
  }
  inline const D3DXVECTOR3 &getCameraUp() const {
    return m_cameraPDUS.getUp();
  }
  inline D3DXVECTOR3 getCameraRight() const {
    return m_cameraPDUS.getRight();
  }
  void setCameraPDUS(       const D3PosDirUpScale &pdus);
  void setCameraPos(        const D3DXVECTOR3     &pos);
  void setCameraOrientation(const D3DXVECTOR3     &dir, const D3DXVECTOR3 &up);
  void setCameraLookAt(     const D3DXVECTOR3     &point);
  inline String getCameraString() const {
    return format(_T("Camera:%s"), m_cameraPDUS.toString().cstr());
  }
  void setViewAngel(float angel);
  inline float getViewAngel() const {
    return m_viewAngel;
  }
  void setNearViewPlane(float zn);
  inline float getNearViewPlane() const {
    return m_nearViewPlane;
  }
  inline D3DXMATRIX getProjMatrix() const {
    return getTransformation(D3DTS_PROJECTION);
  }

  void setAnimationFrameIndex(int &oldValue, int newValue);

  void   setLightDirection(        UINT index, const D3DXVECTOR3 &dir);
  void   setLightPosition(         UINT index, const D3DXVECTOR3 &pos);
  String getLightString(           UINT index) const;
  String getLightString() const;

  LIGHT  getDefaultLightParam(D3DLIGHTTYPE type = D3DLIGHT_DIRECTIONAL);
  void   setLightParam(            const LIGHT &param);
  LIGHT  getLightParam(            UINT index) const;
  const CompactArray<LIGHT> getAllLights() const;
  inline D3DLIGHTTYPE getLightType(UINT index) const {
    return getLightParam(index).Type;
  }
  void removeLight(                UINT index);
  void setLightEnabled(            UINT index, bool enabled);
  inline bool isLightEnabled(      UINT index) const {
    return m_lightsEnabled->contains(index);
  }
  inline bool isLightDefined(      UINT index) const {
    return m_lightsDefined->contains(index);
  }
  inline bool isLightVisible(      UINT index) const {
    return getLightsVisible().contains(index);
  }
  inline const BitSet &getLightsDefined() const {
    return *m_lightsDefined;
  }
  inline const BitSet &getLightsEnabled() const {
    return *m_lightsEnabled;
  }
  BitSet getLightsVisible() const;
  D3LightControl *setLightControlVisible(UINT index, bool visible);

  inline int getMaxLightCount() const {
    return m_maxLightCount;
  }
  inline int getLightCount() const {
    return (int)m_lightsDefined->size();
  }
  inline int getLightEnabledCount() const {
    return (int)m_lightsEnabled->size();
  }

  D3DMATERIAL getDefaultMaterial() const;
  inline const MATERIAL &getMaterial(UINT index) const {
    return m_materials[index];
  }
  const CompactArray<MATERIAL> &getAllMaterials() const {
    return m_materials;
  }
  void setMaterial(const MATERIAL &material);
  int  addMaterial(const D3DMATERIAL &material);
  void removeMaterial(UINT index);

  inline bool isMaterialDefined(UINT index) const {
    return (index < m_materials.size()) && (m_materials[index].m_index == index);
  }
  const BitSet getMaterialsDefined() const;
  inline int getMaterialCount() const {
    return (int)getMaterialsDefined().size();
  }
  String getMaterialString(UINT index) const;
  String getMaterialString() const;

  static inline int getTextureCoordCount() {
    return s_textureCoordCount;
  }
  static inline const D3PosDirUpScale &getOrigo() {
    return s_pdusOrigo;
  }
  inline void setStreamSource(LPDIRECT3DVERTEXBUFFER buffer, int vertexSize, DWORD fvf) {
    FV(m_device->SetStreamSource(0, buffer, 0, vertexSize));
    setFVF(fvf);
  }
  inline void setIndices(LPDIRECT3DINDEXBUFFER indexBuffer) {
    FV(m_device->SetIndices(indexBuffer));
  }
  inline void drawPrimitive(D3DPRIMITIVETYPE pt, int startVertex, int primitiveCount) {
    FV(m_device->DrawPrimitive(pt, startVertex, primitiveCount));
  }
  inline void drawIndexedPrimitive(D3DPRIMITIVETYPE pt, int baseVertexIndex, UINT minVertexIndex, UINT numVertices, UINT startIndex, UINT primCount) {
    FV(m_device->DrawIndexedPrimitive(pt, baseVertexIndex, minVertexIndex, numVertices, startIndex, primCount));
  }
  D3Ray          getPickRay(     const CPoint &point) const;
  D3SceneObject *getPickedObject(const CPoint &point, long mask = PICK_ALL, D3DXVECTOR3 *hitPoint = NULL, D3PickedInfo *info = NULL) const;

  LPDIRECT3DVERTEXBUFFER  allocateVertexBuffer(DWORD fvf , UINT count, UINT *bufferSize = NULL);
  LPDIRECT3DINDEXBUFFER   allocateIndexBuffer( bool int32, UINT count, UINT *bufferSize = NULL);
  LPD3DXMESH              allocateMesh(        DWORD fvf , UINT faceCount, UINT vertexCount, DWORD options) {
    LPD3DXMESH mesh;
    FV(D3DXCreateMeshFVF(faceCount, vertexCount, options, fvf, m_device, &mesh));
    return mesh;
  }
  LPDIRECT3DTEXTURE loadTextureFromFile(const String &fileName) {
    return AbstractTextureFactory::loadTextureFromFile(m_device, fileName);
  }
  LPDIRECT3DTEXTURE loadTextureFromResource(int resId, const String &typeName) {
    return AbstractTextureFactory::loadTextureFromResource(m_device, resId, typeName);
  }
  LPDIRECT3DTEXTURE loadTextureFromByteArray(ByteArray &ba) {
    return AbstractTextureFactory::loadTextureFromByteArray(m_device, ba);
  }
  LPDIRECT3DTEXTURE getTextureFromBitmap(HBITMAP bm) {
    return AbstractTextureFactory::getTextureFromBitmap(m_device, bm);
  }
  LPDIRECT3DTEXTURE loadTextureFromBitmapResource(int id) {
    return AbstractTextureFactory::loadTextureFromBitmapResource(m_device, id);
  }

  LPD3DXEFFECT      compileEffect(const String &srcText, StringArray &errorMsg);
  void saveState(const String &fileName) const;
  void loadState(const String &fileName);
  void save(      ByteOutputStream &s) const;
  void load(      ByteInputStream  &s);
  void saveLights(ByteOutputStream &s) const;
  void loadLights(ByteInputStream  &s);
};

LPD3DXMESH        createMeshFromVertexFile(     AbstractMeshFactory &amf, const String &fileName, bool doubleSided);
LPD3DXMESH        createMeshFromObjFile(        AbstractMeshFactory &amf, const String &fileName, bool doubleSided);
//LPD3DXMESH     createMeshMarchingCube(     LPDIRECT3DDEVICE device, const IsoSurfaceParameters        &param);

String toString(LPD3DXMESH             mesh        );
String toString(LPDIRECT3DVERTEXBUFFER vertexBuffer);
String toString(LPDIRECT3DINDEXBUFFER  indexBuffer );

class D3SceneObject {
private:
  const D3PosDirUpScale &getConstPDUS() const {
    return ((D3SceneObject*)this)->getPDUS();
  }

protected:
  D3Scene     &m_scene;
  String       m_name;
  bool         m_visible;
  void        *m_userData;
  inline void setFillAndShadeMode() {
    m_scene.setFillMode(getFillMode()).setShadeMode(getShadeMode());
  }
  inline void setSceneMaterial() {
    if(hasMaterial()) m_scene.selectMaterial(getMaterialIndex());
  }
  inline void setLightingEnable(bool enabled) {
    m_scene.setLightingEnable(enabled);
  }
  inline void drawPrimitive(D3DPRIMITIVETYPE pt, int startVertex, int count) {
    m_scene.drawPrimitive(pt, startVertex, count);
  }
public:
  D3SceneObject(D3Scene &scene, const String &name=_T("Untitled")) 
    : m_scene(scene)
    , m_name(name)
    , m_visible(true)
    , m_userData(NULL)
  {
  }
  virtual ~D3SceneObject() {
  }
  virtual SceneObjectType getType() const {
    return SOTYPE_VISUALOBJECT;
  }
  virtual void draw() = 0;
  virtual LPD3DXMESH getMesh() const {
    return NULL;
  }
  virtual D3PosDirUpScale &getPDUS() {
    return m_scene.getObjPDUS();
  }
  virtual int getMaterialIndex() const {
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
  const MATERIAL &getMaterial() const {
    return getScene().getMaterial(getMaterialIndex());
  }
  inline bool hasMaterial() const {
    return getMaterialIndex() >= 0;
  }
  D3DXMATRIX getWorldMatrix() const {
    return getConstPDUS().getWorldMatrix();
  }
  inline const D3DXVECTOR3 &getPos() const {
    return getConstPDUS().getPos();
  }
  inline const D3DXVECTOR3 &getDir() const {
    return getConstPDUS().getDir();
  }
  inline const D3DXVECTOR3 &getUp() const {
    return getConstPDUS().getUp();
  }
  inline D3DXVECTOR3 getRight() const {
    return getConstPDUS().getRight();
  }
  inline D3DXVECTOR3 getScale() const {
    return getConstPDUS().getScale();
  }
  inline void setPos(const D3DXVECTOR3 &pos) {
    getPDUS().setPos(pos);
  }
  inline void setOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
    getPDUS().setOrientation(dir,up);
  }
  inline void setScale(const D3DXVECTOR3 &scale) {
    getPDUS().setScale(scale);
  }
  inline void setScaleAll(float scale) {
    getPDUS().setScaleAll(scale);
  }
  bool intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info = NULL) const;

  inline const String &getName() const {
    return m_name;
  }
  inline void setName(const String &name) {
    m_name = name;
  }
};

class SceneObjectWithVertexBuffer : public D3SceneObject {
protected:
  int                    m_primitiveCount;
  DWORD                  m_fvf;
  int                    m_vertexSize;
  LPDIRECT3DVERTEXBUFFER m_vertexBuffer;
  void *allocateVertexBuffer(int vertexSize, UINT count, DWORD fvf);
  void unlockVertexBuffer();
  inline void setStreamSource() {
    getScene().setStreamSource(m_vertexBuffer, m_vertexSize, m_fvf);
  }

public:
  SceneObjectWithVertexBuffer(D3Scene &scene);
  ~SceneObjectWithVertexBuffer();

  inline LPDIRECT3DVERTEXBUFFER &getVertexBuffer() {
    return m_vertexBuffer;
  }
  String toString() const {
    return format(_T("%s\nVertexBuffer:\n%s")
                 ,__super::toString().cstr()
                 ,indentString(::toString(m_vertexBuffer),2).cstr());
  }
};

class SceneObjectWithIndexBuffer : public SceneObjectWithVertexBuffer {
protected:
  LPDIRECT3DINDEXBUFFER m_indexBuffer;
  void *allocateIndexBuffer(bool int32, int count);
  void unlockIndexBuffer();

  inline void setIndices() {
    getScene().setIndices(m_indexBuffer);
  }

public:
  SceneObjectWithIndexBuffer(D3Scene &scene);
  ~SceneObjectWithIndexBuffer();
  String toString() const {
    return format(_T("%s\nIndexBuffer:\n%s")
                 ,__super::toString().cstr()
                 ,indentString(::toString(m_indexBuffer),2).cstr());
  }
};

class SceneObjectWithMesh : public D3SceneObject {
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
  SceneObjectWithMesh(D3Scene &scene, LPD3DXMESH mesh = NULL); // if mesh != NULL, it will be released when Object is destroyed
  ~SceneObjectWithMesh();
  LPD3DXMESH getMesh() const {
    return m_mesh;
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
  void draw();
  String toString() const {
    return format(_T("%s\nMesh:\n%s")
                 ,getName().cstr()
                 ,indentString(::toString(getMesh()),2).cstr());
  }
};

class SceneObjectBox : public SceneObjectWithMesh {
private:
  void makeSquareFace(MeshBuilder &mb, int v0, int v1, int v2, int v3);
public:
  SceneObjectBox(D3Scene &scene, const D3DXCube3 &cube);
};

class D3LineArray : public SceneObjectWithVertexBuffer {
private:
  void initBuffer(const Line *lines, int n);
public:
  D3LineArray(D3Scene &scene, const Line *lines, int n);
  D3LineArray(D3Scene &scene, const Vertex p1, const Vertex p2); // make a wireframe box
  void draw();
};

class D3Curve : public SceneObjectWithVertexBuffer {
public:
  D3Curve(D3Scene &scene, const VertexArray &points);
  void draw();
};

class D3CurveArray : public SceneObjectWithVertexBuffer {
private:
  CompactIntArray m_curveSize;
public:
  D3CurveArray(D3Scene &scene, const CurveArray &curves);
  void draw();
};

typedef enum {
  ANIMATE_FORWARD
 ,ANIMATE_BACKWARD
 ,ANIMATE_ALTERNATING
} AnimationType;

class D3AnimatedSurface : public D3SceneObject {
private:
  friend class MeshAnimationThread;
  MeshArray            m_meshArray;
  int                  m_nextMeshIndex, m_lastRenderedIndex;
  MeshAnimationThread *m_animator;
public:
  D3AnimatedSurface(D3Scene &scene, const MeshArray &meshArray);
  ~D3AnimatedSurface();
  void startAnimation(AnimationType type = ANIMATE_FORWARD);
  void stopAnimation();
  bool isRunning() const;
  AnimationType getAnimationType() const;
  void scaleSpeed(double factor); // sleepTime /= factor
  double getFramePerSec() const;
  LPD3DXMESH getMesh() const;
  void draw();
  SceneObjectType getType() const {
    return SOTYPE_ANIMATEDOBJECT;
  }
};

class D3LineArrow : public SceneObjectWithVertexBuffer {
private:
  int m_materialIndex;
public:
  D3LineArrow(D3Scene &scene, const Vertex &from, const Vertex &to, D3DCOLOR color = 0);
  int getMaterialIndex() const {
    return m_materialIndex;
  }
  void setColor(D3DCOLOR color);
  void draw();
};

class D3PickRayArrow : public D3LineArrow {
private:
  D3PosDirUpScale m_pdus;
public:
  D3PickRayArrow(D3Scene &scene, const D3Ray &ray) : D3LineArrow(scene, ray.m_orig, ray.m_orig + 2 * ray.m_dir) {
    D3DXMATRIX m;
    m_pdus.setWorldMatrix(*D3DXMatrixIdentity(&m));
  }
  D3PosDirUpScale &getPDUS() {
    return m_pdus;
  }
};

CurveArray createSphereObject(double r);
