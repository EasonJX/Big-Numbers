#pragma once

#include <CompactArray.h>
#include <BitSet.h>
#include <Date.h>
#include <Timer.h>
#include <XMLDoc.h>
#include <PropertyContainer.h>
#include <NumberInterval.h>
#include <MFCUtil/D3DeviceFactory.h>
#include <MFCUtil/ColorSpace.h>
#include "D3TextureFactory.h"
#include "MeshBuilder.h"
#include "MeshArray.h"
#include "Light.h"
#include "Material.h"
#include "D3Math.h"

class D3LightControl;
class D3SceneObject;

typedef enum {
  SP_BACKGROUNDCOLOR           // D3DCOLOR
 ,SP_AMBIENTCOLOR              // D3DCOLOR
 ,SP_CAMERAPDUS                // D3PosDirUpScale
 ,SP_PROJECTIONTRANSFORMATION  // D3DXMATRIX
 ,SP_RIGHTHANDED               // bool
 ,SP_LIGHTPARAMETERS           // LIGHT
 ,SP_LIGHTCOUNT                // UINT
 ,SP_MATERIALPARAMETERS        // MATERIAL
 ,SP_MATERIALCOUNT             // UINT
 ,SP_RENDERTIME                // Timestamp
 ,SP_OBJECTCOUNT               // UINT
 ,SP_ANIMATIONFRAMEINDEX       // int
} D3SceneProperty;

typedef enum {
  SOTYPE_NULL
 ,SOTYPE_VISUALOBJECT
 ,SOTYPE_LIGHTCONTROL
 ,SOTYPE_ANIMATEDOBJECT
} SceneObjectType;

#define OBJTYPE_MASK(type) (1 << (type))

#define OBJMASK_VISUALOBJECT   OBJTYPE_MASK(SOTYPE_VISUALOBJECT  )
#define OBJMASK_LIGHTCONTROL   OBJTYPE_MASK(SOTYPE_LIGHTCONTROL  )
#define OBJMASK_ANIMATEDOBJECT OBJTYPE_MASK(SOTYPE_ANIMATEDOBJECT)
#define OBJMASK_ALL            (-1)

// Informarmation about where a ray intersects a mesh
class D3PickedInfo {
private:
  // Index of face.
  int           m_faceIndex;
  // Indices into vertexArray
  int           m_vertexIndex[3];
  // TextureVertex. Relative coordinates in picked face
  D3DXVECTOR3   m_facePoint[3];
  float         m_U, m_V;
public:
  inline D3PickedInfo() {
    clear();
  }
  inline D3PickedInfo(int faceIndex, int vertexIndex[3], D3DXVECTOR3 facePoint[3], float U,float V)
                    : m_faceIndex(faceIndex)
                    , m_U(U), m_V(V)
  {
    for(int i = 0; i < 3; i++) {
      m_vertexIndex[i] = vertexIndex[i];
      m_facePoint[i]   = facePoint[i];
    }
  }
  inline void clear() {
    memset(this, 0, sizeof(D3PickedInfo));
    m_faceIndex = -1;
  }
  inline bool isEmpty() const {
    return m_faceIndex < 0;
  }
  inline int getFaceIndex() const {
    return m_faceIndex;
  }
  inline TextureVertex getTextureVertex() const {
    return TextureVertex(m_U, m_V);
  }
  inline D3DXVECTOR3 getMeshPoint() const {
    return m_facePoint[0] + m_U * (m_facePoint[1] - m_facePoint[0]) + m_V * (m_facePoint[2] - m_facePoint[0]);
  }
  // i = [0..2];
  inline int getVertexIndex(int i) const {
    return m_vertexIndex[i];
  }
  // i = [0..2];
  inline const D3DXVECTOR3 &getVertexPoint(int i) const {
    return m_facePoint[i];
  }
  String toString(int dec = 3) const;
};

