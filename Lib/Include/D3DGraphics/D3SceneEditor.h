#pragma once

#include <TinyBitSet.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/PropertyDialogMap.h>
#include <D3DGraphics/Light.h>
#include <D3DGraphics/Material.h>
#include <D3DGraphics/D3LightControl.h>

typedef enum {
  CONTROL_IDLE
 ,CONTROL_CAMERA_WALK
 ,CONTROL_OBJECT_POS
 ,CONTROL_OBJECT_SCALE
 ,CONTROL_LIGHT
 ,CONTROL_SPOTLIGHTPOINT
 ,CONTROL_SPOTLIGHTANGLES
 ,CONTROL_ANIMATION_SPEED
 ,CONTROL_MATERIAL
 ,CONTROL_LIGHTCOLOR
 ,CONTROL_BACKGROUNDCOLOR
 ,CONTROL_AMBIENTLIGHTCOLOR
} CurrentObjectControl;

#define RENDER_3D   0x1
#define RENDER_INFO 0x2
#define RENDER_ALL  (RENDER_3D|RENDER_INFO)

typedef enum {
  SE_ENABLED
 ,SE_HANDLEPROPERTYCHANGES
 ,SE_RENDERENABLED
 ,SE_MOUSEVISIBLE
} StateFlags;

class D3SceneContainer {
public:
  virtual D3Scene &getScene()               = 0;
  virtual CWnd    *getMessageWindow()       = 0;
  virtual CWnd    *get3DWindow()            = 0;
  // renderFlags is any combination of RENDER_3D,RENDER_INFO
  virtual void     render(BYTE renderFlags) = 0;
  virtual void     modifyContextMenu(CMenu &menu) {
  }
};

class CenterOfRotation {
public:
  D3SceneObject *m_obj; // which object does m_pos refer to
  D3DXVECTOR3    m_pos; // relative to mesh (0,0,0)
  CenterOfRotation() {
    reset();
  }
  inline void reset() {
    set(NULL,D3DXORIGIN);
  }
  inline void set(D3SceneObject *obj, const D3DXVECTOR3 &pos) {
    m_obj = obj;
    m_pos = pos;
  }
};

class D3SceneEditor : public PropertyChangeListener {
private:
    D3SceneContainer                 *m_sceneContainer;
    CurrentObjectControl              m_currentControl;
    D3SceneObject                    *m_currentSceneObject, *m_coordinateSystem;
    PropertyDialogMap                 m_propertyDialogMap;
    PropertyContainer                *m_currentEditor;
    BitSet8                           m_stateFlags;
    CPoint                            m_lastMouse;
    CenterOfRotation                  m_centerOfRotation;
    D3DXVECTOR3                       m_pickedPoint; // in world space
    D3Ray                             m_pickedRay;   // in world space
    D3PickedInfo                      m_pickedInfo;
    String                            m_paramFileName;

    inline CWnd *getMessageWindow() const {
      return m_sceneContainer->getMessageWindow();
    }
    inline CWnd *get3DWindow() const {
      return m_sceneContainer->get3DWindow();
    }
    D3Scene &getScene() const {
      return m_sceneContainer->getScene();
    }
    inline CPoint screenPTo3DP(CPoint p) const {
      get3DWindow()->ScreenToClient(&p);
      return p;
    }
    inline void render(BYTE flags) {
      if(isRenderEnabled()) m_sceneContainer->render(flags);
    }

    void rotateCurrentVisualFrwBckw(  double angle1 , double angle2);
    void rotateCurrentVisualLeftRight(double angle) ;
    void adjustCurrentVisualScale(int component, double factor);

    void moveCurrentSceneObjectXY(CPoint pt);
    void moveCurrentSceneObjectXZ(CPoint pt);

