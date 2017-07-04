#include "stdafx.h"
#include "MainFrm.h"
#include "Degreedlg.h"
#include "ScaleDlg.h"
#include "AdjustColorDialog.h"
#include "ApproximateFillDlg.h"
#include "EraseToolSizedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_NEW                        , OnFileNew                         )
    ON_COMMAND(ID_FILE_OPEN                       , OnFileOpen                        )
    ON_COMMAND(ID_FILE_INSERT                     , OnFileInsert                      )
    ON_COMMAND(ID_APP_EXIT                        , OnAppExit                         )
    ON_COMMAND(ID_FILE_MRU_FILE1                  , OnFileMruFile1                    )
    ON_COMMAND(ID_FILE_MRU_FILE2                  , OnFileMruFile2                    )
    ON_COMMAND(ID_FILE_MRU_FILE3                  , OnFileMruFile3                    )
    ON_COMMAND(ID_FILE_MRU_FILE4                  , OnFileMruFile4                    )
    ON_COMMAND(ID_FILE_MRU_FILE5                  , OnFileMruFile5                    )
    ON_COMMAND(ID_FILE_MRU_FILE6                  , OnFileMruFile6                    )
    ON_COMMAND(ID_FILE_MRU_FILE7                  , OnFileMruFile7                    )
    ON_COMMAND(ID_FILE_MRU_FILE8                  , OnFileMruFile8                    )
    ON_COMMAND(ID_FILE_MRU_FILE9                  , OnFileMruFile9                    )
    ON_COMMAND(ID_FILE_MRU_FILE10                 , OnFileMruFile10                   )
    ON_COMMAND(ID_FILE_MRU_FILE11                 , OnFileMruFile11                   )
    ON_COMMAND(ID_FILE_MRU_FILE12                 , OnFileMruFile12                   )
    ON_COMMAND(ID_FILE_MRU_FILE13                 , OnFileMruFile13                   )
    ON_COMMAND(ID_FILE_MRU_FILE14                 , OnFileMruFile14                   )
    ON_COMMAND(ID_FILE_MRU_FILE15                 , OnFileMruFile15                   )
    ON_COMMAND(ID_FILE_MRU_FILE16                 , OnFileMruFile16                   )
    ON_COMMAND(ID_FILE_GEM                        , OnFileSave                        )
    ON_COMMAND(ID_FILE_GEM_SOM                    , OnFileSaveAs                      )
    ON_COMMAND(ID_EDIT_UNDO                       , OnEditUndo                        )
    ON_COMMAND(ID_EDIT_REDO                       , OnEditRedo                        )
    ON_COMMAND(ID_EDIT_CUT                        , OnEditCut                         )
    ON_COMMAND(ID_EDIT_COPY                       , OnEditCopy                        )
    ON_COMMAND(ID_EDIT_PASTE                      , OnEditPaste                       )
    ON_COMMAND(ID_OPTIONS_COLOR                   , OnOptionsColor                    )
    ON_COMMAND(ID_OPTIONS_ZOOM_X1                 , OnOptionsZoomX1                   )
    ON_COMMAND(ID_OPTIONS_ZOOM_X2                 , OnOptionsZoomX2                   )
    ON_COMMAND(ID_OPTIONS_ZOOM_X4                 , OnOptionsZoomX4                   )
    ON_COMMAND(ID_OPTIONS_ZOOM_X8                 , OnOptionsZoomX8                   )
    ON_COMMAND(ID_OPTIONS_APPROXIMATEFILLTOLERANCE, OnOptionsApproximateFillTolerance )
    ON_COMMAND(ID_OPTIONS_GRABCOLOR               , OnOptionsGrabColor                )
    ON_COMMAND(ID_TOOLS_PEN                       , OnToolsPen                        )
    ON_COMMAND(ID_TOOLS_LINE                      , OnToolsLine                       )
    ON_COMMAND(ID_TOOLS_RECTANGLE                 , OnToolsRectangle                  )
    ON_COMMAND(ID_TOOLS_POLYGON                   , OnToolsPolygon                    )
    ON_COMMAND(ID_TOOLS_ELLIPSE                   , OnToolsEllipse                    )
    ON_COMMAND(ID_TOOLS_TEXT                      , OnToolsText                       )
    ON_COMMAND(ID_TOOLS_DRAWTEXT                  , OnToolsDrawtext                   )
    ON_COMMAND(ID_TOOLS_FILL                      , OnToolsFill                       )
    ON_COMMAND(ID_TOOLS_FILLTRANSPARENT           , OnToolsFillTransparent            )
    ON_COMMAND(ID_TOOLS_APPROXIMATEFILL           , OnToolsApproximateFill            )
    ON_COMMAND(ID_TOOLS_CHANGEHUE                 , OnToolsChangeHue                  )
    ON_COMMAND(ID_TOOLS_MOVERECTANGLE             , OnToolsMoveRectangle              )
    ON_COMMAND(ID_TOOLS_MOVEREGION                , OnToolsMoveRegion                 )
    ON_COMMAND(ID_TOOLS_ERASE                     , OnToolsErase                      )
    ON_COMMAND(ID_FUNCTION_ROTATE                 , OnFunctionRotate                  )
    ON_COMMAND(ID_FUNCTION_SCALE                  , OnFunctionScale                   )
    ON_COMMAND(ID_FUNCTION_MIRROR_HORIZONTAL      , OnFunctionMirrorHorizontal        )
    ON_COMMAND(ID_FUNCTION_MIRROR_VERTICAL        , OnFunctionMirrorVertical          )
    ON_COMMAND(ID_FUNCTION_MAKEGRAYSCALE          , OnFunctionMakegrayscale           )
    ON_COMMAND(ID_FUNCTION_CHANGEHUE              , OnFunctionChangeHue               )
    ON_COMMAND(ID_FUNCTION_SOBELEDGEDETECTION     , OnFunctionSobelEdgeDetection      )
    ON_COMMAND(ID_FUNCTION_LAPACEEDGEDETECTION    , OnFunctionLapaceEdgeDetection     )
    ON_COMMAND(ID_FUNCTION_CANNYEDGEDETECTION     , OnFunctionCannyEdgeDetection      )
    ON_COMMAND(ID_FUNCTION_DIRECTIONFILTER        , OnFunctionDirectionalFilter       )
    ON_COMMAND(ID_FUNCTION_GAUSSFILTER            , OnFunctionGaussFilter             )
    ON_COMMAND(ID_SCROLL_LINE_DOWN                , OnScrollLineDown                  )
    ON_COMMAND(ID_SCROLL_LINE_UP                  , OnScrollLineUp                    )
    ON_COMMAND(ID_SCROLL_PAGE_DOWN                , OnScrollPageDown                  )
    ON_COMMAND(ID_SCROLL_PAGE_UP                  , OnScrollPageUp                    )
    ON_COMMAND(ID_SCROLL_LEFT                     , OnScrollLeft                      )
    ON_COMMAND(ID_SCROLL_RIGHT                    , OnScrollRight                     )
    ON_COMMAND(ID_SCROLL_PAGE_LEFT                , OnScrollPageLeft                  )
    ON_COMMAND(ID_SCROLL_PAGE_RIGHT               , OnScrollPageRight                 )
    ON_COMMAND(ID_SCROLL_TO_TOP                   , OnScrollToTop                     )
    ON_COMMAND(ID_SCROLL_TO_BOTTOM                , OnScrollToBottom                  )
    ON_COMMAND(ID_SCROLL_TO_LEFT                  , OnScrollToLeft                    )
    ON_COMMAND(ID_SCROLL_TO_RIGHT                 , OnScrollToRight                   )
    ON_COMMAND(ID_POPTOOL                         , OnPopTool                         )
    ON_COMMAND(ID_DELETE                          , OnDelete                          )
