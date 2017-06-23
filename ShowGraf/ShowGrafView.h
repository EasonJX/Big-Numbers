#pragma once

#include <MFCUtil/Coordinatesystem/CoordinateSystem.h>
#include "ShowGrafDoc.h"

class CShowGrafView : public CFormView, public FunctionPlotter {
private:
    enum { IDD = IDD_SHOWGRAFVIEW };

    CCoordinateSystem           m_coordinateSystem;
    bool                        m_firstDraw;
    CFont                       m_axisFont;
    CFont                       m_buttonFont;
    bool                        m_dragging;
    RectangleTransformation     m_mouseDownTransform;
    CPoint                      m_mouseDownPoint;

    void plotFunction(Function &f, const DoubleInterval &interval, COLORREF color);
    void plotFunction(Function &f, COLORREF color);
    bool paintAll(CDC &dc, const CRect &rect, CFont *axisFont, CFont *buttonFont);
    bool isMenuItemChecked(int id);
    void enableMenuItem(int id, bool enabled);
    void checkMenuItem( int id, bool checked);
    CShowGrafDoc *getDoc();
    void startDragging(const CPoint &point);
    void stopDragging();
protected: // create from serialization only
  DECLARE_DYNCREATE(CShowGrafView)

  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnSize(       UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

public:
    CShowGrafView();
    virtual ~CShowGrafView();
    void initScale();
    void clear();

    CCoordinateSystem &getCoordinateSystem() {
      return m_coordinateSystem;
    }
    void setAxisColor(COLORREF color) {
      getCoordinateSystem().setAxisColor(color);
    }
    void setBackgroundColor(COLORREF color) {
      getCoordinateSystem().setBackGroundColor(color);
    }
    void setRetainAspectRatio(bool retain) {
      m_coordinateSystem.setRetainAspectRatio(retain);
    }
    RectangleTransformation &getTransformation() {
      return getCoordinateSystem().getTransformation();
    }
    DataRange getDataRange() {
      return m_coordinateSystem.getDataRange();
    }
    void setDataRange(const DataRange &dr, bool makeSpace) {
      getCoordinateSystem().setDataRange(dr, makeSpace);
    }
    DoubleInterval getXInterval() {
      return getDataRange().getXInterval();
    }
    DoubleInterval getYInterval() {
      return getDataRange().getYInterval();
    }
    void setXAxisType(AxisType type) {
      getCoordinateSystem().setXAxisType(type);
    }
    void setYAxisType(AxisType type) {
      getCoordinateSystem().setYAxisType(type);
    }
    void addFunctionGraph(FunctionGraphParameters &param);

    virtual void OnDraw(CDC *pDC);  // overridden to draw this view
    virtual BOOL OnPreparePrinting(          CPrintInfo *pInfo);
    virtual void OnBeginPrinting(  CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(    CDC *pDC, CPrintInfo *pInfo);

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif
};