    D3DXVECTOR3 getCurrentSceneObjectPos();
    void        setCurrentSceneObjectPos(   const D3DXVECTOR3 &pos);
    void        setCurrentVisualOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up);
    void        setCurrentVisualScale(      const D3DXVECTOR3 &pos);
    void        resetCenterOfRotation();
    void        setCenterOfRotation();
    inline D3DXVECTOR3  getCenterOfRotation() const {
      return (getCurrentVisual() == m_centerOfRotation.m_obj) ? m_centerOfRotation.m_pos : D3DXORIGIN;
    }
    void walkWithCamera(       double dist   , double angle);
    void sidewalkWithCamera(   double upDist , double rightDist);
    void rotateCameraUpDown(   double angle) ;
    void rotateCameraLeftRight(double angle);

    void setMouseVisible(bool visible);
    inline bool isMouseVisible() const {
      return m_stateFlags.contains(SE_MOUSEVISIBLE);
    }
    inline void enableRender() {
      m_stateFlags.add(SE_RENDERENABLED);
    }
    inline void disableRender() {
      m_stateFlags.remove(SE_RENDERENABLED);
    }
    inline bool isRenderEnabled() const {
      return m_stateFlags.contains(SE_RENDERENABLED);
    }
    inline void enablePropertyChanges() {
      m_stateFlags.add(SE_HANDLEPROPERTYCHANGES);
    }
    inline void disablePropertyChanges() {
      m_stateFlags.remove(SE_HANDLEPROPERTYCHANGES);
    }
    inline bool isPropertyChangesEnabled() const {
      return m_stateFlags.contains(SE_HANDLEPROPERTYCHANGES);
    }
    CMenu &loadMenu(CMenu &menu, int id);
    void showContextMenu(CMenu &menu, CPoint point);

    void setCurrentControl(CurrentObjectControl control);

          D3LightControl *getCurrentLightControl();
    const D3LightControl *getCurrentLightControl() const;
    void setLightControlRenderEffect(bool enabled);
    void setSpecularEnable(          bool enabled);
    void OnSaveSceneParameters();
    void OnLoadSceneParameters();
    void OnControlObjectMoveRotate();
    void OnControlObjectPos();
    void OnControlObjectScale();

    void setLightControlsVisible(bool visible);
    void addLight(D3DLIGHTTYPE type);
    void setSpotToPointAt(CPoint point);
    void OnMouseMoveCameraWalk(           UINT nFlags, CPoint pt);
    void OnMouseMoveObjectPos(            UINT nFlags, CPoint pt);
    void OnMouseWheelObjectPos(           UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelObjectScale(         UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelAnimationSpeed(      UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelCameraWalk(          UINT nFlags, short zDelta, CPoint pt);

    void OnMouseMoveLight(                UINT nFlags, CPoint pt);
    void OnMouseMoveLightPoint(           UINT nFlags, CPoint pt);
    void OnMouseMoveLightSpot(            UINT nFlags, CPoint pt);
    void OnMouseWheelLight(               UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightPoint(          UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightDirectional(    UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightSpot(           UINT nFlags, short zDelta, CPoint pt);
    void OnMouseWheelLightSpotAngle(      UINT nFlags, short zDelta, CPoint pt);

    void OnContextMenuBackground(  CPoint point);
    void OnContextMenuSceneObject( CPoint point);
    void OnContextMenuVisualObject(CPoint point);
    void OnContextMenuLightControl(CPoint point);
public:
    void OnControlCameraWalk();
private:
    void OnObjectStartAnimation();
    void OnObjectStartBckAnimation();
    void OnObjectStartAltAnimation();
    void OnObjectReverseAnimation();
    void OnObjectStopAnimation();
    void OnObjectControlSpeed();
    void OnObjectEditMaterial();
    void OnObjectResetScale();
    void OnObjectRemove();
    void OnObjectSetCenterOfRotation();
    void OnLightAdjustColors();
    void OnLightAdjustSpotAngles();
    void OnLightControlSpotAt();
    void OnLightControlHide();
    void OnAddLightDirectional();
    void OnAddLightPoint();
    void OnAddLightSpot();
    void OnEditAmbientLight();
    void OnEditBackgroundColor();
    void OnFillmodePoint();
    void OnFillmodeWireframe();
    void OnFillmodeSolid();
    void OnShadingFlat();
    void OnShadingGouraud();
    void OnShadingPhong();
    void setCoordinateSystemVisible(bool visible);
    void setLightEnabled(bool enabled);
    void OnLightRemove();
    void OnLButtonDown(  UINT nFlags, CPoint point);
    void OnLButtonUp(    UINT nFlags, CPoint point);
    void OnMouseMove(    UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    BOOL OnMouseWheel(   UINT nFlags, short zDelta, CPoint pt);
    void OnContextMenu(  CWnd *pwnd, CPoint point);
    String stateFlagsToString() const;
    String getSelectedString() const;
public:
    D3SceneEditor();
    ~D3SceneEditor();
    void init(D3SceneContainer *sceneContainer);
    void close();
    void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
    void setEnabled(bool enabled);
    inline bool isEnabled() const {
      return m_stateFlags.contains(SE_ENABLED);
    }
    void                  setCurrentSceneObject(D3SceneObject *obj);

    inline D3SceneObject *getCurrentSceneObject() const {
      return m_currentSceneObject;
    }
    // Return NULL, if m_currentSceneObject->type not in {SOTYPE_VISUALOBJECT, SOTYPE_ANIMATEDOBJECT, }
    D3SceneObject     *getCurrentVisual() const;
    // return NULL, if m_currentSceneObject->type not SOTYPE_ANIMATEDOBJECT
    D3AnimatedSurface *getCurrentAnimatedObject() const;
    inline const D3DXVECTOR3 &getPickedPoint() const {
      return m_pickedPoint;
    }
    inline bool hasCurrentSceneObject() const {
      return getCurrentSceneObject() != NULL;
    }
    inline bool hasCurrentVisual() const {
      return getCurrentVisual() != NULL;
    }
    inline bool hasCurrentAnimatedObject() const {
      return getCurrentAnimatedObject() != NULL;
    }
    inline const D3Ray &getPickedRay() const {
      return m_pickedRay;
    }
    inline const D3PickedInfo &getPickedInfo() const {
      return m_pickedInfo;
    }
    inline bool isCoordinateSystemVisible() const {
      return m_coordinateSystem && (m_coordinateSystem->isVisible());
    }
    BOOL PreTranslateMessage(MSG *pMsg);
    String toString() const;
};