END_MESSAGE_MAP()

static UINT indicators[] = {
  ID_SEPARATOR           // status line indicator
 ,IDS_MOUSEPOSITION
// ,IDS_INFOFIELD
};

CMainFrame::CMainFrame() {
  m_currentColor             = BLACK;
  m_currentDegree            = 0;
  m_eraseToolSize            = CSize(10,5);
  m_approximateFillTolerance = 1;
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(__super::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
      | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
  {
    TRACE0("Failed to create toolbar\n");
    return -1;      // fail to create
  }

  if(!m_wndStatusBar.Create(this) ||!m_wndStatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT))) {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  m_bAutoMenuEnable = false;
  theApp.m_device.attach(*this);

  return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
  if(!__super::PreCreateWindow(cs)) {
    return FALSE;
  }
  return TRUE;
}

BOOL CMainFrame::PreTranslateMessage(MSG *pMsg) {
  BOOL result;
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    result = TRUE;
  } else {
    result = __super::PreTranslateMessage(pMsg);
  }
  CMyPaintView *view = getView();
  if(view->isMouseOnDocument()) {
    CPoint mp = getView()->getCurrentMousePoint();
    m_wndStatusBar.SetPaneText(1,format(_T("%3d,%3d px"),mp.x,mp.y).cstr());
  } else {
    m_wndStatusBar.SetPaneText(1,EMPTYSTRING);
  }

