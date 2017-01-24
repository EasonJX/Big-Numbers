#include "stdafx.h"
#include <Math.h>
#include <direct.h>
#include <MFCUtil/SelectDirDlg.h>
#include <ByteFile.h>
#include <Random.h>
#include "Mandelbrot.h"
#include "MandelbrotDlg.h"
#include "JuliaDlg.h"
#include "ShowColorMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CMandelbrotDlg::CMandelbrotDlg(CWnd *pParent) : CDialog(CMandelbrotDlg::IDD, pParent) {
  m_hIcon                     = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_state                     = STATE_IDLE;
  m_precisionMode             = FPU_NORMAL_PRECISION;
  m_pixRect                   = NULL;
  m_pixelAccessor             = NULL;
  m_imageDC                   = NULL;
  m_colorMap                  = NULL;
  m_frameGenerator            = NULL;
}

BOOL CMandelbrotDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CMandelbrotDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMandelbrotDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZING()
	  ON_WM_SIZE()
	  ON_WM_CLOSE()
	  ON_WM_TIMER()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_NCLBUTTONDOWN()
    ON_WM_NCMOUSEMOVE()
	  ON_COMMAND(ID_FILE_SAVERECTANGLE                   , OnFileSaveRectangle                  )
	  ON_COMMAND(ID_FILE_SAVECOLORMAP                    , OnFileSaveColorMap                   )
	  ON_COMMAND(ID_FILE_SAVEIMAGE                       , OnFileSaveImage                      )
	  ON_COMMAND(ID_FILE_LOADRECTANGLE                   , OnFileLoadRectangle                  )
	  ON_COMMAND(ID_FILE_LOADCOLORMAP                    , OnFileLoadColorMap                   )
	  ON_COMMAND(ID_FILE_MAKEMOVIE                       , OnFileMakeMovie                      )
    ON_COMMAND(ID_FILE_STOPMOVIE                       , OnFileStopMovie                      )
    ON_COMMAND(ID_FILE_EXIT                            , OnFileExit                           )
	  ON_COMMAND(ID_EDIT_CALCULATEIMAGE                  , OnEditCalculateImage                 )
	  ON_COMMAND(ID_EDIT_SUSPENDCALCULATION              , OnEditSuspendCalculation             )
	  ON_COMMAND(ID_EDIT_ABORTCALCULATION                , OnEditAbortCalculation               )
	  ON_COMMAND(ID_EDIT_NEWCOLORMAP                     , OnEditNewColorMap                    )
    ON_COMMAND(ID_EDIT_BACK                            , OnEditBack                           )
	  ON_COMMAND(ID_OPTIONS_COLORMAP                     , OnOptionsColorMap                    )
	  ON_COMMAND(ID_OPTIONS_SHOWCOLORMAP                 , OnOptionsShowColorMap                )
    ON_COMMAND(ID_OPTIONS_32BITSFLOATINGPOINT          , OnOptions32BitsFloatingPoint         )
    ON_COMMAND(ID_OPTIONS_64BITSFLOATINGPOINT          , OnOptions64BitsFloatingPoint         )
    ON_COMMAND(ID_OPTIONS_80BITSFLOATINGPOINT          , OnOptions80BitsFloatingPoint         )
    ON_COMMAND(ID_OPTIONS_ANIMATE_CALCULATION          , OnOptionsAnimateCalculation          )
    ON_COMMAND(ID_OPTIONS_PAINTORBIT                   , OnOptionsPaintOrbit                  )
    ON_COMMAND(ID_OPTIONS_USEEDGEDETECTION             , OnOptionsUseEdgeDetection            )
    ON_COMMAND(ID_OPTIONS_USEONLY1CPU                  , OnOptionsUseOnly1CPU                 )
    ON_COMMAND(ID_OPTIONS_RETAIN_ASPECTRATIO           , OnOptionsRetainAspectRatio           )
    ON_COMMAND(ID_HELP_ABOUTMANDELBROT                 , OnHelpAboutMandelbrot                )
    ON_MESSAGE(ID_MSG_STARTCALCULATION                 , OnMsgStartCalculation                )
    ON_MESSAGE(ID_MSG_UPDATEWINDOWSTATE                , OnMsgUpdateWindowState               )
    ON_MESSAGE(ID_MSG_MOVIEDONE                        , OnMsgMovieDone                       )
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CMandelbrotDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

HCURSOR CMandelbrotDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

BOOL CMandelbrotDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);   // Set big icon
  SetIcon(m_hIcon, FALSE);  // Set small icon

  m_crossIcon  = createIcon(AfxGetApp()->m_hInstance, IDB_MARK_COLORBITMAP, IDB_MARK_MASKBITMAP);
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  theApp.m_device.attach(*this);

  m_dummyPr        = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, 10, 10);
  m_mbContainer    = new DialogMBContainer(this, m_dummyPr);
  m_calculatorPool = new CalculatorPool(*m_mbContainer);

  const CSize startSize(750,750);;

  setClientRectSize(this, CSize(startSize.cx+50, startSize.cy+50));
  setWindowPosition(this, IDC_STATIC_IMAGEWINDOW, CPoint(0, 0));
  setClientRectSize(this, IDC_STATIC_IMAGEWINDOW, startSize);
  const CRect imageRect = ::getWindowRect(this, IDC_STATIC_IMAGEWINDOW);
  const int infoWidth = imageRect.Width() *3 / 7;
  setWindowPosition(this, IDC_STATIC_INFOWINDOW, CPoint(0, imageRect.bottom));
  setWindowSize(    this, IDC_STATIC_INFOWINDOW, CSize(infoWidth, 20));
  const CRect infoRect = ::getWindowRect(this, IDC_STATIC_INFOWINDOW);
  setWindowPosition(this, IDC_STATIC_MOUSEINFO , CPoint(infoRect.right, infoRect.top));
  setWindowSize(    this, IDC_STATIC_MOUSEINFO , CSize(imageRect.right - infoRect.right, 20));

  setClientRectSize(this, CSize(imageRect.Width(), infoRect.bottom));
  
  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATIC_IMAGEWINDOW, RELATIVE_SIZE                  );
  m_layoutManager.addControl(IDC_STATIC_INFOWINDOW , RELATIVE_Y_POS | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_MOUSEINFO  , RELATIVE_POSITION              );

  m_animateCalculation = isMenuItemChecked(this, ID_OPTIONS_ANIMATE_CALCULATION);
  m_paintOrbit         = isMenuItemChecked(this, ID_OPTIONS_PAINTORBIT         );
  m_useOnly1CPU        = isMenuItemChecked(this, ID_OPTIONS_USEONLY1CPU        );
  m_useEdgeDetection   = isMenuItemChecked(this, ID_OPTIONS_USEEDGEDETECTION   );
  m_retainAspectRatio  = isMenuItemChecked(this, ID_OPTIONS_RETAIN_ASPECTRATIO );
  m_suspendingMenuTextIsSuspending = true;

