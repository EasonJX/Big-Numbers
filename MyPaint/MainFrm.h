#pragma once

#include "DrawTool.h"
#include "MyPaintView.h"
#include <Stack.h>

class CMainFrame : public CFrameWnd {
private:
  HACCEL           m_accelTable;
  double           m_currentDegree;
  ScaleParameters  m_currentScale;
  Stack<DrawTool*> m_toolStack;
  D3DCOLOR         m_currentColor;
  FontParameters   m_currentFontParameters;
  String           m_currentText;
  UINT             m_approximateFillTolerance;
  CSize            m_eraseToolSize;

  void onFileMruFile(int index);
  void setCurrentDrawTool(int id);
  void setCurrentDrawTool(DrawTool *newDrawTool);
  void checkToolItem(int id);
  void ajourRedoUndo();
  bool save(const CString &name);
  bool onFileSave();
  bool checkSave();
  CMyPaintView *getView() {
    return (CMyPaintView*)GetActiveView();
  }

  CMyPaintDoc *getDocument();
  void scroll(int dx, int dy);
  CPoint getMaxScroll();
  void applyFilter(PixRectFilter &filter);
  void applyMirror(bool vertical);
protected:
  CMainFrame();
  CStatusBar  m_wndStatusBar;
  CToolBar    m_wndToolBar;
  DECLARE_DYNCREATE(CMainFrame)

public:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual ~CMainFrame();

  bool loadFile(const String &fileName);
  void setCurrentZoomFactor(int id);
  void saveDocState();
  void pushTool(DrawTool *tool);
  void popTool();
  void updateTitle();

  DrawTool *getCurrentDrawTool() {
    return m_toolStack.top();
  }

  bool hasDrawTool() const {
    return !m_toolStack.isEmpty();
  }

  void resetCurrentDrawTool();

  D3DCOLOR getCurrentColor() const {
    return m_currentColor;
  }

  int getApproximateFillTolerance() const {
    return m_approximateFillTolerance;
  }

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileInsert();
    afx_msg void OnAppExit();
    afx_msg void OnFileMruFile1();
    afx_msg void OnFileMruFile2();
    afx_msg void OnFileMruFile3();
    afx_msg void OnFileMruFile4();
    afx_msg void OnFileMruFile5();
    afx_msg void OnFileMruFile6();
    afx_msg void OnFileMruFile7();
    afx_msg void OnFileMruFile8();
    afx_msg void OnFileMruFile9();
    afx_msg void OnFileMruFile10();
    afx_msg void OnFileMruFile11();
    afx_msg void OnFileMruFile12();
    afx_msg void OnFileMruFile13();
    afx_msg void OnFileMruFile14();
    afx_msg void OnFileMruFile15();
    afx_msg void OnFileMruFile16();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditUndo();
    afx_msg void OnEditRedo();
    afx_msg void OnEditCut();
    afx_msg void OnEditCopy();
    afx_msg void OnEditPaste();
    afx_msg void OnOptionsColor();
    afx_msg void OnOptionsZoomX1();
    afx_msg void OnOptionsZoomX2();
    afx_msg void OnOptionsZoomX4();
    afx_msg void OnOptionsZoomX8();
    afx_msg void OnOptionsApproximateFillTolerance();
    afx_msg void OnOptionsGrabColor();
    afx_msg void OnToolsPen();
    afx_msg void OnToolsLine();
    afx_msg void OnToolsRectangle();
    afx_msg void OnToolsPolygon();
    afx_msg void OnToolsEllipse();
    afx_msg void OnToolsText();
    afx_msg void OnToolsDrawtext();
    afx_msg void OnToolsFill();
    afx_msg void OnToolsFillTransparent();
    afx_msg void OnToolsApproximateFill();
    afx_msg void OnToolsChangeHue();
    afx_msg void OnToolsMoveRectangle();
    afx_msg void OnToolsMoveRegion();
    afx_msg void OnToolsErase();
    afx_msg void OnFunctionRotate();
    afx_msg void OnFunctionScale();
    afx_msg void OnFunctionMirrorHorizontal();
    afx_msg void OnFunctionMirrorVertical();
    afx_msg void OnFunctionMakegrayscale();
    afx_msg void OnFunctionChangeHue();
    afx_msg void OnFunctionSobelEdgeDetection();
    afx_msg void OnFunctionLapaceEdgeDetection();
    afx_msg void OnFunctionCannyEdgeDetection();
    afx_msg void OnFunctionDirectionalFilter();
    afx_msg void OnFunctionGaussFilter();
    afx_msg void OnScrollLineDown();
    afx_msg void OnScrollLineUp();
    afx_msg void OnScrollPageDown();
    afx_msg void OnScrollPageUp();
    afx_msg void OnScrollLeft();
    afx_msg void OnScrollRight();
    afx_msg void OnScrollPageLeft();
    afx_msg void OnScrollPageRight();
    afx_msg void OnScrollToTop();
    afx_msg void OnScrollToBottom();
    afx_msg void OnScrollToLeft();
    afx_msg void OnScrollToRight();
    afx_msg void OnPopTool();
    afx_msg void OnDelete();
    DECLARE_MESSAGE_MAP()
};