//m_wndStatusBar.SetPaneText(2,getDocument()->getInfo().cstr());
  ajourRedoUndo();
  return result;
}

void CMainFrame::ajourRedoUndo() {
  enableMenuItem(this,ID_EDIT_UNDO,getDocument()->canUndo());
  enableMenuItem(this,ID_EDIT_REDO,getDocument()->canRedo());
}


#ifdef _DEBUG
void CMainFrame::AssertValid() const {
  __super::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

#endif //_DEBUG

CMyPaintDoc *CMainFrame::getDocument() {
  CMyPaintView *view = getView();
  return view ? view->GetDocument() : NULL;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);

  if(getView() != NULL && !hasDrawTool()) {
    OnToolsPen();
  }
}

void CMainFrame::OnFileNew() {
  if(!checkSave()) {
    return;
  }
  getDocument()->OnNewDocument();
  updateTitle();
  getView()->refreshDoc();
}

static String getLoadFileName() {
  static const TCHAR *loadFileDialogExtensions = _T("Picture files\0*.bmp;*.dib;*.jpg;*.emf;*.gif;*.ico;*.wmf;\0"
                                                    "BMP-Files (*.bmp)\0*.bmp;\0"
                                                    "DIB-files (*.dib)\0*.dib;\0"
                                                    "JPG-files (*.jpg)\0*.jpg;\0"
                                                    "TIFF-files (*.tiff)\0*.tiff;\0"
                                                    "GIF-files (*.gif)\0*.gif;\0"
                                                    "ICO-files (*.ico)\0*.ico;\0"
                                                    "CUR-files (*.cur)\0*.cur;\0"
                                                    "All files (*.*)\0*.*\0\0");
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = loadFileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open file");
  dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING;

  if(dlg.DoModal() != IDOK) {
    return EMPTYSTRING;
  } else {
    return dlg.m_ofn.lpstrFile;
  }
}

void CMainFrame::OnFileOpen() {
  if(!checkSave()) {
    return;
  }

  const String fileName = getLoadFileName();
  if(fileName.length() == 0) {
    return;
  }
  loadFile(fileName);
}

void CMainFrame::OnFileInsert() {
  const String fileName = getLoadFileName();
  if(fileName.length() == 0) {
    return;
  }
  try {
    PixRect *image = PixRect::load(theApp.m_device, ByteInputFile(fileName));
    pushTool(new InsertImageTool(getView(),image));
  } catch(Exception e) {
    MessageBox(e.what(),_T("Error"),MB_ICONWARNING);
  }
}

void CMainFrame::OnFileMruFile1()  { onFileMruFile( 0); }
void CMainFrame::OnFileMruFile2()  { onFileMruFile( 1); }
void CMainFrame::OnFileMruFile3()  { onFileMruFile( 2); }
void CMainFrame::OnFileMruFile4()  { onFileMruFile( 3); }
void CMainFrame::OnFileMruFile5()  { onFileMruFile( 4); }
void CMainFrame::OnFileMruFile6()  { onFileMruFile( 5); }
void CMainFrame::OnFileMruFile7()  { onFileMruFile( 6); }
void CMainFrame::OnFileMruFile8()  { onFileMruFile( 7); }
void CMainFrame::OnFileMruFile9()  { onFileMruFile( 8); }
void CMainFrame::OnFileMruFile10() { onFileMruFile( 9); }
void CMainFrame::OnFileMruFile11() { onFileMruFile(10); }
void CMainFrame::OnFileMruFile12() { onFileMruFile(11); }
void CMainFrame::OnFileMruFile13() { onFileMruFile(12); }
void CMainFrame::OnFileMruFile14() { onFileMruFile(13); }
void CMainFrame::OnFileMruFile15() { onFileMruFile(14); }
void CMainFrame::OnFileMruFile16() { onFileMruFile(15); }