//  setWindowPosition(this, CPoint(0,0)); 
  centerWindow(this);
  m_imageWindow = GetDlgItem(IDC_STATIC_IMAGEWINDOW);

  m_calculatorPool->addPropertyChangeListener(m_mbContainer);
  initScale();
  setWorkSize();
  setColorMapData(ColorMapData());
  startCalculation();

  return TRUE;  // return TRUE  unless you set the focus to a control
}

// -------------------------------- File menu -------------------------------------------

static const TCHAR *rectangleFileExtensions = _T("Rectangle files (*.rect)\0*.rect;\0"
                                                 "All files (*.*)\0*.*\0\0");

static const TCHAR *colormapFileExtensions  = _T("Colormap files (*.map)\0*.map;\0"
                                                 "All files (*.*)\0*.*\0\0");

void CMandelbrotDlg::OnFileSaveRectangle() {
  try {
    CFileDialog dlg(FALSE);
    dlg.m_ofn.lpstrFilter = rectangleFileExtensions;
    dlg.m_ofn.lpstrTitle  = _T("Save Rectangle");

    const RealRectangle r = m_transform.getFromRectangle();

    String fileName = format(_T("[%s,%s]x[%s,%s].rect")
                            ,::toString(r.getMinX()  ,16,0, ios::scientific).cstr()
                            ,::toString(r.getMinY()  ,16,0, ios::scientific).cstr()
                            ,::toString(r.getWidth() , 5,0, ios::scientific).cstr()
                            ,::toString(r.getHeight(), 5,0, ios::scientific).cstr()
                            );
    TCHAR nameBuffer[256];
    _tcscpy(nameBuffer, fileName.cstr());
    dlg.m_ofn.lpstrFile = nameBuffer;
    dlg.m_ofn.nMaxFile  = ARRAYSIZE(nameBuffer);
    if(dlg.DoModal() != IDOK || _tcslen(nameBuffer) == 0) {
      return;
    }

    FileNameSplitter nameInfo(nameBuffer);
    if(nameInfo.getExtension().length() == 0) {
      nameInfo.setExtension(_T("rect"));
    }
    saveRectangle(nameInfo.getAbsolutePath());
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CMandelbrotDlg::OnFileSaveColorMap() {
  try {
    CFileDialog dlg(FALSE);
    dlg.m_ofn.lpstrFilter = colormapFileExtensions;
    dlg.m_ofn.lpstrTitle  = _T("Save Colormap");

    if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
      return;
    }
    FileNameSplitter nameInfo(dlg.m_ofn.lpstrFile);
    if(nameInfo.getExtension().length() == 0) {
      nameInfo.setExtension(_T("map"));
    }
    saveColorMap(nameInfo.getAbsolutePath());
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CMandelbrotDlg::OnFileSaveImage() {
  static const TCHAR *imageFileExtensions = _T("Bitmap files (*.bmp)\0*.bmp;\0"
                                               "All files (*.*)\0*.*\0\0");

  CFileDialog dlg(FALSE);
  dlg.m_ofn.lpstrFilter = imageFileExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save image");

  if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }

  try {
    m_pixRect->writeAsBMP(ByteOutputFile(FileNameSplitter(dlg.m_ofn.lpstrFile).setExtension(_T("bmp")).getFullPath()));
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CMandelbrotDlg::OnFileLoadRectangle() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter  = rectangleFileExtensions;
  dlg.m_ofn.lpstrTitle   = _T("Load Rectangle");
  dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
  if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }
  try {
    loadRectangle(dlg.m_ofn.lpstrFile);
    startCalculation();
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CMandelbrotDlg::OnFileLoadColorMap() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter  = colormapFileExtensions;
  dlg.m_ofn.lpstrTitle   = _T("Load Colormap");
  dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
  if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }
  try {
    loadColorMap(dlg.m_ofn.lpstrFile);
    startCalculation();
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CMandelbrotDlg::OnFileMakeMovie() {
  if(isMakingMovie()) return;
  REDIRECTLOG();
  TCHAR cwd[1024];
  _tgetcwd(cwd, ARRAYSIZE(cwd));
  CSelectDirDlg dlg(cwd);
  if(dlg.DoModal() != IDOK) {
    return;
  }
  m_frameGenerator = new MBFrameGenerator(this, dlg.getSelectedDir());
  updateMovieMenuItem();
  startTimer(2, 5000);
}

void CMandelbrotDlg::OnFileStopMovie() {
  if(!isMakingMovie()) return;
  stopTimer(2);
  delete m_frameGenerator;
  m_frameGenerator = NULL;
  updateWindowStateInternal();
  updateMovieMenuItem();
}

void CMandelbrotDlg::OnFileExit() {
  delete m_calculatorPool;
  PostMessage(WM_QUIT);
}

void CMandelbrotDlg::OnClose() {
  OnFileExit();
}

void CMandelbrotDlg::OnOK() {
}

void CMandelbrotDlg::OnCancel() {
}

// -------------------------------- Edit menu -------------------------------------------

void CMandelbrotDlg::OnEditCalculateImage() {
  startCalculation();
}

void CMandelbrotDlg::OnEditSuspendCalculation() {
  if(isCalculationActive()) {
    if(m_calculatorPool->getCalculatorsInState(CALC_RUNNING).isEmpty()) {
      m_calculatorPool->resumeCalculation();
    } else {
      m_calculatorPool->suspendCalculation();
    }
  }
}

void CMandelbrotDlg::OnEditAbortCalculation() {
  m_calculatorPool->killAll();
}

void CMandelbrotDlg::OnEditNewColorMap() {
  makeColorMap();
  startCalculation();
}

void CMandelbrotDlg::OnEditBack() {
  popImage();
}

// -------------------------------- Options menu ----------------------------------------

void CMandelbrotDlg::OnOptionsColorMap() {
  if(isCalculationActive()) {
    return;
  }
  CColorMapDlg dlg(m_colorMapData);
  if(dlg.DoModal() == IDOK) {
    if(setColorMapData(dlg.getColorMapData())) {
      startCalculation();
    }
  }
}

void CMandelbrotDlg::OnOptionsShowColorMap() {
  CShowColorMapDlg dlg(m_colorMapData.m_maxIteration, m_colorMap);
  dlg.DoModal();
}

void CMandelbrotDlg::OnOptions32BitsFloatingPoint() {
  setPrecision(ID_OPTIONS_32BITSFLOATINGPOINT);
}

void CMandelbrotDlg::OnOptions64BitsFloatingPoint() {
  setPrecision(ID_OPTIONS_64BITSFLOATINGPOINT);
}

void CMandelbrotDlg::OnOptions80BitsFloatingPoint() {
  setPrecision(ID_OPTIONS_80BITSFLOATINGPOINT);
}

void CMandelbrotDlg::OnOptionsAnimateCalculation() {
  m_animateCalculation = toggleMenuItem(this, ID_OPTIONS_ANIMATE_CALCULATION);
}

void CMandelbrotDlg::OnOptionsPaintOrbit() {
  m_paintOrbit = toggleMenuItem(this, ID_OPTIONS_PAINTORBIT);
}

void CMandelbrotDlg::OnOptionsUseOnly1CPU() {
  m_useOnly1CPU = toggleMenuItem(this, ID_OPTIONS_USEONLY1CPU);
}

void CMandelbrotDlg::OnOptionsUseEdgeDetection() {
  m_useEdgeDetection = toggleMenuItem(this, ID_OPTIONS_USEEDGEDETECTION);
}

void CMandelbrotDlg::OnOptionsRetainAspectRatio() {
  if(m_retainAspectRatio = toggleMenuItem(this, ID_OPTIONS_RETAIN_ASPECTRATIO)) {
    if(m_transform.adjustAspectRatio()){
      startCalculation();
    }
  }
}

void CMandelbrotDlg::OnHelpAboutMandelbrot() {
  CAboutDlg dlg;
  dlg.DoModal();
}

// ----------------------------------- Mouse --------------------------------------------

CPoint CMandelbrotDlg::getImagePointFromMousePoint(const CPoint &p) const {
  CPoint tmp = p;
  ClientToScreen(&tmp);
  m_imageWindow->ScreenToClient(&tmp);
  return tmp;
}

void CMandelbrotDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  CDialog::OnLButtonDown(nFlags, point);
  if(!isCalculationActive()) {
    m_mouseDownPoint = getImagePointFromMousePoint(point);
#ifdef SAVE_CALCULATORINFO
    if(nFlags & MK_CONTROL) {
      const CalculatorInfo *info = m_calculatorPool->findInfo(m_mouseDownPoint);
      if(info != NULL) {
        paintPointSet(info->getEdgeSet() , RGB(255,255,255));
        paintPointSet(info->getInnerSet(), RGB(255,255,0));
        MessageBox(info->toString().cstr(), _T("Info"));
        paintPointSet(info->getEdgeSet() , BLACK);
        paintPointSet(info->getInnerSet(), BLACK);
      }
      CDialog::OnLButtonDown(nFlags, point);
      return;
    }
#endif
    setDragRect(m_mouseDownPoint,m_mouseDownPoint);
    setState(STATE_DRAGGING);
  }
}

void CMandelbrotDlg::paintPointSet(const PointSet &ps, COLORREF color) {
  CClientDC dc(m_imageWindow);
  for(Iterator<size_t> it = ((PointSet&)ps).getIterator(); it.hasNext();) {
    const CPoint p = ps.next(it);
    dc.SetPixel(p, color);
  }
}

void CMandelbrotDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  CDialog::OnLButtonUp(nFlags, point);
  if(getState() == STATE_DRAGGING) {
    setState(STATE_IDLE);
    if(getArea(m_dragRect) != 0) {
      pushImage();
      setScale(m_transform.backwardTransform(m_dragRect));
      startCalculation();
    }
  }
}

void CMandelbrotDlg::OnLButtonDblClk(UINT nFlags, CPoint point) {
  CDialog::OnLButtonDblClk(nFlags, point);
  m_mouseDownPoint = getImagePointFromMousePoint(point);
  const RealPoint2D p = m_transform.backwardTransform(m_mouseDownPoint);
  CJuliaDlg dlg(p);
  dlg.DoModal();
}

void CMandelbrotDlg::OnRButtonDown(UINT nFlags, CPoint point) {
  CDialog::OnRButtonDown(nFlags, point);
  if(!isCalculationActive()) {
    m_mouseDownPoint = getImagePointFromMousePoint(point);
    setState(STATE_MOVING);
  }
}

void CMandelbrotDlg::OnRButtonUp(UINT nFlags, CPoint point) {
  CDialog::OnRButtonUp(nFlags, point);
  if(getState() == STATE_MOVING) {
    setState(STATE_IDLE);
    calculateMovedImage(getImagePointFromMousePoint(point) - m_mouseDownPoint);
  }
}

void CMandelbrotDlg::OnMouseMove(UINT nFlags, CPoint point) {
  CDialog::OnMouseMove(nFlags, point);
  const CPoint p = getImagePointFromMousePoint(point);
  switch(getState()) {
  case STATE_IDLE    :
    break;
  case STATE_DRAGGING:
    if(nFlags & MK_LBUTTON) {
      setDragRect(m_mouseDownPoint,p);
      CPoint rb = m_dragRect.BottomRight();
      if(rb != p) {
        m_imageWindow->ClientToScreen(&rb);
        SetCursorPos(rb.x,rb.y);
      }
    } else {
      removeDragRect();
      setState(STATE_IDLE);
    }
    break;

  case STATE_MOVING  :
    if(nFlags & MK_RBUTTON) {
      paintMovedImage(p - m_mouseDownPoint);
    } else {
      paintMovedImage(CSize(0,0));
      setState(STATE_IDLE);
    }
    break;
  }
  showMousePoint(p);
}

BOOL CMandelbrotDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  if(!isCalculationActive()) {
    if(zDelta > 0) {
      pushImage();
      m_imageWindow->ScreenToClient(&pt);
      setScale(m_transform.zoom(pt,0.25));
      startCalculation();
    } else {
      popImage();
    }
  }
  return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

// ----------------------------------- Message handlers ---------------------------------

long CMandelbrotDlg::OnMsgStartCalculation(WPARAM wp, LPARAM lp) {
  startCalculation();
  return 0;
}

long CMandelbrotDlg::OnMsgUpdateWindowState(WPARAM wp, LPARAM lp) {
  updateWindowStateInternal();
  return 0;
}

long CMandelbrotDlg::OnMsgMovieDone(WPARAM wp, LPARAM lp) {
  OnFileStopMovie();
  return 0;
}

void CMandelbrotDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  switch(id) {
  case CALCULATIONACTIVE:
    { const bool oldActive = *(bool*)oldValue;
      const bool newActive = *(bool*)newValue;
      if(oldActive && !newActive) {
        m_jobQueue.clear();
        stopTimer(1);
        DLOG((_T("CalculationACtive=FALSE. Now POST WM_PAINT\n")));
        PostMessage(WM_PAINT);
      } else if(newActive) {
        PostMessage(ID_MSG_UPDATEWINDOWSTATE);
      }
    }
    break;
  case RUNNINGSET:
    { const CalculatorSet oldRunSet = *(CalculatorSet*)oldValue;
      const CalculatorSet newRunSet = *(CalculatorSet*)newValue;
      if(oldRunSet.isEmpty() != newRunSet.isEmpty()) {
        PostMessage(ID_MSG_UPDATEWINDOWSTATE);
      }
    }
    break;
  }
}

void CMandelbrotDlg::OnPaint() {
  if (IsIconic())   {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    if(!isCalculationActive(false)) {
      updateWindowStateInternal();
      clearPixelAccessor();
      flushPixRect();
      m_hasResized = false;
      if(isMakingMovie()) {
        m_frameGenerator->notifyFrameReady();
      }
    }
    CDialog::OnPaint();
  }
}

// --------------------------------------------------------------------------------------------------

void CMandelbrotDlg::setState(DlgState newState) {
  m_state = newState;
  showWindowState();
  switch(m_state) {
  case STATE_IDLE:
    setWindowCursor(m_imageWindow, MAKEINTRESOURCE(OCR_NORMAL));
    break;
  case STATE_DRAGGING:
    setWindowCursor(m_imageWindow, MAKEINTRESOURCE(OCR_CROSS));
    break;
  case STATE_MOVING:
    setWindowCursor(m_imageWindow, MAKEINTRESOURCE(OCR_HAND));
    break;
  }
}

const TCHAR *CMandelbrotDlg::s_stateName[] = {
  _T("IDLE")
 ,_T("DRAGGING")
 ,_T("MOVING")
};

void CMandelbrotDlg::flushPixRect() {
  m_gate.wait();

  clearPixelAccessor();
  CClientDC dc(m_imageWindow);
  PixRect::bitBlt(dc,0,0,m_pixRect->getWidth(),m_pixRect->getHeight(),SRCCOPY,m_pixRect,0,0);

  m_gate.signal();
}

void CMandelbrotDlg::updateWindowStateInternal() {
  const bool isActive = isCalculationActive();

  enableMenuItem(this, ID_FILE_SAVEIMAGE               ,!isActive   );
  enableMenuItem(this, ID_FILE_MAKEMOVIE               ,!isActive   );
  enableMenuItem(this, ID_EDIT_CALCULATEIMAGE          ,!isActive   );
  enableMenuItem(this, ID_EDIT_ABORTCALCULATION        ,isActive    );
  enableMenuItem(this, ID_EDIT_SUSPENDCALCULATION      ,isActive    );
  enableMenuItem(this, ID_EDIT_NEWCOLORMAP             ,!isActive && m_colorMapData.m_randomSeed);
  enableMenuItem(this, ID_EDIT_BACK                    ,!isActive && !m_pictureStack.isEmpty());
  setSuspendingMenuText(!isCalculationSuspended());
  enableMenuItem(this, ID_OPTIONS_COLORMAP             ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_32BITSFLOATINGPOINT  ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_64BITSFLOATINGPOINT  ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_80BITSFLOATINGPOINT  ,!isActive   );
  enableSubMenuContainingId(this, ID_OPTIONS_80BITSFLOATINGPOINT  ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_ANIMATE_CALCULATION  ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_PAINTORBIT           ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_USEEDGEDETECTION     ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_USEONLY1CPU          ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_RETAIN_ASPECTRATIO   ,!isActive   );
  showWindowState();
}

void CMandelbrotDlg::setSuspendingMenuText(bool isSuspendingText) {
  if(isSuspendingText != m_suspendingMenuTextIsSuspending) {
    setMenuItemText(this
                   ,ID_EDIT_SUSPENDCALCULATION
                   ,isSuspendingText ? _T("Suspend calculation\tAlt+S")
                                     : _T("Resume calculation\tAlt+S")
                   );
    m_suspendingMenuTextIsSuspending = isSuspendingText;
  }
}

void CMandelbrotDlg::showWindowState() {
  if(!isMakingMovie()) {
    const RealRectangle r = m_transform.getFromRectangle();

    setWindowText(this
                , format(_T("[%s;%s]x[%s,%s]")
                        ,::toString(r.getMinX()  ,16,0, ios::scientific).cstr()
                        ,::toString(r.getMinY()  ,16,0, ios::scientific).cstr()
                        ,::toString(r.getWidth() , 5,0, ios::scientific).cstr()
                        ,::toString(r.getHeight(), 5,0, ios::scientific).cstr()
                        )
                 );

    setWindowText(this, IDC_STATIC_INFOWINDOW
                      , format(_T("Level:%2d. %s")
                              , m_pictureStack.getHeight()
                              , isCalculationActive()
                              ? (isCalculationSuspended() ? _T("Suspended") : _T("Calculating"))
                              : getStateName()
                              )
                 );
  } else {
    setWindowText(this, _T("Generating image list"));

    setWindowText(this, IDC_STATIC_INFOWINDOW
                      , format(_T("Calculating frame %d/%d"), m_frameGenerator->getFrameIndex(), m_frameGenerator->getTotalFrameCount()));
  }
}

void CMandelbrotDlg::showMousePoint(const CPoint &p) {
  if(m_pixRect->contains(p)) {
    const RealPoint2D rp = m_transform.backwardTransform(p);
    const String msg = format(_T(" (%3d,%3d) [%s,%s]")
                             , p.x,p.y
                             ,::toString(rp.x,16,24,ios::scientific|ios::showpos).cstr()
                             ,::toString(rp.y,16,24,ios::scientific|ios::showpos).cstr()
                       );
    setWindowText(this, IDC_STATIC_MOUSEINFO, msg);
  }
}

void CMandelbrotDlg::remoteStartCalculation() {
  DLOG((_T("Post STARTCALCULATION\n")));
  PostMessage(ID_MSG_STARTCALCULATION);
}

void CMandelbrotDlg::setWorkSize() {
  setWorkSize(getClientRect(m_imageWindow).Size());
}

void CMandelbrotDlg::setWorkSize(const PixRect *src) {
  setWorkSize(src->getSize());
}

void CMandelbrotDlg::setWorkSize(const CSize &size) {
  if(m_imageDC != NULL) {
    ::ReleaseDC(m_imageWindow->m_hWnd, m_imageDC);
    m_imageDC = NULL;
  }
  m_imageRGN    = CreateRectRgn(0,0,size.cx, size.cy);
  m_imageDC     = ::GetDCEx(m_imageWindow->m_hWnd, m_imageRGN, DCX_CACHE | DCX_LOCKWINDOWUPDATE);

  if((m_pixRect != NULL) && (size == m_pixRect->getSize())) {
    return;
  }

  createPixRect(size);
  if(m_pictureStack.getHeight() == 0) {
    initScale();
  }
  if(isValidSize()) {
    m_transform.setToRectangle(CRect(0,0,size.cx, size.cy));
    if(m_retainAspectRatio) {
      m_transform.adjustAspectRatio();
    }
  }
}

void CMandelbrotDlg::createPixRect(const CSize &size) {
  if(m_pixRect != NULL) {
    delete m_pixRect;
    m_pixRect = NULL;
  }

  if(size.cx <= 0 || size.cy <= 0) {
    return;
  }

  m_pixRect = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, size);
}

