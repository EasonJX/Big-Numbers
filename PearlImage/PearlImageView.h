#pragma once

#include "PearlImageDoc.h"
#include "EdgeMark.h"

class CMainFrame;

class CPearlImageView : public CScrollView {
private:
  bool             m_initialized;
  int              m_currentZoomFactor;
  // last point in document coordinates
  CPoint           m_currentDocPoint;
  CPoint           m_maxScroll;
   CPrintInfo     *m_printInfo;
  EdgeMarkArray    m_edgeMark;
  const EdgeMark  *m_currentEdgeMark;
  CRect            m_dragRect,*m_lastDragRect;

  inline CMainFrame *getMainFrame() {
    return (CMainFrame*)GetParent();
  }

  inline const CMainFrame *getMainFrame() const {
    return (const CMainFrame*)GetParent();
  }

  void         paintBackgroundAndEdge(CDC &dc);
  void         setScrollRange();
  inline const EdgeMark *findEdgeMark(const CPoint &viewp) const {
    return m_edgeMark.findEdgeMark(viewp);
  }
  inline void  setCursor(int id) {
    setWindowCursor(this,MAKEINTRESOURCE(id));
  }

  void         resetResizingFrame();
  void         paintResizingFrame(const CPoint &docp);
  void         resizeDocument();
  inline CSize getDocSize() const {
    return GetDocument()->getSize();
  }
  inline CSize getZoomedDocSize() const {
    const CSize ds = getDocSize();
    return CSize(ds.cx*m_currentZoomFactor, ds.cy*m_currentZoomFactor);
  }
  inline CRect getDocumentRect() const {
   return CRect(ORIGIN, getDocSize());
  }
  inline CRect getViewRect() const {
    return CRect(ORIGIN,docToView(getDocSize()));
  }
  CPoint       viewToDoc(const CPoint &viewPoint) const;
  CPoint       docToView(const CPoint &docPoint)  const;
  // coordinates of upper-left corner of visible part image in
  inline CPoint getViewTopLeft() const {
    return GetScrollPosition();
  }
  inline CPoint getDocTopLeft() const {
    CPoint tp = getViewTopLeft();
    tp.x /= getCurrentZoomFactor(); tp.y /= getCurrentZoomFactor();
    return tp;
  }
protected:
  CPearlImageView();
  DECLARE_DYNCREATE(CPearlImageView)
public:
    CPearlImageDoc *GetDocument();
    inline const CPearlImageDoc *GetDocument() const {
       return (const CPearlImageDoc*)m_pDocument;
    }
    const PixRect *getImage();

    void       repaint();
    void       refreshDoc();
    void       clear();
    void       setCurrentZoomFactor(int factor);
    inline int getCurrentZoomFactor() const {
      return m_currentZoomFactor;
    }

    inline CPoint   getCurrentDocPoint() const {
      return m_currentDocPoint;
    }
    void       setCurrentDocPoint(const CPoint &p);

    inline COLORREF getBackgroundColor() const {
      return RGB(207,217,232);
    }
    inline bool         isMouseOnDocument() const {
      return getDocumentRect().PtInRect(m_currentDocPoint) ? true : false;
    }

    // in zoomed Doc
    inline const CPoint &getMaxScroll() const {
      return m_maxScroll;
    }

    virtual void OnDraw(CDC *pDC);  // overridden to draw this view
    virtual void OnInitialUpdate();
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
    virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
#if defined(_DEBUG)
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    DECLARE_MESSAGE_MAP()
};

#if !defined(_DEBUG  )// debug version in PearlImageView.cpp
inline CPearlImageDoc* CPearlImageView::GetDocument()
   { return (CPearlImageDoc*)m_pDocument; }
#endif