void CMainFrame::onFileMruFile(int index) {
  if(!checkSave()) {
    return;
  }
  const String name = theApp.getRecentFile(index);
  loadFile(name);
}

void CMainFrame::loadFile(const String &fileName) {
  resetCurrentDrawTool();
  try {
    getDocument()->OnOpenDocument(fileName.cstr());
    updateTitle();
    getView()->clear();
    getView()->refreshDoc();
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CMainFrame::updateTitle() {
  CMyPaintDoc *doc = getDocument();
  CString title    = doc->GetTitle();
  CSize   size     = doc->getSize();
  setWindowText(this, format(_T("%s - %dx%d"), (LPCTSTR)title, size.cx,size.cy));
}

static const TCHAR *saveFileDialogExtensions = _T("Bitmap files (*.bmp)\0*.bmp;\0"
                                                  "JPEG files (*.jpg)\0*.jpg;\0"
                                                  "TIFF files (*.tiff)\0*.tiff;\0"
                                                  "All files (*.*)\0*.*\0\0");

void CMainFrame::OnFileSave() {
  onFileSave();
}

bool CMainFrame::onFileSave() { // retuns true if succeeded
  CMyPaintDoc *doc = getDocument();
  CString name = doc->GetPathName();
  if(doc->hasDefaultName()) {
    CFileDialog dlg(FALSE,_T("bmp"),name);
    dlg.m_ofn.lpstrTitle  = _T("Save image");
    dlg.m_ofn.lpstrFilter = saveFileDialogExtensions;
    if(dlg.DoModal() == IDOK) {
      name = dlg.m_ofn.lpstrFile;
    } else {
      return false;
    }
  }
  return save(name);
}

void CMainFrame::OnFileSaveAs() {
  CString name = getDocument()->GetPathName();
  FileNameSplitter info((LPCTSTR)name);
  CFileDialog dlg(FALSE,info.getExtension().cstr(),name);
  dlg.m_ofn.lpstrFilter = saveFileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save image");
  if(dlg.DoModal() == IDOK) {
    name = dlg.m_ofn.lpstrFile;
    save(name);
  }
}

bool CMainFrame::save(const CString &name) { // returns true if succeeded
  try {
    getDocument()->OnSaveDocument(name);
    updateTitle();
    return true;
  } catch(Exception e) {
    MessageBox(e.what());
    return false;
  }
}

void CMainFrame::saveDocState() {
  getDocument()->saveState();
}

void CMainFrame::OnEditUndo() {
  if(getDocument()->undo()) {
    getView()->repaint();
    updateTitle();
  }
}

void CMainFrame::OnEditRedo() {
  if(getDocument()->redo()) {
    getView()->repaint();
    updateTitle();
  }
}

void CMainFrame::OnEditCut() {
  if(hasDrawTool()) {
    getCurrentDrawTool()->cut();
  }
}

void CMainFrame::OnEditCopy() {
  if(hasDrawTool()) {
    getCurrentDrawTool()->copy();
  }
}

void CMainFrame::OnEditPaste() {
  HBITMAP bitmap = getClipboardBitmap();
  if(bitmap != NULL) {
    pushTool(new InsertImageTool(getView(),new PixRect(theApp.m_device, bitmap)));
  }
}

void CMainFrame::OnOptionsColor() {
  CColorDialog dlg;
  dlg.m_cc.Flags |= CC_RGBINIT;
  dlg.m_cc.rgbResult = D3DCOLOR2COLORREF(m_currentColor);
  if(dlg.DoModal() == IDOK) {
    m_currentColor = COLORREF2D3DCOLOR(dlg.m_cc.rgbResult);
  }
}

void CMainFrame::OnOptionsZoomX1() {
  setCurrentZoomFactor(ID_OPTIONS_ZOOM_X1);
}

void CMainFrame::OnOptionsZoomX2() {
  setCurrentZoomFactor(ID_OPTIONS_ZOOM_X2);
}

void CMainFrame::OnOptionsZoomX4() {
  setCurrentZoomFactor(ID_OPTIONS_ZOOM_X4);
}

void CMainFrame::OnOptionsZoomX8() {
  setCurrentZoomFactor(ID_OPTIONS_ZOOM_X8);
}

static int zoomItems[] = { ID_OPTIONS_ZOOM_X1, ID_OPTIONS_ZOOM_X2, ID_OPTIONS_ZOOM_X4, ID_OPTIONS_ZOOM_X8 };

void CMainFrame::setCurrentZoomFactor(int id) {
  for(int i = 0; i < ARRAYSIZE(zoomItems); i++) {
    checkMenuItem(this,zoomItems[i],false);
  }
  checkMenuItem(this,id,true);
  switch(id) {
  case ID_OPTIONS_ZOOM_X1: getView()->setCurrentZoomFactor(1); break;
  case ID_OPTIONS_ZOOM_X2: getView()->setCurrentZoomFactor(2); break;
  case ID_OPTIONS_ZOOM_X4: getView()->setCurrentZoomFactor(4); break;
  case ID_OPTIONS_ZOOM_X8: getView()->setCurrentZoomFactor(8); break;
  default                : getView()->setCurrentZoomFactor(1); break;
  }
}


void CMainFrame::OnOptionsApproximateFillTolerance() {
  CApproximateFillDlg dlg(m_approximateFillTolerance);
  if(dlg.DoModal() == IDOK) {
    m_approximateFillTolerance = dlg.getTolerance();
  }
}

void CMainFrame::OnOptionsGrabColor() {
}

void CMainFrame::OnToolsPen() {
  setCurrentDrawTool(ID_TOOLS_PEN);
}

void CMainFrame::OnToolsLine() {
  setCurrentDrawTool(ID_TOOLS_LINE);
}

void CMainFrame::OnToolsRectangle() {
  setCurrentDrawTool(ID_TOOLS_RECTANGLE);
}

void CMainFrame::OnToolsPolygon() {
  setCurrentDrawTool(ID_TOOLS_POLYGON);
}

void CMainFrame::OnToolsEllipse() {
  setCurrentDrawTool(ID_TOOLS_ELLIPSE);
}

void CMainFrame::OnToolsText() {
  setCurrentDrawTool(ID_TOOLS_TEXT);
}

void CMainFrame::OnToolsDrawtext() {
  setCurrentDrawTool(ID_TOOLS_DRAWTEXT);
}

void CMainFrame::OnToolsFill() {
  setCurrentDrawTool(ID_TOOLS_FILL);
}

void CMainFrame::OnToolsFillTransparent() {
  setCurrentDrawTool(ID_TOOLS_FILLTRANSPARENT);
}

void CMainFrame::OnToolsApproximateFill() {
  setCurrentDrawTool(ID_TOOLS_APPROXIMATEFILL);
}

void CMainFrame::OnToolsChangeHue() {
  setCurrentDrawTool(ID_TOOLS_CHANGEHUE);
}

void CMainFrame::OnToolsMoveRectangle() {
  setCurrentDrawTool(ID_TOOLS_MOVERECTANGLE);
}

void CMainFrame::OnToolsMoveRegion() {
  setCurrentDrawTool(ID_TOOLS_MOVEREGION);
}

void CMainFrame::OnToolsErase() {
  setCurrentDrawTool(ID_TOOLS_ERASE);
}

void CMainFrame::OnFunctionRotate() {
  CDegreeDlg dlg(m_currentDegree);
  if(dlg.DoModal() != IDOK) {
    return;
  }
  m_currentDegree = dlg.getDegree();

  theApp.BeginWaitCursor();

  CMyPaintDoc *doc = getDocument();
  saveDocState();
  doc->setImage(PixRect::rotateImage(doc->getImage(),m_currentDegree));
  theApp.EndWaitCursor();
  getView()->refreshDoc();
  updateTitle();
}

void CMainFrame::OnFunctionScale() {
  CScaleDlg dlg(m_currentScale);
  if(dlg.DoModal() != IDOK) {
    return;
  }
  m_currentScale = dlg.getScaleParameters();

  theApp.BeginWaitCursor();

  CMyPaintDoc *doc = getDocument();
  saveDocState();
  doc->setImage(PixRect::scaleImage(doc->getImage(),m_currentScale));

  theApp.EndWaitCursor();
  getView()->refreshDoc();
  updateTitle();
}

void CMainFrame::OnFunctionMirrorHorizontal()    { applyMirror(false);                 }
void CMainFrame::OnFunctionMirrorVertical()      { applyMirror(true);                  }
void CMainFrame::OnFunctionMakegrayscale()       { applyFilter(GrayScaleFilter());     }
void CMainFrame::OnFunctionSobelEdgeDetection()  { applyFilter(SobelFilter());         }
void CMainFrame::OnFunctionLapaceEdgeDetection() { applyFilter(LaplaceFilter());       }
void CMainFrame::OnFunctionCannyEdgeDetection()  { applyFilter(CannyEdgeFilter());     }
void CMainFrame::OnFunctionGaussFilter()         { applyFilter(GaussFilter());         }
void CMainFrame::OnFunctionDirectionalFilter()   { applyFilter(EdgeDirectionFilter()); }

void CMainFrame::OnFunctionChangeHue() {
  CAdjustColorDialog dlg(*getView(),this);
  dlg.DoModal();
}

void CMainFrame::applyMirror(bool vertical) {
  theApp.BeginWaitCursor();
  saveDocState();
  CMyPaintDoc *doc = getDocument();
  doc->setImage(PixRect::mirror(doc->getImage(),vertical));
  Invalidate();
  theApp.EndWaitCursor();
}

void CMainFrame::applyFilter(PixRectFilter &filter) {
  theApp.BeginWaitCursor();
  saveDocState();
  getDocument()->getImage()->apply(filter);
  Invalidate();
  theApp.EndWaitCursor();
}

void CMainFrame::OnPopTool() {
  popTool();
}

void CMainFrame::OnDelete() {
  getCurrentDrawTool()->OnDelete();
}

void CMainFrame::setCurrentDrawTool(int id) {
  checkToolItem(id);
  switch(id) {
  case ID_TOOLS_PEN:
    setCurrentDrawTool(new PenTool(getView()));
    break;
  case ID_TOOLS_LINE:
    setCurrentDrawTool(new LineTool(getView()));
    break;
  case ID_TOOLS_RECTANGLE:
    setCurrentDrawTool(new RectangleTool(getView()));
    break;
  case ID_TOOLS_POLYGON:
    setCurrentDrawTool(new PolygonTool(getView()));
    break;
  case ID_TOOLS_ELLIPSE:
    setCurrentDrawTool(new EllipseTool(getView()));
    break;
  case ID_TOOLS_TEXT:
    setCurrentDrawTool(new TextTool(getView(),false,m_currentFontParameters,m_currentText));
    break;
  case ID_TOOLS_DRAWTEXT:
    setCurrentDrawTool(new TextTool(getView(),true,m_currentFontParameters,m_currentText));
    break;
  case ID_TOOLS_FILL:
    setCurrentDrawTool(new FillTool(getView()));
    break;
  case ID_TOOLS_FILLTRANSPARENT:
    setCurrentDrawTool(new FillTransparentTool(getView()));
    break;
  case ID_TOOLS_APPROXIMATEFILL:
    setCurrentDrawTool(new ApproximateFillTool(getView()));
    break;
  case ID_TOOLS_CHANGEHUE      :
    setCurrentDrawTool(new ChangeHueTool(getView()));
    break;
  case ID_TOOLS_MOVERECTANGLE:
    setCurrentDrawTool(new MoveRectangleTool(getView()));
    break;
  case ID_TOOLS_MOVEREGION:
    setCurrentDrawTool(new MoveRegionTool(getView()));
    break;
  case ID_TOOLS_ERASE:
    setCurrentDrawTool(new EraseTool(getView()));
    break;
  default:
    setCurrentDrawTool(new NullTool());
    break;
  }
}

void CMainFrame::resetCurrentDrawTool() {
  if(hasDrawTool()) {
    getCurrentDrawTool()->reset();
  }
}

void CMainFrame::setCurrentDrawTool(DrawTool *newDrawTool) {
  if(hasDrawTool()) {
    popTool();
  }
  pushTool(newDrawTool);
}

void CMainFrame::popTool() {
  DrawTool *tool = m_toolStack.pop();
  delete tool;
}

void CMainFrame::pushTool(DrawTool *tool) {
  if(hasDrawTool() && getCurrentDrawTool()->getType() == INSERTIMAGETOOL && tool->getType() == INSERTIMAGETOOL) {
    popTool();
  }
  m_toolStack.push(tool);
}

static int toolItems[] = { ID_TOOLS_PEN          , ID_TOOLS_LINE
                         , ID_TOOLS_RECTANGLE    , ID_TOOLS_POLYGON        , ID_TOOLS_ELLIPSE
                         , ID_TOOLS_TEXT         , ID_TOOLS_DRAWTEXT
                         , ID_TOOLS_FILL         , ID_TOOLS_FILLTRANSPARENT, ID_TOOLS_APPROXIMATEFILL
                         , ID_TOOLS_CHANGEHUE
                         , ID_TOOLS_MOVERECTANGLE, ID_TOOLS_MOVEREGION
                         , ID_TOOLS_ERASE
};

void CMainFrame::checkToolItem(int id) {
  for(int i = 0; i < ARRAYSIZE(toolItems); i++) {
    checkMenuItem(this,toolItems[i],false);
  }
  checkMenuItem(this,id,true);
}

CPoint CMainFrame::getMaxScroll() {
  CRect r;
  getView()->GetClientRect(&r);
  CSize rectSize = r.Size();
  CSize docSize = getDocument()->getSize();
  int zoom = getView()->getCurrentZoomFactor();
  return CPoint(max(0,docSize.cx*zoom-rectSize.cx),max(0,docSize.cy*zoom-rectSize.cy));
}

void CMainFrame::scroll(int dx, int dy) {
  CPoint topLeft   = getView()->GetScrollPosition();
  CPoint maxScroll = getMaxScroll();
  int newX = minMax(topLeft.x+dx,0,maxScroll.x);
  int newY = minMax(topLeft.y+dy,0,maxScroll.y);

  getView()->ScrollToPosition(CPoint(newX,newY));
}

void CMainFrame::OnScrollLineDown() {
  scroll(0,20);
}

void CMainFrame::OnScrollLineUp() {
  scroll(0,-20);
}

void CMainFrame::OnScrollPageDown() {
  CRect r;
  getView()->GetClientRect(&r);
  scroll(0,r.Height());
}

void CMainFrame::OnScrollPageUp() {
  CRect r;
  getView()->GetClientRect(&r);
  scroll(0,-r.Height());
}

void CMainFrame::OnScrollLeft() {
  scroll(-20,0);
}

void CMainFrame::OnScrollRight() {
  scroll(20,0);
}

void CMainFrame::OnScrollPageLeft() {
  CRect r;
  getView()->GetClientRect(&r);
  scroll(-r.Width(),0);
}

void CMainFrame::OnScrollPageRight() {
  CRect r;
  getView()->GetClientRect(&r);
  scroll(r.Width(),0);
}

void CMainFrame::OnScrollToTop() {
  CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(topLeft.x,0));
}

void CMainFrame::OnScrollToBottom() {
  CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(topLeft.x,getMaxScroll().y));
}

void CMainFrame::OnScrollToLeft() {
  CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(0,topLeft.y));
}

void CMainFrame::OnScrollToRight() {
  CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(getMaxScroll().x,topLeft.y));
}

void CMainFrame::OnAppExit() {
  if(!checkSave()) {
    return;
  }
  PostMessage(WM_QUIT);
}

void CMainFrame::OnClose() {
  OnAppExit();
}

bool CMainFrame::checkSave() { // return true to continue user action
  if(!getDocument()->IsModified()) {
    return true;
  }

  switch(MessageBox( _T("       Save changes"),_T("Save"),MB_YESNOCANCEL | MB_ICONQUESTION)) {
  case IDYES   :
    return onFileSave();
  case IDNO    :
    return true;
  case IDCANCEL:
    return false;
  }
  return false;
}
