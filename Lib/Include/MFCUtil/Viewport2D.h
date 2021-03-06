#pragma once

#include <Math/Transformation.h>
#include "WinTools.h"
#include "Point2DP.h"

class Viewport2D {
private:
  RectangleTransformation *m_tr;
  bool                     m_ownTransformation;
  bool                     m_retainAspectRatio;

  mutable CDC             *m_dc;
  mutable CRgn             m_clipRgn;
  mutable CPen             m_currentPen;
  mutable int              m_currentPenStyle;
  mutable int              m_currentPenWidth;
  mutable COLORREF         m_currentPenColor;

  CPen &setCurrentPen(int penStyle, int width, COLORREF color) const;
  inline void paintLine(int x1, int y1, int x2, int y2) const {
    m_dc->MoveTo(x1, y1); m_dc->LineTo(x2, y2);
  }
  void destroyClipRgn() const;
  void destroyTransformation();
  void destroyPen() const;
  inline bool hasDC() const {
    return m_dc && m_dc->m_hDC;
  }
#if defined(_DEBUG)
  void noDCError(const TCHAR *method) const;
#define CHECKHASDC() { if(!hasDC()) noDCError(__TFUNCTION__); }
#else
#define CHECKHASDC()
#endif  // _DEBUG

public:
  Viewport2D(bool retainAspectRatio = false);
  Viewport2D(CDC &dc, const Rectangle2DR &from, const Rectangle2DR &to, bool retainAspectRatio = false);
  Viewport2D(CDC &dc, RectangleTransformation &tr,  bool retainAspectRatio = false);
  ~Viewport2D();

  inline CDC *getDC() {
    return m_dc;
  }

  inline CDC *setDC(CDC *dc) const {
    CDC *old = m_dc; m_dc = dc; return old;
  }

  inline const RectangleTransformation &getTransformation() const {
    return *m_tr;
  }
  inline RectangleTransformation &getTransformation() {
    return *m_tr;
  }

  inline Rectangle2DR getFromRectangle() const {
    return m_tr->getFromRectangle();
  }

  inline Rectangle2DR getToRectangle() const {
    return m_tr->getToRectangle();
  }

  inline void setFromRectangle(const Rectangle2DR &rect) {
    m_tr->setFromRectangle(rect);
  }

  void setToRectangle(const Rectangle2DR &rect);

  inline void setScale(IntervalScale newScale, int flags) {
    m_tr->setScale(newScale, flags);
  }

  void setRetainAspectRatio(bool retainAspectRatio);

  inline bool isRetainingAspectRatio() const {
    return m_retainAspectRatio;
  }

  inline const IntervalTransformation &getXTransformation() const {
    return m_tr->getXTransformation();
  }

  inline const IntervalTransformation &getYTransformation() const {
    return m_tr->getYTransformation();
  }

  inline Point2DP forwardTransform(const Point2DP &p) const {
    return m_tr->forwardTransform(p);
  }

  inline Point2DP forwardTransform(double x, double y) const {
    return m_tr->forwardTransform(x,y);
  }

  inline Point2DP backwardTransform(const Point2DP &p) const {
    return m_tr->backwardTransform(p);
  }

  inline Rectangle2DR forwardTransform(const Rectangle2DR &r) const {
    return m_tr->forwardTransform(r);
  }

  inline Rectangle2DR backwardTransform(const Rectangle2DR &r) const {
    return m_tr->backwardTransform(r);
  }

  void        setClipping(bool         clip) const;
  void        MoveTo(    const Point2DP  &p) const;
  void        MoveTo(    double x, double y) const;
  void        LineTo(    const Point2DP  &p) const;
  void        LineTo(    double x, double y) const;
  void        SetPixel(  const Point2DP  &p, COLORREF color) const;
  void        SetPixel(  double x, double y, COLORREF color) const;
  void        paintCross(const Point2DP  &p, COLORREF color, int size=4) const;
  COLORREF    GetPixel(  const Point2DP  &p) const;
  COLORREF    GetPixel(  double x, double y) const;
  bool        Rectangle( const Rectangle2DR &r) const;
  bool        Rectangle( double x1, double y1, double x2, double y2) const;
  void        FillSolidRect(const Rectangle2DR &r, COLORREF color) const;
  // transparent background
  // If bckSave is specified, the bounding recangle of the written text is returned (can be used for selection)
  // and the original pixels in the bounding rectangle containing the text, will be saved, for later restore
  void        TextOut( const Point2DP &p, const String &text, COLORREF color, BackgroundSaver *bckSave = NULL) const;
  void        clear(COLORREF color) const;
  CGdiObject *SelectObject(CGdiObject *object) const;
  CBitmap    *SelectObject(CBitmap    *bitmap) const;
  CPen       *SelectObject(CPen       *pen   ) const;
  CFont      *SelectObject(CFont      *font  ) const;

  CGdiObject *SelectStockObject(   int index ) const;
  void paintDragRect(const Rectangle2D &rect, SIZE size, const Rectangle2D &lastRect, SIZE lastSize, CBrush *brush = NULL, CBrush *lastBrush = NULL) const;
};
