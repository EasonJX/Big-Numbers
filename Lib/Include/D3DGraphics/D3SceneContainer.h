#pragma once

#include <MFCUtil/WinTools.h>
#include "D3CameraArray.h"

class D3Scene;

class WindowPair {
private:
  HWND m_hwnd[2];
public:
  WindowPair() {
    clear();
  }
  WindowPair(HWND w1, HWND w2) {
    m_hwnd[0] = w1;
    m_hwnd[1] = w2;
  }
  inline void clear() {
    m_hwnd[0] = (HWND)INVALID_HANDLE_VALUE;
    m_hwnd[1] = (HWND)INVALID_HANDLE_VALUE;
  }
  inline bool isEmpty() const {
    return !IsWindow(m_hwnd[0]);
  }
  inline HWND getWindow(UINT index) const {
    return m_hwnd[index];
  }
};

#define SC_RENDER3D           0x0001
#define SC_RENDERINFO         0x0002
#define SC_RENDERNOW          0x0004
#define SC_RENDERALL          (SC_RENDER3D | SC_RENDERINFO)

class D3SceneContainer {
private:
  BYTE      m_renderLevel;
  CameraSet m_accumulatedCameraSet;
  BYTE      m_accumulatedRenderFlags;

public:
  D3SceneContainer() : m_renderLevel(0), m_accumulatedRenderFlags(0) {
  }
  virtual D3Scene   &getScene() = 0;
  virtual UINT       get3DWindowCount() const = 0;
  virtual HWND       getMessageWindow() const = 0;
  virtual HWND       get3DWindow(UINT index) const = 0;
  virtual void       modifyContextMenu(HMENU menu) {
  }
  virtual bool       canSplit3DWindow(HWND hwnd) const {
    return false;
  }
  virtual WindowPair split3DWindow(HWND current, bool vertical) {
    return WindowPair();
  }
  virtual bool       canDelete3DWindow(HWND hwnd) const {
    return false;
  }
  virtual bool       delete3DWindow(HWND hwnd) {
    return false;
  }
  inline CSize       getWinSize(UINT index) const {
    return getClientRect(get3DWindow(index)).Size();
  }
  inline void        incrLevel() {
    m_renderLevel++;
  }
  void               decrLevel();
  // renderFlags is any combination of SC_RENDER*
  void               render(  BYTE renderFlags, CameraSet cameraSet);
  virtual void       doRender(BYTE renderFlags, CameraSet cameraSet);
};