bool CMandelbrotDlg::setColorMapData(const ColorMapData &colorMapData) {
  if((colorMapData != m_colorMapData) || (m_colorMap == NULL)) {
    m_colorMapData = colorMapData;
    makeColorMap();
    return true;
  }
  return false;
}

static int newRandomStep(int old) {
  const int result = randInt(2, 20);
  if(old > 0) {
    return -result;
  }
  return result;
}

void CMandelbrotDlg::makeColorMap() {
  if(m_colorMap != NULL) {
    delete[] m_colorMap;
    m_colorMap = NULL;
  }
  m_colorMap = new D3DCOLOR[m_colorMapData.m_maxIteration+1];
  if(m_colorMapData.m_randomSeed) {
    randomize();
  } else {
    _standardRandomGenerator.setSeed(m_colorMapData.m_seed);
  }

  int r  = randInt(0,255);
  int g  = randInt(0,255);
  int b  = randInt(0,255);
  int dr = newRandomStep(randInt(-5,5));
  int dg = newRandomStep(randInt(-5,5));
  int db = newRandomStep(randInt(-5,5));
  if(dr == 0) dr =  3;
  if(dg == 0) dg = -7;
  if(db == 0) db = 13;
  UINT i;
  for(i = 0; i < m_colorMapData.m_maxIteration; i++) {
    D3DCOLOR &c = m_colorMap[i];
    if((c = RGB(r,g,b)) == EMPTY_COLOR) {
      c = NEXTTO_EMPTY_COLOR;
    } else if(c == BLACK     ) {
      c = NEXTTO_BLACK;
    } else if(c == FILL_COLOR) {
      c = NEXTTO_FILL_COLOR;
    }

    if(r+dr > 255 || r+dr < 0) dr = newRandomStep(dr);
    if(g+dg > 255 || g+dg < 0) dg = newRandomStep(dg);
    if(b+db > 255 || b+db < 0) db = newRandomStep(db);
    r += dr;
    g += dg;
    b += db;
  }
  m_colorMap[i] = BLACK;
}

