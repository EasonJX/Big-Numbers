#pragma once

#include <Stack.h>
#include <MFCUtil/Layoutmanager.h>
#include "MBCalculator.h"
#include "ColorMapDlg.h"
#include "ImageListThread.h"

typedef enum {
  STATE_IDLE
 ,STATE_DRAGGING
 ,STATE_MOVING
} DlgState;

class FractalImage {
public:
  RealRectangle2D m_scale;
  PixRect        *m_pixRect;
  FractalImage(const RealRectangle2D &scale, PixRect *pixRect)
  : m_scale(scale)
  , m_pixRect(pixRect)
  {
  }
};

typedef enum {
  NW = 0
 ,SW = 1
 ,NE = 2
 ,SE = 3
} MoveDirection;

class CMandelbrotDlg;

class ExpTransformation {
private:
  Real m_a, m_b;
  RealInterval m_toInterval;
public:
  ExpTransformation(const RealInterval &from, const RealInterval &to);
  Real transform(const Real &x) const {
    return m_b*pow(m_a,x);
  }
  const RealInterval &getToInterval() const {
    return m_toInterval;
  }
};

class MBFrameGenerator : public FrameGenerator {
private:
  CMandelbrotDlg            &m_dlg;
  const String               m_dirName;
  // in complex plane
  const RealRectangle2D      m_finalRect;
  RealRectangle2D            m_startRect;
  ExpTransformation         *m_expTransform;
  RealLinearTransformation  *m_linearTransform;
  // in pixels
  const CSize                m_imageSize;
  ImageListThread           *m_imageListThread;
  Semaphore                  m_frameReady;
  int                        m_totalFrameCount;
  int                        m_frameIndex;
  HBITMAP                    m_bm;
  HDC                        m_dc;

  static int findTotalFrameCount(const RealRectangle2D &startRect, const RealRectangle2D &finalRect);
  void postMovieDone();
  // Return false if done
  bool requestNextFrame();
  RealRectangle2D getInterpolatedRectangle() const;
public:
  MBFrameGenerator(CMandelbrotDlg *dlg, const String &dirName);
  ~MBFrameGenerator();
  String getDirName() {
    return m_dirName;
  }
  CSize getFrameSize() {
    return m_imageSize;
  }
  void notifyFrameReady() {
    DLOG(_T("notify FrameReady\n"));
    m_frameReady.signal();
  }
  int getTotalFrameCount() const {
    return m_totalFrameCount;
  }
  int getFrameIndex() const {
    return m_frameIndex;
  }
  // should return NULL when no more frames.
  HBITMAP nextBitmap();
};

class CMandelbrotDlg;

class DialogMBContainer : public MBContainer {
private:
  CMandelbrotDlg *m_dlg;
public:
  DialogMBContainer(CMandelbrotDlg *dlg, PixRect *pr) : MBContainer(pr), m_dlg(dlg) {
  }

  const RealRectangleTransformation &getTransformation()       const;
  UINT                               getMaxIteration()         const;
  const D3DCOLOR                    *getColorMap()             const;
  FPUPrecisionMode                   getPrecisionMode()        const;
  PixelAccessor                     *getPixelAccessor();
  bool                               calculateWithOrbit()      const;
  bool                               useEdgeDetection()        const;
  bool                               getJobToDo(CRect &rect);
  void                               paintMark(const CPoint &p);
  void                               setPixel(UINT x, UINT y, D3DCOLOR color);
  D3DCOLOR                           getPixel(UINT x, UINT y) const;
  void                               handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
};

class CMandelbrotDlg : public CDialog, public PropertyChangeListener {
private:
    static const TCHAR         *s_stateName[];
    HICON                       m_hIcon;
    HACCEL                      m_accelTable;
    HICON                       m_crossIcon;
    SimpleLayoutManager         m_layoutManager;
    MBContainer                *m_mbContainer;

    FPUPrecisionMode            m_precisionMode;
    ColorMapData                m_colorMapData;
    RealRectangle2D             m_rect0, m_zoom1Rect;

    bool                        m_animateCalculation;
    bool                        m_calculateWithOrbit;
    bool                        m_useOnly1CPU;
    bool                        m_useEdgeDetection;
    bool                        m_retainAspectRatio;
    bool                        m_showZoomFactor;
    bool                        m_suspendingMenuTextIsSuspending;
    Size2D                      m_zoomFactor;
    DlgState                    m_state;
    CalculatorPool             *m_calculatorPool;
    MBFrameGenerator           *m_frameGenerator;
    D3DCOLOR                   *m_colorMap; // last element is always BLACK
    RealRectangleTransformation m_transform;
    Semaphore                   m_gate;
    bool                        m_hasResized;
    BitSet8                     m_runningTimerSet;
    SynchronizedQueue<CRect>    m_jobQueue;
    CWnd                       *m_imageWindow;
    HRGN                        m_imageRGN;
    HDC                         m_imageDC;
    PixRect                    *m_pixRect, *m_dummyPr;
    PixelAccessor              *m_pixelAccessor;
    CRect                       m_dragRect;
    CPoint                      m_mouseDownPoint;
    Stack<FractalImage>         m_pictureStack;