class D3SceneRenderState {
public:
  D3DFILLMODE    m_fillMode;
  D3DSHADEMODE   m_shadeMode;
  D3DCOLOR       m_backgroundColor;
  D3DCOLOR       m_ambientColor;
  DWORD          m_fvf;
  D3DCULL        m_cullMode;
  D3DBLEND       m_srcBlend;
  D3DBLEND       m_dstBlend;
  D3DZBUFFERTYPE m_zEnable;
  bool           m_normalizeNormals        : 1;
  bool           m_alphaBlendEnable        : 1;
  bool           m_lighting                : 1;
  bool           m_specularHighLightEnable : 1;
  int            m_selectedMaterialId;

  D3SceneRenderState() {
    setDefault();
  }
  void getValuesFromDevice(LPDIRECT3DDEVICE device);
  void setValuesToDevice(  LPDIRECT3DDEVICE device);
  void setDefault();
};

#define SC_INITDONE    0x01
#define SC_PROPCHANGES 0x02
#define SC_RIGHTHANDED 0x04

class D3Scene : public PropertyContainer
              , public AbstractMeshFactory
              , public AbstractTextureFactory
{
private:
  friend class D3SceneObjectIterator;

  HWND                              m_hwnd;
  BYTE                              m_stateFlags;
  LPDIRECT3DDEVICE                  m_device;
  D3SceneRenderState                m_renderState;
  D3PosDirUpScale                   m_origoPDUS, m_camPDUS, m_defaultObjPDUS;
  int                               m_maxLightCount;
  static int                        s_textureCoordCount;
  BitSet                            m_lightsEnabled, m_lightsDefined;
  MATERIAL                          m_undefinedMaterial;
  MaterialMap                       m_materialMap;
  CompactArray<D3SceneObject*>      m_objectArray;
  float                             m_viewAngel;
  float                             m_nearViewPlane;
  Timestamp                         m_renderTime;

  inline D3Scene &setFlags(BYTE flags) {
    m_stateFlags |= flags;
    return *this;
  }
  inline D3Scene &clrFlags(BYTE flags) {
    m_stateFlags &= ~flags;
    return *this;
  }
  inline bool isSet(BYTE flags) const {
    return (m_stateFlags & flags) != 0;
  }

  void notifyPropertyChanged(int id, const void *oldValue, const void *newValue);
  void notifyObjectCountChanged(UINT oldCount);
  void setCamLookAt(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up);
  void setDefaultObjTrans(const D3DXVECTOR3 &pos
                         ,const D3DXVECTOR3 &dir
                         ,const D3DXVECTOR3 &up
                         ,const D3DXVECTOR3 &scale);

  // call setDevViewMatrix(m_camPDUS.getViewMatrix());
  D3Scene &updateDevViewMatrix();
  // call setDevProjMatrix with newly generated projection-matrix
  D3Scene &updateDevProjMatrix();
  // call setDevTransformation(D3DTS_PROJECTION, m), and notify propertychangeListeners with propertyId=SP_PROJECTIONTRANSFORMATION
  D3Scene &setDevProjMatrix(       const D3DXMATRIX &m);
  // call setDevTransformation(D3DTS_VIEW, m).
  inline D3Scene &setDevViewMatrix(const D3DXMATRIX &m) {
    return setDevTransformation(D3DTS_VIEW, m);
  }
  template<typename T> void setDevRenderState(D3DRENDERSTATETYPE id, T value) {
    FV(m_device->SetRenderState(id, (DWORD)value));
  }

  // set's the specified transformation on m_device. Doesn't notify propertyChangeListeners
  D3Scene &setDevTransformation(D3DTRANSFORMSTATETYPE id, const D3DXMATRIX &m);
  D3DXMATRIX getDevTransformation(D3DTRANSFORMSTATETYPE id) const;

  // Return -1 if none exist
  int  getFirstFreeLightIndex() const;
  UINT getFirstFreeMaterialId() const;
  void removeAllLights();
  D3LightControl *findLightControlByLightIndex(int lightIndex);
  D3LightControl *addLightControl(    UINT lightIndex);
  // Remove the lightControl associated with the given lightIndex from the scene, if its allocated,
  // AND delete the LightControl. the light will remain in its current state
  void destroyLightControl(           UINT lightIndex);
  // Remove and destroy all allocated lightcontrols
  // The lights will remain in their current states
  void destroyAllLightControls();
  void removeSceneObject(size_t index);
public:
  D3Scene(bool rightHanded = true);
  ~D3Scene();
  void init(HWND hwnd);
  void close();
  void initTrans();
  void resetCamTrans();
  void resetDefaultObjTrans();
  void render();
  void OnSize();
  inline HWND getHwnd() {
    return m_hwnd;
  }
  inline LPDIRECT3DDEVICE getDevice() {
    return m_device;
  }
  void addSceneObject(   D3SceneObject *obj);
  // Remove obj from scene. if obj is an animated object, the animationthread will be stopped.
  // Does NOT delete the object
  void removeSceneObject(D3SceneObject *obj);
  void removeAllSceneObjects();
  Iterator<D3SceneObject*> getObjectIterator(long mask = OBJMASK_ALL) const;
  bool isSceneObject(const D3SceneObject *obj) const;
  void stopAllAnimations();
  inline int getObjectCount() const {
    return (int)m_objectArray.size();
  }

  // Set device fill-mode (D3DRS_FILLMODE), and update m_renderState.m_fillMode, if different from current
  D3Scene &setFillMode(D3DFILLMODE fillMode) {
    if(fillMode != getFillMode()) {
      setDevRenderState(D3DRS_FILLMODE, fillMode);
      m_renderState.m_fillMode = fillMode;
    }
    return *this;
  }
  inline D3DFILLMODE getFillMode() const {
    return m_renderState.m_fillMode;
  }
  // Set device' shade-mode (D3DRS_SHADEMODE), and update m_renderState.m_shademode, if different from current
  D3Scene &setShadeMode(D3DSHADEMODE shadeMode) {
    if(shadeMode != getShadeMode()) {
      setDevRenderState(D3DRS_SHADEMODE, shadeMode);
      m_renderState.m_shadeMode = shadeMode;
    }
    return *this;
  }
  inline D3DSHADEMODE getShadeMode() const {
    return m_renderState.m_shadeMode;
  }
  // set property renderState.m_backgroundColor, and notifies properyChangeListerners. (property-id=SP_BACKGROUNDCOLOR)
  D3Scene &setBackgroundColor(D3DCOLOR color) {
    setProperty(SP_BACKGROUNDCOLOR, m_renderState.m_backgroundColor, color);
    return *this;
  }
  inline D3PCOLOR getBackgroundColor() const {
    return m_renderState.m_backgroundColor;
  }
  // set device ambient light (D3DRS_AMBIENT) , and notifies properyChangeListerners. (property-id=SP_AMBIENTCOLOR)
  D3Scene &setAmbientColor(D3DCOLOR color) {
    if(color != getAmbientColor()) {
      setDevRenderState(D3DRS_AMBIENT, color);
      setProperty(SP_AMBIENTCOLOR, m_renderState.m_ambientColor,color);
    }
    return *this;
  }
  inline D3PCOLOR getAmbientColor() const {
    return m_renderState.m_ambientColor;
  }

  // call device->SetFVF and update m_renderState.m_fvf if different from current
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
  inline D3DCULL getCullMode() const {
    return m_renderState.m_cullMode;
  }
  // set device cullmode (D3DRS_CULLMODE) and and update m_renderState.m_cullmode if different from current
  inline D3Scene &setCullMode(D3DCULL cullMode) {
    if(cullMode != getCullMode()) {
      setDevRenderState(D3DRS_CULLMODE, cullMode);
      m_renderState.m_cullMode = cullMode;
    }
    return *this;
  }
  inline D3DBLEND getSrcBlend() const {
    return m_renderState.m_srcBlend;
  }
  // set device srcblend (D3DRS_SRCBLEND) and and update m_renderState.m_srcBlend if different from current
  inline D3Scene &setSrcBlend(D3DBLEND blend) {
    if(blend != getSrcBlend()) {
      setDevRenderState(D3DRS_SRCBLEND, blend);
      m_renderState.m_srcBlend = blend;
    }
    return *this;
  }
  inline D3DBLEND getDstBlend() const {
    return m_renderState.m_dstBlend;
  }
  // set device dstblend (D3DRS_DESTBLEND) and and update m_renderState.m_dstBlend if different from current
  inline D3Scene &setDstBlend(D3DBLEND blend) {
    if(blend != getDstBlend()) {
      setDevRenderState(D3DRS_DESTBLEND, blend);
      m_renderState.m_dstBlend = blend;
    }
    return *this;
  }

  // set device zenable (D3DRS_ZENABLE) and and update m_renderState.m_zEnable if different from current
  D3Scene &setZEnable(D3DZBUFFERTYPE bufferType) {
    if(bufferType != getZEnable()) {
      setDevRenderState(D3DRS_ZENABLE, bufferType);
      m_renderState.m_zEnable = bufferType;
    }
    return *this;
  }
  inline D3DZBUFFERTYPE getZEnable() const {
    return m_renderState.m_zEnable;
  }

  // set device normalizeNormals (D3DRS_NORMALIZENORMALS) and and update m_renderState.m_normalizeNormals if different from current
  D3Scene &setNormalizeNormalsEnable(bool enabled) {
    if(enabled != isNormalizeNormalsEnable()) {
      setDevRenderState(D3DRS_NORMALIZENORMALS, enabled ? TRUE : FALSE);
      m_renderState.m_normalizeNormals = enabled;
    }
    return *this;
  }
  inline bool isNormalizeNormalsEnable() const {
    return m_renderState.m_normalizeNormals;
  }
  // set device enableAlphaBlend (D3DRS_ALPHABLENDENABLE) and and update m_renderState.m_alphaBlendEnable if different from current
  D3Scene &D3Scene::setAlphaBlendEnable(bool enabled) {
    if(enabled != isAlphaBlendEnable()) {
      setDevRenderState(D3DRS_ALPHABLENDENABLE, enabled ? TRUE : FALSE);
      m_renderState.m_alphaBlendEnable = enabled;
    }
    return *this;
  }
  inline bool isAlphaBlendEnable() const {
    return m_renderState.m_alphaBlendEnable;
  }
  // set device lightning (D3DRS_LIGHTING) and and update m_renderState.m_lighting if different from current
  D3Scene &setLightingEnable(bool enabled) {
    if(enabled != isLightingEnable()) {
      setDevRenderState(D3DRS_LIGHTING, enabled ? TRUE : FALSE);
      m_renderState.m_lighting = enabled;
    }
    return *this;
  }
  inline bool isLightingEnable() const {
    return m_renderState.m_lighting;
  }

  // set device specularHighLightEnable (D3DRS_SPECULARENABLE),
  // and update m_renderState.m_specularHighLightEnable if different from current
  D3Scene &setSpecularEnable(bool enabled) {
    if(enabled != isSpecularEnabled()) {
      setDevRenderState(D3DRS_SPECULARENABLE, enabled ? TRUE : FALSE);
      m_renderState.m_specularHighLightEnable = enabled;
    }
    return *this;
  }
  inline bool isSpecularEnabled() const {
    return m_renderState.m_specularHighLightEnable;
  }
  // call m_device->SetMaterial(&mat), where mat is a material created in Scene internal table,
  // and update m_renderState.m_selectedMaterialId = materialId;
  // a warning is shown, if material is undefined in table
  D3Scene &selectMaterial(UINT materialId);
  inline D3Scene &unselectMaterial() {
    m_renderState.m_selectedMaterialId = -1;
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

  void setRightHanded(bool rightHanded);
  inline bool getRightHanded() const {
    return isSet(SC_RIGHTHANDED);
  }
  const D3PosDirUpScale &getOrigoPDUS() const {
    return m_origoPDUS;
  }

  inline D3PosDirUpScale &getDefaultPDUS() {
    return m_defaultObjPDUS;
  }

  inline const D3PosDirUpScale &getCamPDUS() const {
    return m_camPDUS;
  }
  inline const D3DXVECTOR3 &getCamPos() const {
    return getCamPDUS().getPos();
  }
  inline const D3DXVECTOR3 &getCamDir() const {
    return getCamPDUS().getDir();
  }
  inline const D3DXVECTOR3 &getCamUp() const {
    return getCamPDUS().getUp();
  }
  inline D3DXVECTOR3 getCamRight() const {
    return getCamPDUS().getRight();
  }
  void setCamPDUS(       const D3PosDirUpScale &pdus);
  void setCamPos(        const D3DXVECTOR3     &pos);
  void setCamOrientation(const D3DXVECTOR3     &dir, const D3DXVECTOR3 &up);
  void setCamLookAt(     const D3DXVECTOR3     &point);
  String getCamString() const;
  D3Scene &setViewAngel(float angel);
  inline float getViewAngel() const {
    return m_viewAngel;
  }
  D3Scene &setNearViewPlane(float zn);
  inline float getNearViewPlane() const {
    return m_nearViewPlane;
  }
  inline D3DXMATRIX getDevProjMatrix() const {
    return getDevTransformation(D3DTS_PROJECTION);
  }
  inline D3Scene &setDevWorldMatrix(const D3DXMATRIX &world) {
    return setDevTransformation(D3DTS_WORLD, world);
  }
  inline D3DXMATRIX getDevWorldMatrix() const {
    return getDevTransformation(D3DTS_WORLD);
  }
  void setAnimationFrameIndex(int &oldValue, int newValue);

// --------------------------- LIGHT ----------------------------

  void   setLight(            const LIGHT &param);
  LIGHT  getLight(            UINT lightIndex) const;
  LightArray          getAllLights() const;
  inline D3DLIGHTTYPE getLightType(UINT lightIndex) const {
    return getLight(lightIndex).Type;
  }
  UINT addLight(                   const D3DLIGHT &light);
  void removeLight(                UINT lightIndex);
  void setLightEnabled(            UINT lightIndex, bool enabled);
  inline bool isLightEnabled(      UINT lightIndex) const {
    return m_lightsEnabled.contains(lightIndex);
  }
  inline bool isLightDefined(      UINT lightIndex) const {
    return m_lightsDefined.contains(lightIndex);
  }
  inline const BitSet &getLightsDefined() const {
    return m_lightsDefined;
  }
  inline const BitSet &getLightsEnabled() const {
    return m_lightsEnabled;
  }
  inline bool isLightControlVisible(UINT lightIndex) const {
    return getLightControlsVisible().contains(lightIndex);
  }
  BitSet getLightControlsVisible() const;
  D3LightControl *setLightControlVisible(UINT lightIndex, bool visible);

  inline int getMaxLightCount() const {
    return m_maxLightCount;
  }
  inline int getLightCount() const {
    return (int)m_lightsDefined.size();
  }
  inline int getLightEnabledCount() const {
    return (int)m_lightsEnabled.size();
  }
  void   setLightDirection(        UINT lightIndex, const D3DXVECTOR3 &dir);
  void   setLightPosition(         UINT lightIndex, const D3DXVECTOR3 &pos);
  String getLightString(           UINT lightIndex) const;
  String getLightString() const;

// --------------------------- MATERIAL ----------------------------

  inline const MATERIAL &getMaterial(UINT materialId) const {
    const MATERIAL *m = m_materialMap.get(materialId);
    return m ? *m : m_undefinedMaterial;
  }
  const MaterialMap &getAllMaterials() const {
    return m_materialMap;
  }
  void setMaterial(const MATERIAL    &material);
  UINT addMaterial(const D3DMATERIAL &material);
  void removeMaterial(UINT materialId);
  // assume lcMaterial is defined. Dont notify listeners about change in material
  void setLightControlMaterial(const MATERIAL &lcMaterial);

  inline bool isMaterialDefined(UINT materialId) const {
    return m_materialMap.get(materialId) != NULL;
  }
  inline UINT getMaterialCount() const {
    return (UINT)m_materialMap.size();
  }
  String getMaterialString(UINT materialId) const;
  String getMaterialString() const;

  static inline int getTextureCoordCount() {
    return s_textureCoordCount;
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
  D3Ray          getPickedRay(     const CPoint &point) const;
  // if hitPoint != NULL, it will receive point (in worldspace) of rays intersection with nearest object
  // if ray != NULL, it will receive value returned by getPicRay(point)
  // if dist != NULL, is will receieve distnce to hitpoint, if any
  // if info != NULL, it will receieve info returned by SceneObject::intersectWithRay, for the closest, visible object
  D3SceneObject *getPickedObject(const CPoint &point, long mask = OBJMASK_ALL, D3DXVECTOR3 *hitPoint = NULL, D3Ray *ray  = NULL, float *dist = NULL, D3PickedInfo *info = NULL) const;
  D3SceneObject *getPickedObject(const D3Ray  &ray  , long mask = OBJMASK_ALL, D3DXVECTOR3 *hitPoint = NULL, float *dist = NULL, D3PickedInfo *info = NULL) const;

  template<typename VertexType> LPDIRECT3DVERTEXBUFFER allocateVertexBuffer(UINT count, UINT *bufferSize = NULL) {
    const UINT vertexSize = sizeof(VertexType);
    UINT tmp, &totalSize = bufferSize ? *bufferSize : tmp;
    totalSize = vertexSize * count;
    LPDIRECT3DVERTEXBUFFER result;
    V(m_device->CreateVertexBuffer(totalSize, 0, VertexType::FVF_Flags, D3DPOOL_DEFAULT, &result, NULL)); TRACE_CREATE(result);
    return result;
  }

  LPDIRECT3DINDEXBUFFER   allocateIndexBuffer( bool int32, UINT count, UINT *bufferSize = NULL);
  LPD3DXMESH              allocateMesh(        DWORD fvf , UINT faceCount, UINT vertexCount, DWORD options);
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
  void save(const String &fileName) const;
  void load(const String &fileName);
  void save(         XMLDoc &doc) const;
  void load(         XMLDoc &doc);
  void saveLights(   XMLDoc &doc, XMLNodePtr parent) const;
  void loadLights(   XMLDoc &doc, XMLNodePtr parent);
  void saveMaterials(XMLDoc &doc, XMLNodePtr parent) const;
  void loadMaterials(XMLDoc &doc, XMLNodePtr parent);
};

LPD3DXMESH        createMeshFromVertexFile(     AbstractMeshFactory &amf, const String &fileName, bool doubleSided);
LPD3DXMESH        createMeshFromObjFile(        AbstractMeshFactory &amf, const String &fileName, bool doubleSided);
//LPD3DXMESH     createMeshMarchingCube(     LPDIRECT3DDEVICE device, const IsoSurfaceParameters        &param);

D3DXCube3 getBoundingBox(LPDIRECT3DVERTEXBUFFER vertexBuffer);
D3DXCube3 getBoundingBox(LPD3DXMESH             mesh);

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
    if(hasMaterial()) m_scene.selectMaterial(getMaterialId());
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
    return m_scene.getDefaultPDUS();
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
  const MATERIAL &getMaterial() const {
    return getScene().getMaterial(getMaterialId());
  }
  inline bool hasMaterial() const {
    return getMaterialId() >= 0;
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

class D3SceneObjectWithVertexBuffer : public D3SceneObject {
private:
  void releaseVertexBuffer();
protected:
  int                    m_primitiveCount;
  DWORD                  m_fvf;
  int                    m_vertexSize;
  LPDIRECT3DVERTEXBUFFER m_vertexBuffer;
  template<typename VertexType> VertexType *allocateVertexArray(UINT count) {
    releaseVertexBuffer();
    UINT bufferSize;
    m_vertexBuffer = getScene().allocateVertexBuffer<VertexType>(count, &bufferSize);
    m_vertexSize = sizeof(VertexType);
    m_fvf = VertexType::FVF_Flags;
    assert(bufferSize == m_vertexSize * count);
    VertexType *bufferItems = NULL;
    lockVertexArray((void**)&bufferItems, bufferSize);
    return bufferItems;
  }

  void  lockVertexArray(void **a, UINT nbytes);
  void  unlockVertexArray();
  inline void setStreamSource() {
    getScene().setStreamSource(m_vertexBuffer, m_vertexSize, m_fvf);
  }

public:
  D3SceneObjectWithVertexBuffer(D3Scene &scene);
  ~D3SceneObjectWithVertexBuffer();

  inline LPDIRECT3DVERTEXBUFFER &getVertexBuffer() {
    return m_vertexBuffer;
  }
  inline bool hasVertexBuffer() const {
    return m_vertexBuffer != NULL;
  }
  D3DXCube3 getBoundingBox() const;
  String toString() const;
};

class D3SceneObjectWithIndexBuffer : public D3SceneObjectWithVertexBuffer {
private:
  void releaseIndexBuffer();
protected:
  LPDIRECT3DINDEXBUFFER m_indexBuffer;
  void *allocateIndexBuffer(bool int32, int count);
  void unlockIndexBuffer();

  inline void setIndices() {
    getScene().setIndices(m_indexBuffer);
  }

public:
  D3SceneObjectWithIndexBuffer(D3Scene &scene);
  ~D3SceneObjectWithIndexBuffer();
  inline bool hasIndexBuffer() const {
    return m_indexBuffer != NULL;
  }
  String toString() const;
};

class D3SceneObjectWithMesh : public D3SceneObject {
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

class D3SceneObjectSolidBox : public D3SceneObjectWithMesh {
private:
  int m_materialId;
  void makeSquareFace(MeshBuilder &mb, int v0, int v1, int v2, int v3);
  void init(const Vertex &p1, const Vertex &p2);
public:
  D3SceneObjectSolidBox::D3SceneObjectSolidBox(D3Scene &scene, const D3DXCube3 &cube, int materialId = 0)
    : D3SceneObjectWithMesh(scene)
    , m_materialId(materialId)
  {
    init(cube.getMin(), cube.getMax());
  }

  D3SceneObjectSolidBox::D3SceneObjectSolidBox(D3Scene &scene, const Vertex &p1, const Vertex &p2, int materialId = 0)
    : D3SceneObjectWithMesh(scene)
    , m_materialId(materialId)
  {
    init(p1, p2);
  }
  int getMaterialId() const {
    return m_materialId;
  }
  inline void setmaterialId(int materialId) {
    m_materialId = materialId;
  }
};

class D3SceneObjectSolidBoxWithPos : public D3SceneObjectSolidBox {
private:
  D3PosDirUpScale m_pdus;
public:
  D3SceneObjectSolidBoxWithPos(D3Scene &scene, const D3DXCube3 &cube, int materialId = 0)
    : D3SceneObjectSolidBox(scene, cube, materialId)
    , m_pdus(scene.getDefaultPDUS())
  {
  }
  D3PosDirUpScale &getPDUS() {
    return m_pdus;
  }
};

class D3SceneObjectLineArray : public D3SceneObjectWithVertexBuffer {
protected:
  void initBuffer(const Line3D *lines, UINT n);
public:
  D3SceneObjectLineArray(D3Scene &scene) : D3SceneObjectWithVertexBuffer(scene) {
  }
  D3SceneObjectLineArray(D3Scene &scene, const Line3D *lines, UINT n);
  D3SceneObjectLineArray(D3Scene &scene, const CompactArray<Line3D> &lineArray);
  void draw();
};

class D3SceneObjectWireFrameBox : public D3SceneObjectLineArray {
private:
  void init(const Vertex &p1, const Vertex &p2);
public:
  inline D3SceneObjectWireFrameBox(D3Scene &scene, const D3DXCube3 &cube)
    : D3SceneObjectLineArray(scene)
  {
    init(cube.getMin(), cube.getMax());
  }
  inline D3SceneObjectWireFrameBox(D3Scene &scene, const Vertex &p1, const Vertex &p2)
    : D3SceneObjectLineArray(scene)
  {
    init(p1,p2);
  }
};

class D3SceneObjectCurve : public D3SceneObjectWithVertexBuffer {
public:
  D3SceneObjectCurve(D3Scene &scene, const VertexArray &points);
  void draw();
};

class D3SceneObjectCurveArray : public D3SceneObjectWithVertexBuffer {
private:
  CompactIntArray m_curveSize;
public:
  D3SceneObjectCurveArray(D3Scene &scene, const CurveArray &curves);
  void draw();
};

typedef enum {
  ANIMATE_FORWARD
 ,ANIMATE_BACKWARD
 ,ANIMATE_ALTERNATING
} AnimationType;

class D3AnimatedSurface : public D3SceneObject, public TimeoutHandler {
private:
  MeshArray     m_meshArray;
  const UINT    m_frameCount;
  D3DFILLMODE   m_fillMode;
  D3DSHADEMODE  m_shadeMode;
  Timer         m_timer;
  float         m_sleepTime;
  AnimationType m_animationType;
  bool          m_forward;
  bool          m_running;
  int           m_nextMeshIndex, m_lastRenderedIndex;
  void nextIndex();
  int  getSleepTime() const;

public:
  D3AnimatedSurface(D3Scene &scene, const MeshArray &meshArray);
  ~D3AnimatedSurface();
  void startAnimation(AnimationType type = ANIMATE_FORWARD);
  void stopAnimation();
  void handleTimeout(Timer &timer);
  inline bool isRunning() const {
    return m_running;
  }
  inline AnimationType getAnimationType() const {
    return m_animationType;
  }
  // sleepTime /= factor
  void scaleSpeed(float factor);
  inline float getFramePerSec() const {
    return m_running ? 1000.0f / m_sleepTime : 0;
  }
  LPD3DXMESH getMesh() const;
  void draw();
  bool hasFillMode() const {
    return true;
  }
  void setFillMode(D3DFILLMODE fillMode) {
    m_fillMode = fillMode;
  }
  D3DFILLMODE getFillMode() const {
    return m_fillMode;
  }
  bool hasShadeMode() const {
    return true;
  }
  void setShadeMode(D3DSHADEMODE shadeMode) {
    m_shadeMode = shadeMode;
  }
  D3DSHADEMODE getShadeMode() const {
    return m_shadeMode;
  }
  SceneObjectType getType() const {
    return SOTYPE_ANIMATEDOBJECT;
  }
};

class D3SceneObjectLineArrow : public D3SceneObjectWithVertexBuffer {
public:
  D3SceneObjectLineArrow(D3Scene &scene, const Vertex &from, const Vertex &to);
  void draw();
};

class D3PickRayArrow : public D3SceneObjectLineArrow {
private:
  D3PosDirUpScale m_pdus;
public:
  D3PickRayArrow(D3Scene &scene, const D3Ray &ray)
    : D3SceneObjectLineArrow(scene, ray.m_orig, ray.m_orig + 2 * ray.m_dir)
    , m_pdus(scene.getRightHanded())
  {
    D3DXMATRIX m;
    m_pdus.setWorldMatrix(*D3DXMatrixIdentity(&m));
  }
  D3PosDirUpScale &getPDUS() {
    return m_pdus;
  }
};
