#pragma once

#include "ShowGrafView.h"
#include "ShowGrafDoc.h"

class CMainFrame : public CFrameWnd {
private:
  HACCEL                      m_accelTable;
  FunctionGraphParameters     m_functionParam;
  ParametricGraphParameters   m_parametricCurveParam;
  IsoCurveGraphParameters     m_isoCurveParam;
  DiffEquationGraphParameters m_diffEqParam;
  CCustomFitThread           *m_fitThread;

  void onFileMruFile(int index);
  void initMenuItems();
protected:
  DECLARE_DYNCREATE(CMainFrame)
public:
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
  virtual BOOL PreTranslateMessage(MSG *pMsg);

  CMainFrame();
  virtual ~CMainFrame();
  void showPositon(const CPoint &p);
  void activateInitialOptions();
  void              setRollAvg(bool on);  
  void              setGrid(bool grid);
  bool              hasGrid() const;
  void              setXAxisType(  AxisType          type    );
  void              setYAxisType(  AxisType          type    );
  AxisType          getXAxisType() const;
  AxisType          getYAxisType() const;
  void              setGraphStyle( GraphStyle        newStyle);
  GraphStyle        getGraphStyle() const;
  void              setTrigoMode(TrigonometricMode mode    );
  TrigonometricMode getTrigoMode() const;
  void              setIgnoreErrors(bool ignore);
  bool              getIgnoreErrors();
  void startCustomFitThread();
  void startPolynomialFitThread();
  void stopFitThread();
  void makeExpoFit();
  void makePotensFit();

  CShowGrafView *getView() {
    return (CShowGrafView*)GetActiveView();
  }
  CShowGrafDoc *getDoc() {
    return (CShowGrafDoc*)GetActiveDocument();
  }
  FunctionGraphParameters     &getFunctionParam() {
    return m_functionParam;
  }
  ParametricGraphParameters   &getParametricCurveParam() {
    return m_parametricCurveParam;
  }
  IsoCurveGraphParameters     &getIsoCurveParam() {
    return m_isoCurveParam;
  }
  DiffEquationGraphParameters &getDiffEquationParam() {
    return m_diffEqParam;
  }

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  CToolBar    m_wndToolBar;
  CStatusBar  m_wndStatusBar;

  afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnFileNew();
  afx_msg void OnFileOpen();
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
  afx_msg void OnViewGrid();
  afx_msg void OnViewScaleXLinear();
  afx_msg void OnViewScaleXLogarithmic();
  afx_msg void OnViewScaleXNormaldistribution();
  afx_msg void OnViewScaleYLinear();
  afx_msg void OnViewScaleYLogarithmic();
  afx_msg void OnViewScaleYNormaldistribution();
  afx_msg void OnViewScaleYDatetime();
  afx_msg void OnViewScaleXDatetime();
  afx_msg void OnViewScaleResetScale();
  afx_msg void OnViewStyleCurve();
  afx_msg void OnViewStylePoint();
  afx_msg void OnViewStyleCross();
  afx_msg void OnViewInterval();
  afx_msg void OnViewRetainAspectRatio();
  afx_msg void OnViewRollAvg();
  afx_msg void OnViewSetRollAvgSize();
  afx_msg void OnViewRefreshFiles();
  afx_msg void OnToolsFitPolynomial();
  afx_msg void OnToolsFitExpoFunction();
  afx_msg void OnToolsFitPotensFunction();
  afx_msg void OnToolsFitCustomFunction();
  afx_msg void OnToolsPlotFunction();
  afx_msg void OnToolsParametricCurve();
  afx_msg void OnToolsImplicitDefinedCurve();
  afx_msg void OnToolsDifferentialEquations();
  afx_msg void OnOptionsIgnoreErrors();
  afx_msg void OnOptionsRadians();
  afx_msg void OnOptionsDegrees();
  afx_msg void OnOptionsGrads();
  afx_msg void OnSelectMenuDelete();
  afx_msg void OnSelectMenuEdit();
  afx_msg void OnSelectMenuHide();
  afx_msg void OnSelectMenuShow();
  afx_msg void OnSelectMenuStyleCurve();
  afx_msg void OnSelectMenuStylePoint();
  afx_msg void OnSelectMenuStyleCross();

  DECLARE_MESSAGE_MAP()
};