void CMandelbrotDlg::saveColorMap(const String &fileName) {
  ByteOutputFile out(fileName);
  const UINT maxIteration = m_colorMapData.m_maxIteration;
  out.putBytes((BYTE*)&maxIteration, sizeof(maxIteration));
  out.putBytes((BYTE*)m_colorMap, sizeof(m_colorMap[0])*(maxIteration+1));
}

void CMandelbrotDlg::loadColorMap(const String &fileName) {
  ByteInputFile in(fileName);
  UINT maxIteration;
  D3DCOLOR *colorMap = NULL;
  try {
    in.getBytesForced((BYTE*)&maxIteration, sizeof(maxIteration));
    D3DCOLOR *colorMap = new D3DCOLOR[maxIteration+1];
    in.getBytesForced((BYTE*)colorMap, sizeof(colorMap[0])*(maxIteration+1));
    m_colorMapData.m_maxIteration = maxIteration;
    delete[] m_colorMap;
    m_colorMap = colorMap;
  } catch(...) {
    delete colorMap;
    throw;
  }
}

void CMandelbrotDlg::saveRectangle(const String &fileName) {
  const RealRectangle r = m_transform.getFromRectangle();
  ByteOutputFile(fileName).putBytes((BYTE*)&r, sizeof(r));
}