    void updateWindowStateInternal();
    void showWindowState();
    void showMousePoint(const CPoint &p);
    void saveRectangle(const String &fileName);
    void loadRectangle(const String &fileName);
    void createColorMap();
    void destroyColorMap();
    void saveColorMap(const String &fileName);
    void loadColorMap(const String &fileName);
    void setWorkSize();
    void setWorkSize(const PixRect *src);
    void setWorkSize(  const CSize &size);
    void createPixRect(const CSize &size);
    void destroyPixRect();
    void setUncalculatedPixelsToEmpty();
    void clearUncalculatedWindowArea();
    int  getCPUCountToUse() const;
    bool setColorMapData(const ColorMapData &colorMapData);
    void setPrecision(int id);
    void setScale(const Real &minX, const Real &maxX, const Real &minY, const Real &maxY, bool allowAdjustAspectRatio);
    void updateZoomFactor();
    void startTimer(UINT id, int msec);
    void stopTimer(UINT id);
    inline bool isValidSize() const {
      return m_pixRect != NULL;
    }
    inline RealRectangle2D getScale() const {
      return m_transform.getFromRectangle();
    }
    void     setDragRect(   const CPoint &topLeft, const CPoint &bottomRight);
    CRect    createDragRect(const CPoint &topLeft, const CPoint &bottomRight);
    void     removeDragRect();
    CPoint   getImagePointFromMousePoint(const CPoint &p) const;
    PixRect *getCalculatedPart(    const CSize &dp);
    void     paintMovedImage(      const CSize &dp);
    void     calculateMovedImage(  const CSize &dp);
    static   MoveDirection getMoveDirection(const CSize &dp);
    void     flushPixRect();
    void     paintZoomFactor(CDC &dc);
    void     putPixRect(const PixRect *src);
    void     pushImage();
    void     popImage();
    void     resetImageStack();
    void     paintPointSet(const PointSet &ps, COLORREF color);
    void     startCalculation();
    void     setRectanglesToCalculate(const CompactArray<CRect> &rectangles);
    void     setRectangleToCalculate(const CRect &rectangle);
    void     setSuspendingMenuText(bool isSuspendingText);
    inline bool hasPixelAccessor() const {
      return m_pixelAccessor != NULL;
    }
    void clearPixelAccessor();
    void setState(DlgState newState);
    inline DlgState getState()  const {
      return m_state;
    }
    inline const TCHAR *getStateName() const {
      return s_stateName[m_state];
    }
public:
    CMandelbrotDlg(CWnd *pParent = NULL);

    void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

    inline CWnd *getImageWindow() {
      return m_imageWindow;
    }
    inline CSize getImageSize() {
      return getClientRect(m_imageWindow).Size();
    }
    void     setScale(const RealRectangle2D &scale, bool allowAdjustAspectRatio = true);
    void     setPixel(UINT x, UINT y, D3DCOLOR color);
    D3DCOLOR getPixel(UINT x, UINT y) const;

    inline const RealRectangleTransformation &getTransformation() const {
      return m_transform;
    }

    inline RealRectangleTransformation &getTransformation() {
      return m_transform;
    }

    inline UINT getMaxIteration() const {
      return m_colorMapData.m_maxIteration;
    }

    inline const D3DCOLOR *getColorMap() const {
      return m_colorMap;
    }

    inline FPUPrecisionMode getPrecisionMode() const {
      return m_precisionMode;
    }

    PixelAccessor *getPixelAccessor();

    inline bool calculateWithOrbit() const {
      return m_calculateWithOrbit;
    }

    inline bool useEdgeDetection() const {
      return m_useEdgeDetection;
    }

    inline bool isRetainAspectRatio() const {
      return m_retainAspectRatio;
    }

    inline bool showZoomFactor() const {
      return m_showZoomFactor;
    }

    inline const Size2D &getZoomFactor() const {
      return m_zoomFactor;
    }

    void paintMark(const CPoint &p);

    bool getJobToDo(CRect &rect);

//-----------------------------------------------

  void initScale();
  inline bool isCalculationActive(bool checkMovie = true) const {
    return (m_calculatorPool && m_calculatorPool->isCalculationActive()) || (checkMovie && isMakingMovie());
  }
  inline bool isCalculationSuspended() const {
    return isCalculationActive() && m_calculatorPool->getCalculatorsInState(CALC_RUNNING).isEmpty();
  }
  inline bool animateCalculation() const {
    return m_animateCalculation;
  }
  inline bool isMakingMovie() const {
    return m_frameGenerator != NULL;
  }
  void updateMovieMenuItem();
  void remoteStartCalculation();

  enum { IDD = IDD_MANDELBROT_DIALOG };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
  afx_msg void OnSize(  UINT nType, int cx, int cy);
  afx_msg void OnClose();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnCancel();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(  UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
  afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
  afx_msg void OnFileSaveRectangle();
  afx_msg void OnFileSaveColorMap();
  afx_msg void OnFileSaveImage();
  afx_msg void OnFileLoadRectangle();
  afx_msg void OnFileLoadColorMap();
  afx_msg void OnFileMakeMovie();
  afx_msg void OnFileStopMovie();
  afx_msg void OnFileExit();
  afx_msg void OnEditCalculateImage();
  afx_msg void OnEditSuspendCalculation();
  afx_msg void OnEditAbortCalculation();
  afx_msg void OnEditNewColorMap();
  afx_msg void OnEditBack();
  afx_msg void OnOptionsColorMap();
  afx_msg void OnOptionsShowColorMap();
  afx_msg void OnOptions32BitsFloatingPoint();
  afx_msg void OnOptions64BitsFloatingPoint();
  afx_msg void OnOptions80BitsFloatingPoint();
  afx_msg void OnOptionsAnimateCalculation();
  afx_msg void OnOptionsPaintOrbit();
  afx_msg void OnOptionsUseEdgeDetection();
  afx_msg void OnOptionsUseOnly1CPU();
  afx_msg void OnOptionsRetainAspectRatio();
  afx_msg void OnOptionsShowZoomFactor();
  afx_msg void OnHelpAboutMandelbrot();
  afx_msg LRESULT OnMsgStartCalculation( WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgUpdateWindowState(WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgMovieDone(        WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()
};