void CMandelbrotDlg::loadRectangle(const String &fileName) {
  RealRectangle r;
  ByteInputFile(fileName).getBytesForced((BYTE*)&r, sizeof(r));
  setScale(r);
}

void CMandelbrotDlg::setPrecision(int id) {
  if(isCalculationActive()) {
    return;
  }
  checkMenuItem(this, ID_OPTIONS_32BITSFLOATINGPOINT,false);
  checkMenuItem(this, ID_OPTIONS_64BITSFLOATINGPOINT,false);
  checkMenuItem(this, ID_OPTIONS_80BITSFLOATINGPOINT,false);
  checkMenuItem(this, id, true);
  switch(id) {
  case ID_OPTIONS_32BITSFLOATINGPOINT:
    m_precisionMode = FPU_LOW_PRECISION;
    break;
  case ID_OPTIONS_64BITSFLOATINGPOINT:
    m_precisionMode = FPU_NORMAL_PRECISION;
    break;
  case ID_OPTIONS_80BITSFLOATINGPOINT:
    m_precisionMode = FPU_HIGH_PRECISION;
    break;
  }
  FPU::setPrecisionMode(m_precisionMode);

  startCalculation();
}

void CMandelbrotDlg::setScale(const RealRectangle &scale, bool allowAdjustAspectRatio) {
  setScale(scale.getMinX(),scale.getMaxX(),scale.getMinY(),scale.getMaxY(), allowAdjustAspectRatio);
}

void CMandelbrotDlg::setScale(const Real &minX, const Real &maxX, const Real &minY, const Real &maxY, bool allowAdjustAspectRatio) {
  FPU::setPrecisionMode(getPrecisionMode());
  m_transform.setFromRectangle(RealRectangle(minX, maxY, maxX-minX, minY-maxY));
  if(m_retainAspectRatio && allowAdjustAspectRatio) {
    m_transform.adjustAspectRatio();
  }
}

bool CMandelbrotDlg::isValidSize() const {
  return m_pixRect != NULL;
}

void CMandelbrotDlg::setDragRect(const CPoint &topLeft, const CPoint &bottomRight) {
  CClientDC dc(m_imageWindow);

  const CRect newDragRect = createDragRect(topLeft, bottomRight);
  const CSize size(1,1);

  if(getState() == STATE_DRAGGING) {
    dc.DrawDragRect(&newDragRect, size, &m_dragRect, size);
    m_dragRect = newDragRect;
  } else {
    m_dragRect = newDragRect;
    dc.DrawDragRect(&m_dragRect, size, &m_dragRect, size);
  }
}

CRect CMandelbrotDlg::createDragRect(const CPoint &topLeft, const CPoint &bottomRight) {
  if(!m_retainAspectRatio) {
    return CRect(topLeft,bottomRight);
  } else {
    CSize prSize = m_pixRect->getSize();
    return CRect(topLeft.x, topLeft.y, bottomRight.x, topLeft.y + (bottomRight.x - topLeft.x) * prSize.cy / prSize.cx);
  }
}

void CMandelbrotDlg::removeDragRect() {
  CClientDC dc(m_imageWindow);
  const CSize size(1,1), newSize(0,0);
  dc.DrawDragRect(&m_dragRect,newSize,&m_dragRect,size);
}

void CMandelbrotDlg::paintMovedImage(const CSize &dp) {
  PixRect *pr = getCalculatedPart(dp);
  CClientDC dc(m_imageWindow);
  PixRect::bitBlt(dc,0,0,pr->getWidth(),pr->getHeight(),SRCCOPY,pr,0,0);
  delete pr;
}

void CMandelbrotDlg::calculateMovedImage(const CSize &dp) {
  if(dp.cx == 0 && dp.cy == 0) {
    return;
  }
  const int  adx = abs(dp.cx);
  const int  ady = abs(dp.cy);
  const int  w   = m_pixRect->getWidth();
  const int  h   = m_pixRect->getHeight();
  PixRect   *pr  = getCalculatedPart(dp);
  m_pixRect->rop(0,0,w,h,SRCCOPY,pr,0,0);
  delete pr;

  CompactArray<CRect> uncalculatedRectangles;
  switch(getMoveDirection(dp)) {
  case NW:
    uncalculatedRectangles.add(CRect(w-adx, 0    , w   , h-ady ));
    uncalculatedRectangles.add(CRect(0    , h-ady, w   , h     ));
    break;
  case SW:
    uncalculatedRectangles.add(CRect(0    , 0    , w   , ady   ));
    uncalculatedRectangles.add(CRect(w-adx, ady  , w   , h     ));
    break;
  case NE:
    uncalculatedRectangles.add(CRect(0   , 0     , adx , h-ady ));
    uncalculatedRectangles.add(CRect(0   , h-ady , w   , h     ));
    break;
  case SE:
    uncalculatedRectangles.add(CRect(0   , 0    , w    , ady   ));
    uncalculatedRectangles.add(CRect(0   , ady  , adx  , h     ));
    break;
  }
  setRectanglesToCalculate(uncalculatedRectangles);
  setScale(m_transform.backwardTransform(m_pixRect->getRect() - dp),false);
  startCalculation();
}

PixRect *CMandelbrotDlg::getCalculatedPart(const CSize &dp) {
  const int  adx    = abs(dp.cx);
  const int  ady    = abs(dp.cy);
  PixRect   *result = m_pixRect->clone(PIXRECT_PLAINSURFACE, true);
  const int  w      = m_pixRect->getWidth();
  const int  h      = m_pixRect->getHeight();
  switch(getMoveDirection(dp)) {
  case NW: // point.x <  mousedownpoint.x && point.y <  mousedownpoint.y
    result->rop(0  ,0  ,w,h,SRCCOPY,m_pixRect,adx,ady);
    break;
  case SW: // point.x <  mousedownpoint.x && point.y >= mousedownpoint.y
    result->rop(0  ,ady,w,h,SRCCOPY,m_pixRect,adx,0  );
    break;
  case NE: // point.x >= mousedownpoint.x && point.y <  mousedownpoint.y
    result->rop(adx,0  ,w,h,SRCCOPY,m_pixRect,0  ,ady);
    break;
  case SE: // point.x >= mousedownpoint.x && point.y >= mousedownpoint.y
    result->rop(adx,ady,w,h,SRCCOPY,m_pixRect,0  ,0  );
    break;
  }
  return result;
}

MoveDirection CMandelbrotDlg::getMoveDirection(const CSize &dp) {
  const int code = dp.cx >= 0 ? 1 : 0;
  return (MoveDirection)((code << 1) + ((dp.cy >= 0) ? 1 : 0));
}

void CMandelbrotDlg::pushImage() {
  if(!isCalculationActive()) {
    m_pictureStack.push(FractalImage(getScale(),m_pixRect->clone(PIXRECT_PLAINSURFACE, true)));
    updateWindowStateInternal();
  }
}

void CMandelbrotDlg::popImage() {
  if(m_pictureStack.isEmpty()) {
    return;
  }
  if(!isCalculationActive()) {
    FractalImage image = m_pictureStack.pop();
    setWorkSize(image.m_pixRect);
    putPixRect(image.m_pixRect);
    delete image.m_pixRect;
    setScale(image.m_scale, false);
    PostMessage(WM_PAINT);
  }
}

void CMandelbrotDlg::putPixRect(const PixRect *src) {
  m_pixRect->rop(0,0,src->getWidth(),src->getHeight(),SRCCOPY,src,0,0);
}

// ----------------------------------- virtuals from PixelReceiver -------------------------

PixelAccessor *CMandelbrotDlg::getPixelAccessor() {
  if(!hasPixelAccessor()) {
    m_pixelAccessor = m_pixRect->getPixelAccessor();
    DLOG((_T("pixRect locked with pixelAccessor\n")));
  }
  return (paintOrbit() || animateCalculation()) ? m_mbContainer : m_pixelAccessor;
}

void CMandelbrotDlg::clearPixelAccessor() {
  if(m_pixelAccessor) {
    delete m_pixelAccessor;
    m_pixelAccessor = NULL;
    DLOG((_T("pixRect unlocked\n")));
  }
}

CSize CMandelbrotDlg::getImageSize() {
  return getClientRect(m_imageWindow).Size();
}

void CMandelbrotDlg::setPixel(UINT x, UINT y, D3DCOLOR color) {
  if(m_pixRect->contains(x,y)) {
    m_pixelAccessor->setPixel(x,y,color);
    SetPixel(m_imageDC, x,y,D3DCOLOR2COLORREF(color));
  }
}

D3DCOLOR CMandelbrotDlg::getPixel(UINT x, UINT y) {
  if(!m_pixRect->contains(x,y)) {
    return BLACK;
  }
  return m_pixelAccessor->getPixel(x,y);
}

void CMandelbrotDlg::setPixel(const CPoint &p, D3DCOLOR color) {
  if(m_pixRect->contains(p)) {
    m_pixelAccessor->setPixel(p,color);
    SetPixel(m_imageDC, p.x,p.y,D3DCOLOR2COLORREF(color));
  }
}

D3DCOLOR CMandelbrotDlg::getPixel(const CPoint &p) {
  if(!m_pixRect->contains(p)) {
    return BLACK;
  }
  return m_pixelAccessor->getPixel(p);
}

void CMandelbrotDlg::paintMark(const CPoint &p) {
  if(!DrawIcon(m_imageDC, p.x-15,p.y-15, m_crossIcon)) {
    const String str = getLastErrorText();
  }
}

bool CMandelbrotDlg::getJobToDo(CRect &rect) {
  bool result = true;

  m_gate.wait();

  if(m_jobQueue.isEmpty()) {
    result = false;
  } else {
    rect = m_jobQueue.get();
  }

  m_gate.signal();

  return result;
}

// -----------------------------------------------------------------------------------------

void CMandelbrotDlg::initScale() {
//  setScale(-2.075601671068005, -2.0756016710594397, 1.1087372571168186e-1, 1.1087372571176775e-1, m_retainAspectRatio);
  setScale(-2.1,2.1,-2.1,2.1, m_retainAspectRatio);
}

void CMandelbrotDlg::startCalculation() {
  m_gate.wait();

  if(isCalculationActive(false)) {
    m_gate.signal();
    return;
  }

  if(m_jobQueue.isEmpty()) { // calculate the whole visible area
    setRectangleToCalculate(m_transform.getToRectangle());
  }
  clearPixelAccessor();
  setUncalculatedPixelsToEmpty();
  if(paintOrbit() || animateCalculation()) {
    clearUncalculatedWindowArea();
  }

  getPixelAccessor();

  startTimer(1, 500);
  m_calculatorPool->startCalculators(getCPUCountToUse());
  m_gate.signal();
}

void CMandelbrotDlg::setUncalculatedPixelsToEmpty() {
  D3DCOLOR emptyColor = EMPTY_COLOR;
  for(size_t i = 0; i < m_jobQueue.size(); i++) {
    m_pixRect->fillRect(m_jobQueue[i], EMPTY_COLOR);
  }
}

void CMandelbrotDlg::clearUncalculatedWindowArea() {
  HBRUSH whiteBrush = CreateSolidBrush(WHITE);
  for(size_t i = 0; i < m_jobQueue.size(); i++) {
    FillRect(m_imageDC, &m_jobQueue[i], whiteBrush);
  }
}

void CMandelbrotDlg::setRectanglesToCalculate(const CompactArray<CRect> &rectangles) {
  m_gate.wait();
  for(size_t i = 0; i < rectangles.size(); i++) {
    setRectangleToCalculate(rectangles[i]);
  }
  m_gate.signal();
}

void CMandelbrotDlg::setRectangleToCalculate(const CRect &rectangle) {
  if(rectangle.Width() <= 0 || rectangle.Height() <= 0) {
    return;
  }
  const int cpus = getCPUCountToUse();
  int height = (cpus == 1) ? rectangle.Height() : (rectangle.Height() / (7 * cpus));
  if(height == 0) {
    height = 1;
  }
  int lastBottom = rectangle.top;
  while(lastBottom < rectangle.bottom) {
    CRect rect;
    rect.left   = rectangle.left;
    rect.right  = rectangle.right;
    rect.top    = lastBottom;
    rect.bottom = rect.top + height;
    if(rect.bottom > rectangle.bottom) {
      rect.bottom = rectangle.bottom;
    }
    if(rect.top < rect.bottom) {
      m_jobQueue.put(rect);
    }
    lastBottom = rect.bottom;
  }
}

int CMandelbrotDlg::getCPUCountToUse() const {
  return (paintOrbit() || m_useOnly1CPU) ? 1 : CalculatorPool::getCPUCount();
}

void CMandelbrotDlg::OnNcLButtonDown(UINT nHitTest, CPoint point) {
  if(isBorderHit(nHitTest)) {
    if((m_pictureStack.getHeight() > 0) || isCalculationActive()) {
      return;
    }
  }
  m_hasResized = false;
  CDialog::OnNcLButtonDown(nHitTest, point);
}

void CMandelbrotDlg::OnSizing(UINT fwSide, LPRECT pRect) {
  CDialog::OnSizing(fwSide, pRect);
  m_hasResized = true;
}

void CMandelbrotDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CMandelbrotDlg::OnNcMouseMove(UINT nHitTest, CPoint point) {
  CDialog::OnNcMouseMove(nHitTest, point);
  if(m_hasResized) {
    setWorkSize();
    if(isValidSize()) {
      startCalculation();
    }
  }
}

// ------------------------------------------ Make Movie Fuinctions --------------------------------

void CMandelbrotDlg::updateMovieMenuItem() {
  if(isMakingMovie()) {
    if(menuItemExists(this, ID_FILE_MAKEMOVIE)) {
      insertMenuItem(this, ID_FILE_MAKEMOVIE,0,_T("Stop &Movie...\tCtrl+O"), ID_FILE_STOPMOVIE);
      removeMenuItem(this, ID_FILE_MAKEMOVIE);
    }
  } else {
    if(menuItemExists(this, ID_FILE_STOPMOVIE)) {
      insertMenuItem(this, ID_FILE_STOPMOVIE,0,_T("Make &Movie...\tCtrl+M"), ID_FILE_MAKEMOVIE);
      removeMenuItem(this, ID_FILE_STOPMOVIE);
    }
  }
}

// --------------------------------------------------------------------------------------------------

void CMandelbrotDlg::startTimer(UINT id, int msec) {
  if(!m_runningTimerSet.contains(id) && SetTimer(id,msec,NULL)) {
    m_runningTimerSet.add(id);
  }
}

void CMandelbrotDlg::stopTimer(UINT id) {
  if(m_runningTimerSet.contains(id) ) {
    KillTimer(id);
    m_runningTimerSet.remove(id);
  }
}

void CMandelbrotDlg::OnTimer(UINT_PTR nIDEvent) {
  switch(nIDEvent) {
  case 1:
    break;
  case 2:
    showWindowState();
    break;
  }
  CDialog::OnTimer(nIDEvent);
}

const RealRectangleTransformation &DialogMBContainer::getTransformation() const {
  return m_dlg->getTransformation();
}

UINT DialogMBContainer::getMaxIteration() const {
  return m_dlg->getMaxIteration();
}

const D3DCOLOR *DialogMBContainer::getColorMap() const {
  return m_dlg->getColorMap();
}

FPUPrecisionMode DialogMBContainer::getPrecisionMode() const {
  return m_dlg->getPrecisionMode();
}

PixelAccessor *DialogMBContainer::getPixelAccessor() {
  return m_dlg->getPixelAccessor();
}

bool DialogMBContainer::paintOrbit() const {
  return m_dlg->paintOrbit();
}

bool DialogMBContainer::useEdgeDetection() const {
  return m_dlg->useEdgeDetection();
}

bool DialogMBContainer::getJobToDo(CRect &rect) {
  return m_dlg->getJobToDo(rect);
}

void DialogMBContainer::paintMark(const CPoint &p) {
  m_dlg->paintMark(p);
}

void DialogMBContainer::setPixel(UINT x, UINT y, D3DCOLOR color) {
  m_dlg->setPixel(x,y,color);
}

D3DCOLOR DialogMBContainer::getPixel(UINT x, UINT y) {
  return m_dlg->getPixel(x,y);
}

void DialogMBContainer::setPixel(const CPoint &p, D3DCOLOR color) {
  m_dlg->setPixel(p,color);
}

D3DCOLOR DialogMBContainer::getPixel(const CPoint &p) {
  return m_dlg->getPixel(p);
}

void DialogMBContainer::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  m_dlg->handlePropertyChanged(source, id, oldValue, newValue);
}

