#include "stdafx.h"
#include <Process.h>
#include <MFCUtil/ProgressWindow.h>
#include "TestExpressionGraphicsDlg.h"
#include "EnterVariablesDlg.h"
#include "ExpressionTreeDlg.h"
#include "TestTreesEqualDlg.h"
#include "../../Test/testexpr/ExpressionSamples.h"

#define APSTUDIO_INVOKED
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FIRST_SAMPLE_COMMAND _APS_NEXT_COMMAND_VALUE
#define LAST_SAMPLE_COMMAND  FIRST_SAMPLE_COMMAND + 200

class CAboutDlg : public CDialog {
public:
  CAboutDlg();
  enum { IDD = IDD_ABOUTBOX };

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTestExpressionGraphicsDlg::CTestExpressionGraphicsDlg(CWnd *pParent /*=NULL*/) : CDialog(CTestExpressionGraphicsDlg::IDD, pParent), m_numberFormat(0) {
    m_exprText         = EMPTYSTRING;
    m_x                = 0.0;
    m_hIcon            = theApp.LoadIcon(IDR_MAINFRAME);
    m_debugExpr        = NULL;
    m_debugThread      = NULL;
    m_debugWinId       = -1;
    m_contextWinId     = -1;
    m_contextRect      = NULL;
    m_leastContextRect = NULL;
    m_currentChildDlg  = NULL;
}

CTestExpressionGraphicsDlg::~CTestExpressionGraphicsDlg() {
}

void CTestExpressionGraphicsDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITX, m_x);
  DDX_CBString(pDX, IDC_COMBOEXPR, m_exprText);
}

#define INVALIDATE() Invalidate(FALSE)

BEGIN_MESSAGE_MAP(CTestExpressionGraphicsDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_QUERYDRAGICON()
    ON_WM_PAINT()
    ON_WM_CLOSE()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(       ID_CONTEXTMENU_SHOWEXPRTREE   , OnContextMenuShowExprTree      )
    ON_COMMAND(       ID_CONTEXTMENU_TOSTANDARDFORM , OnContextMenuToStandardForm    )
    ON_COMMAND(       ID_CONTEXTMENU_TOCANONCALFORM , OnContextMenuToCanoncalForm    )
    ON_COMMAND(       ID_CONTEXTMENU_TONUMERICFORM  , OnContextMenuToNumericForm     )
    ON_COMMAND(       ID_CONTEXTMENU_SHOWNODETREE   , OnContextMenuShowNodeTree      )
    ON_COMMAND(       ID_CONTEXTMENU_EXPAND         , OnContextMenuExpand            )
    ON_COMMAND(       ID_CONTEXTMENU_MULTIPLY       , OnContextMenuMultiply          )
    ON_COMMAND(       ID_GOTOX                      , OnGotoX                        )
    ON_COMMAND(       ID_GOTOFONTSIZE               , OnGotoFontSize                 )
    ON_COMMAND(       ID_GOTONUMBERFORMAT           , OnGotoNumberFormat             )
    ON_CBN_SELCHANGE( IDC_COMBOEXPR                 , OnCbnSelChangeComboExpr        )
    ON_CBN_EDITCHANGE(IDC_COMBOEXPR                 , OnCbnEditChangeComboExpr       )
    ON_EN_CHANGE(     IDC_EDITX                     , OnChangeEditX                  )
    ON_CBN_SELCHANGE( IDC_COMBONUMBERFORMAT         , OnSelchangeComboNumberFormat   )
    ON_CBN_SELCHANGE( IDC_COMBOFONTSIZE             , OnSelChangeComboFontSize       )
    ON_COMMAND(       ID_FILE_EXIT                  , OnFileExit                     )
    ON_COMMAND(       ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis   )
    ON_COMMAND(       ID_EDIT_GOTOCOMBOFX           , OnEditGotoComboFx              )
    ON_COMMAND(       ID_EDIT_ENTERPARAMETERS       , OnEditEnterParameters          )
    ON_COMMAND(       ID_VIEW_SHOWREDUCTIONSTACK    , OnViewShowReductionStack       )
    ON_COMMAND(       ID_VIEW_SHOWRECTANGLES        , OnViewShowRectangles           )
    ON_COMMAND(       ID_DEBUG_REDUCEEXPR           , OnDebugReduceExpr              )
    ON_COMMAND(       ID_DEBUG_REDUCEDERIVED        , OnDebugReduceDerived           )
    ON_COMMAND(       ID_DEBUG_RUN                  , OnDebugRun                     )
    ON_COMMAND(       ID_DEBUG_STOP                 , OnDebugStop                    )
    ON_COMMAND(       ID_DEBUG_CLEARALLBREAKPOINTS  , OnDebugClearAllBreakPoints     )
    ON_COMMAND(       ID_DEBUG_TRACEREDUCTIONSTEP   , OnDebugTraceReductionStep      )
    ON_COMMAND(       ID_DEBUG_STEP1REDUCEITERATION , OnDebugStep1ReduceIteration    )
    ON_COMMAND(       ID_DEBUG_TESTTREESEQUAL       , OnDebugTestTreesEqual          )
    ON_COMMAND(       ID_FUNCTIONS_COMPILEFX        , OnFunctionsCompileFx           )
    ON_COMMAND(       ID_FUNCTIONS_DERIVEFX         , OnFunctionsDeriveFx            )
    ON_COMMAND(       ID_FUNCTIONS_EVALUATEFX       , OnFunctionsEvaluateFx          )
    ON_COMMAND(       ID_FUNCTIONS_EVALUATEDERIVED  , OnFunctionsEvaluateDerived     )
    ON_COMMAND(       ID_FUNCTIONS_REDUCEFX         , OnFunctionsReduceFx            )
    ON_COMMAND(       ID_FUNCTIONS_REDUCEDERIVED    , OnFunctionsReduceDerived       )
    ON_COMMAND(       ID_FUNCTIONS_EVALUATEALL      , OnFunctionsEvaluateAll         )
    ON_COMMAND_RANGE( FIRST_SAMPLE_COMMAND,LAST_SAMPLE_COMMAND, OnSamplesSampleId    )
    ON_MESSAGE(       ID_MSG_RUNSTATE_CHANGED       , OnMsgRunStateChanged           )
    ON_MESSAGE(       ID_MSG_SHOW_DEBUGERROR        , OnMsgShowDebugError            )
    ON_COMMAND(       ID_SAMPLES_RUNALL             , OnSamplesRunall                )
END_MESSAGE_MAP()

BOOL CTestExpressionGraphicsDlg::OnInitDialog() {
//    debugLog(_T("sizeof(ExpressionNode):%d\n"), sizeof(ExpressionNode));
  __super::OnInitDialog();

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

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  m_accelTabel = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  m_reductionStackWindow.substituteControl(this, IDC_STATICREDUCTIONSTACK);
  m_exprCombo.substituteControl(           this, IDC_COMBOEXPR, _T("ExprHistory"));
  m_layoutManager.OnInitDialog(this);

  m_layoutManager.addControl(IDC_COMBOEXPR            , RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATICXLABEL         , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_EDITX                , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_STATICRESULTLABEL    , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_EDITRESULTVALUE      , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_STATICFORMATLABEL    , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_COMBONUMBERFORMAT    , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_STATICDERIVEDLABEL1  , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_EDITDERIVEDVALUE1    , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_STATICDERIVEDLABEL2  , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_EDITDERIVEDVALUE2    , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_STATICDEBUGINFO      , PCT_RELATIVE_RIGHT);
  m_layoutManager.addControl(IDC_STATICCONTEXT        , PCT_RELATIVE_LEFT   | RELATIVE_RIGHT);
  m_layoutManager.addControl(IDC_STATICFONTSIZE       , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_COMBOFONTSIZE        , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_STATICEXPRIMAGE      , PCT_RELATIVE_RIGHT  | PCT_RELATIVE_BOTTOM);
  m_layoutManager.addControl(IDC_STATICDERIVEDLABEL   , PCT_RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_EDITDERIVED          , PCT_RELATIVE_RIGHT  | PCT_RELATIVE_Y_POS );
  m_layoutManager.addControl(IDC_STATICDERIVEDIMAGE   , PCT_RELATIVE_RIGHT  | PCT_RELATIVE_TOP | RELATIVE_BOTTOM );
  m_layoutManager.addControl(IDC_STATICREDUCTIONSTACK , PCT_RELATIVE_LEFT   | RELATIVE_RIGHT | RELATIVE_HEIGHT);

  m_device.attach(*this);

#ifndef TRACE_REDUCTION_CALLSTACK
  enableMenuItem(this, ID_VIEW_SHOWREDUCTIONSTACK , false);
#endif

  loadOptions();
  OnEditGotoComboFx();

  buildSamplesMenu();
  return FALSE;
}

void CTestExpressionGraphicsDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTestExpressionGraphicsDlg::callNotepad(const String &s) {

#define TMPFNAME "c:\\temp\\exprDump.txt"

  FILE *f = mkfopen(TMPFNAME,_T("w"));
  _ftprintf(f, _T("%s"), s.cstr());
  fclose(f);
  intptr_t ret = _spawnl(_P_NOWAIT,"c:\\windows\\system32\\notepad.exe", "notepad", TMPFNAME, NULL);
}

void CTestExpressionGraphicsDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
    paintExprImage();
    paintDerivedImage();
    ajourDialogItems();
  }
}

HCURSOR CTestExpressionGraphicsDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

BOOL CTestExpressionGraphicsDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTabel, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CTestExpressionGraphicsDlg::buildSamplesMenu() {
  int index;
  HMENU samplesMenu    = findMenuContainingId(*GetMenu(), ID_SAMPLES_RUNALL, index);
  HMENU currentSubMenu = NULL;
  int   subMenuCount   = 0, itemsInSubMenu;
  const size_t count = ExpressionSamples::getCount();
#define SAMPLESPERMENU 20
  for(UINT i = 0; i < count; i++) {
    if(currentSubMenu == NULL) {
      currentSubMenu = insertSubMenu(samplesMenu, subMenuCount++, format(_T("%2d - %2d"), i, i + SAMPLESPERMENU-1));
      itemsInSubMenu = 0;
    }
    const TCHAR *text = ExpressionSamples::getSample(i);
    insertMenuItem(currentSubMenu, itemsInSubMenu++, text, FIRST_SAMPLE_COMMAND + i);
    if(itemsInSubMenu == SAMPLESPERMENU) {
      currentSubMenu = NULL;
    }
  }
}

void CTestExpressionGraphicsDlg::OnFileExit() {
  clearExprImage();
  clearDerivedExpr();
  destroyThread();
  EndDialog(IDOK);
}

void CTestExpressionGraphicsDlg::OnEditFindMatchingParentesis() {
  gotoMatchingParanthes(&m_exprCombo);
}

void CTestExpressionGraphicsDlg::OnEditGotoComboFx() {
  m_exprCombo.SetFocus();
}

void CTestExpressionGraphicsDlg::OnEditEnterParameters() {
  if(!m_flags.contains(ISCOMPILED)) {
    return;
  }
  CEnterVariablesDlg dlg(m_expr);
  if(dlg.DoModal() == IDOK) {
    OnFunctionsEvaluateFx();
  }
}

void CTestExpressionGraphicsDlg::OnViewShowReductionStack() {
#ifdef TRACE_REDUCTION_CALLSTACK
  const bool showStack = toggleMenuItem(this, ID_VIEW_SHOWREDUCTIONSTACK);

  CWnd *stackWindow       = GetDlgItem(IDC_STATICREDUCTIONSTACK);
  CWnd *exprImage         = GetDlgItem(IDC_STATICEXPRIMAGE);
  CWnd *derivedImage      = GetDlgItem(IDC_STATICDERIVEDIMAGE);
  CWnd *derivedTextWindow = GetDlgItem(IDC_EDITDERIVED);
  CRect stackRect         = getWindowRect(stackWindow);
  CRect r1                = getWindowRect(exprImage);
  CRect r2                = getWindowRect(derivedImage);
  CRect r3                = getWindowRect(derivedTextWindow);
  const CRect cl          = getClientRect(this);
  if(showStack) {
    stackRect.top = r1.top;
    setWindowRect(stackWindow, stackRect);
    stackWindow->ShowWindow(SW_SHOW);
    r1.right = r2.right = r3.right = stackRect.left;
  } else {
    stackWindow->ShowWindow(SW_HIDE);
    r1.right = r2.right = r3.right = cl.Size().cx;
  }
  setWindowRect(exprImage        ,r1);
  setWindowRect(derivedImage     ,r2);
  setWindowRect(derivedTextWindow,r3);
#endif
}


void CTestExpressionGraphicsDlg::OnViewShowRectangles() {
  toggleMenuItem(this, ID_VIEW_SHOWRECTANGLES);
  INVALIDATE();
}

void CTestExpressionGraphicsDlg::OnGotoX() {
  gotoEditBox(this, IDC_EDITX);
}

void CTestExpressionGraphicsDlg::OnGotoFontSize() {
  getFontSizeCombo()->SetFocus();
  getFontSizeCombo()->ShowDropDown();
}

void CTestExpressionGraphicsDlg::OnGotoNumberFormat() {
  getNumberFormatCombo()->SetFocus();
  getNumberFormatCombo()->ShowDropDown();

}

NumberFormat CTestExpressionGraphicsDlg::getNumberFormat() {
  CComboBox *cb = getNumberFormatCombo();
  return (NumberFormat)cb->GetCurSel();
}

void CTestExpressionGraphicsDlg::setNumberFormat(NumberFormat nf) {
  CComboBox *cb = getNumberFormatCombo();
  cb->SetCurSel(nf);
}

int CTestExpressionGraphicsDlg::getFontSize() {
  CString str;
  CComboBox *cb = getFontSizeCombo();
  cb->GetLBText(cb->GetCurSel(), str);
  const TCHAR *cp = (LPCTSTR)str;
  int result;
  if(_stscanf(cp, _T("%d"), &result) != 1) {
    return -1;
  }
  return result;
}

void CTestExpressionGraphicsDlg::setFontSize(int size) {
  CString    sizeStr = format(_T("%d"),size).cstr();
  CComboBox *cb      = getFontSizeCombo();
  const int  n       = cb->GetCount();
  for(int i = 0; i < n; i++) {
    CString str;
    cb->GetLBText(i, str);
    if(str == sizeStr) {
      cb->SetCurSel(i);
      return;
    }
  }
}

void CTestExpressionGraphicsDlg::OnOK() {
}

void CTestExpressionGraphicsDlg::OnCancel() {
}

void CTestExpressionGraphicsDlg::OnClose() {
  OnFileExit();
}

void CTestExpressionGraphicsDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CTestExpressionGraphicsDlg::OnSamplesSampleId(UINT cmd) {
  const int id = cmd - FIRST_SAMPLE_COMMAND;
  const TCHAR *sample = ExpressionSamples::getSample(id);
  setWindowText(this, IDC_COMBOEXPR, sample);
}

class AllSamplesTester : public InteractiveRunnable {
private:
  const String       m_dir;
  PixRectDevice     &m_device;
  const int          m_fontSize;
  const NumberFormat m_numberFormat;
  UINT               m_maxCount, m_count;

  NumberFormat getNumberFormat() const {
    return m_numberFormat;
  }
  int getFontSize() const {
    return m_fontSize;
  }
public:
  AllSamplesTester(const String &dir, PixRectDevice &device, int fontSize, NumberFormat numberFormat)
                 : m_dir(         dir         )
                 , m_device(      device      )
                 , m_fontSize(    fontSize    )
                 , m_numberFormat(numberFormat)
  {
    m_maxCount = ExpressionSamples::getCount();
    m_count    = 0;
  }
  double getMaxProgress() const {
    return m_maxCount;
  }
  double getProgress() const {
    return m_count;
  };
  String getTitle() {
    return _T("Testing all samples");
  }
  int getSupportedFeatures() {                 // Should return any combination of IR_-constants
    return IR_PROGRESSBAR
         | IR_INTERRUPTABLE
         | IR_SUSPENDABLE
         | IR_SHOWTIMEESTIMATE
         | IR_AUTOCORRELATETIME
         | IR_SHOWPERCENT;
  }
  UINT run();
};

UINT AllSamplesTester::run() {
  for(m_count = 0; m_count < m_maxCount; m_count++) {
    const String str = ExpressionSamples::getSample(m_count);
    try {
      Expression expr;
      expr.compile(str,false);
      Expr::ExpressionImage image = expressionToImage(m_device, expr, getFontSize(), getNumberFormat());
      const String fileName = format(_T("testCase%03d.jpg"), m_count);
      const String fullName = FileNameSplitter::getChildName(m_dir,fileName);
      image.getImage()->writeAsJPG(ByteOutputFile(fullName));
    } catch(Exception e) {
      const String msg = format(_T("Testcase %d\n%s"), m_count, e.what());
      showMessageBox(0, _T("%s"), msg.cstr());
    } catch (...) {
      showWarning(_T("Unknown exception in %s"), __TFUNCTION__);
    }
  }
  return 0;
}

void CTestExpressionGraphicsDlg::OnSamplesRunall() {
  const String     dir = _T("C:\\temp\\ExprList\\Images");
  AllSamplesTester testJob(dir, m_device, getFontSize(), getNumberFormat());
  ProgressWindow(this, testJob, 1);
  startViewPhoto(dir, _T("testCase000.jpg"));
}

void CTestExpressionGraphicsDlg::startViewPhoto(const String &dir, const String &fileName) { // static
  const String fullName = FileNameSplitter::getChildName(dir,fileName);
  const String cmd = format(_T("shimgvw.dll,ImageView_Fullscreen %s"), fullName.cstr());
  HINSTANCE inst = ::ShellExecute(NULL,L"open",_T("rundll32.exe"),cmd.cstr(),dir.cstr(),SW_SHOWNORMAL);
}

void CTestExpressionGraphicsDlg::startThread(int debugWinId, bool singleStep) {
  if(!UpdateData()) {
    return;
  }
  try {
    destroyThread();
    Expression *ee;
    switch(debugWinId) {
    case IDC_STATICEXPRIMAGE   :
      ee = &m_expr;
      break;
    case IDC_STATICDERIVEDIMAGE:
      ee = &m_derivedExpr;
      break;
    default:
      showWarning(_T("startThread:Invalid debugWinId:%d"), debugWinId);
      return;
    }
    m_debugWinId = debugWinId;
    createThread(*ee);
    INVALIDATE();
    if(singleStep) {
      m_debugThread->singleSubStep();
    } else {
      m_debugThread->go();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CTestExpressionGraphicsDlg::createThread(Expression &expr) {
  m_debugThread = new DebugThread(expr); TRACE_NEW(m_debugThread);
  m_debugThread->addPropertyChangeListener(this);
  expr.addPropertyChangeListener(m_debugThread);
#ifdef TRACE_REDUCTION_CALLSTACK
  expr.getReductionStack().addPropertyChangeListener(m_debugThread);
#endif
}

void CTestExpressionGraphicsDlg::destroyThread() {
  m_debugError = EMPTYSTRING;
  if(hasDebugThread()) {
    m_debugThread->kill();
    switch(m_debugWinId) {
    case IDC_STATICEXPRIMAGE   :
      m_expr.removePropertyChangeListener(m_debugThread);
#ifdef TRACE_REDUCTION_CALLSTACK
      m_expr.getReductionStack().removePropertyChangeListener(m_debugThread);
#endif
      break;
    case IDC_STATICDERIVEDIMAGE:
      m_derivedExpr.removePropertyChangeListener(m_debugThread);
#ifdef TRACE_REDUCTION_CALLSTACK
      m_derivedExpr.getReductionStack().removePropertyChangeListener(m_debugThread);
#endif
      break;
    }
    SAFEDELETE(m_debugThread);
    m_debugWinId  = -1;
    clearDebugInfo();
  }
}

void CTestExpressionGraphicsDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(source == m_debugThread) {
    switch(id) {
    case THREAD_RUNNING   :
      PostMessage(ID_MSG_RUNSTATE_CHANGED, (WPARAM)(*(bool*)oldValue), (LPARAM)(*(bool*)newValue));
      break;
    case THREAD_TERMINATED:
      if(m_derivedExpr.getState() == PS_REDUCTIONDONE) {
      }
      INVALIDATE();
      break;
    case THREAD_ERROR     :
      { const TCHAR *msg = (const TCHAR*)newValue;
        m_debugError = msg;
        PostMessage(ID_MSG_SHOW_DEBUGERROR,0,0);
      }
      break;
    default:
      { m_debugError = format(_T("Received unknown property change. Id=%d"), id);
        PostMessage(ID_MSG_SHOW_DEBUGERROR,0,0);
      }
      break;
    }
  } else if(source == m_currentChildDlg) {
    switch(id) {
    case TREE_SELECTEDNODE:
      { ExpressionNode *oldSelected = (ExpressionNode*)oldValue;
        ExpressionNode *newSelected = (ExpressionNode*)newValue;
        if(oldSelected != NULL) {
          oldSelected->unMark();
        }
        if(newSelected != NULL) {
          newSelected->mark();
        }
        updateContextWinImage();
        INVALIDATE();
      }
    }
  }
}

LRESULT CTestExpressionGraphicsDlg::OnMsgRunStateChanged(WPARAM wp, LPARAM lp) {
  const bool oldRunning = wp ? true : false;
  const bool newRunning = lp ? true : false;
  if(!newRunning) {
    paintDebugExpr();
  }
  return 0;
}

LRESULT CTestExpressionGraphicsDlg::OnMsgShowDebugError(WPARAM wp, LPARAM lp) {
  showWarning(m_debugError);
  return 0;
}

void CTestExpressionGraphicsDlg::enableFieldList(const int *ids, int n, bool enabled) {
  while(n--) {
    GetDlgItem(*(ids++))->EnableWindow(enabled ? TRUE : FALSE);
  }
}

void CTestExpressionGraphicsDlg::ajourDialogItems() {
  static const int dialogFields[] = {
    IDC_COMBOEXPR
   ,IDC_EDITX
   ,IDC_COMBOFONTSIZE
   ,IDC_COMBONUMBERFORMAT
  };

  static const int comboFields[] = {
    IDC_COMBOFONTSIZE
   ,IDC_COMBONUMBERFORMAT
  };

#define ENABLEFIELDLIST(a,enabled) enableFieldList(a, ARRAYSIZE(a), enabled)

  if(hasDebugThread()) {
    if(m_debugThread->isRunning()) {
      ENABLEFIELDLIST(dialogFields, false);
      enableMenuItem(this, ID_DEBUG_REDUCEEXPR          , false);
      enableMenuItem(this, ID_DEBUG_REDUCEDERIVED       , false);
      enableMenuItem(this, ID_DEBUG_TRACEREDUCTIONSTEP  , false);
      enableMenuItem(this, ID_DEBUG_STEP1REDUCEITERATION, false);
      enableMenuItem(this, ID_DEBUG_RUN                 , false);
      enableMenuItem(this, ID_DEBUG_STOP                , true );
    } else if(m_debugThread->isTerminated()) {
      ENABLEFIELDLIST(dialogFields, true );
      enableMenuItem(this, ID_DEBUG_REDUCEEXPR          , true );
      enableMenuItem(this, ID_DEBUG_REDUCEDERIVED       , true );
      enableMenuItem(this, ID_DEBUG_TRACEREDUCTIONSTEP  , false);
      enableMenuItem(this, ID_DEBUG_STEP1REDUCEITERATION, false);
      enableMenuItem(this, ID_DEBUG_RUN                 , false);
      enableMenuItem(this, ID_DEBUG_STOP                , false);
    } else { // paused
      ENABLEFIELDLIST(dialogFields  , false);
      ENABLEFIELDLIST(comboFields   , true );
      enableMenuItem(this, ID_DEBUG_REDUCEEXPR          , false);
      enableMenuItem(this, ID_DEBUG_REDUCEDERIVED       , false);
      enableMenuItem(this, ID_DEBUG_TRACEREDUCTIONSTEP  , true );
      enableMenuItem(this, ID_DEBUG_STEP1REDUCEITERATION, true );
      enableMenuItem(this, ID_DEBUG_RUN                 , true );
      enableMenuItem(this, ID_DEBUG_STOP                , true );
    }
  } else { // No debug thread
    ENABLEFIELDLIST(dialogFields, true);
    enableMenuItem(this, ID_DEBUG_REDUCEEXPR            , true );
    enableMenuItem(this, ID_DEBUG_REDUCEDERIVED         , true );
    enableMenuItem(this, ID_DEBUG_TRACEREDUCTIONSTEP    , false);
    enableMenuItem(this, ID_DEBUG_STEP1REDUCEITERATION  , false);
    enableMenuItem(this, ID_DEBUG_RUN                   , false);
    enableMenuItem(this, ID_DEBUG_STOP                  , false);
  }
}

void CTestExpressionGraphicsDlg::paintDebugExpr() {
  try {
    m_debugExpr = &m_debugThread->getDebugExpr();
    showDebugInfo(_T("State:%-14s. it:%d complexity(%s), rat.const:%3zu")
                 ,m_debugExpr->getStateName().cstr()
                 ,m_debugExpr->getReduceIteration()
                 ,m_debugExpr->getComplexity().toString().cstr()
                 ,m_debugExpr->getRationalConstantMap().size()
                 );

#ifdef TRACE_REDUCTION_CALLSTACK
    if(isMenuItemChecked(this, ID_VIEW_SHOWREDUCTIONSTACK)) {
      const ReductionStack &stack = m_debugExpr->getReductionStack();
      const String s = stack.toString();
      m_reductionStackWindow.SetWindowText(s.cstr());
    }
#endif
    switch(m_debugWinId) {
    case IDC_STATICEXPRIMAGE   :
      { const BitSet16 oldFlags = m_flags;
        makeExprImage(*m_debugExpr);
        if(oldFlags.contains(HASFVALUE)) {
//          String varStr = m_debugExpr->getVariables().toString();
          OnFunctionsEvaluateFx();
        }
      }
      break;
    case IDC_STATICDERIVEDIMAGE:
      makeDerivedImage(*m_debugExpr);
      break;
    }

    setWindowText(this, IDC_EDITDERIVED      , m_debugExpr->toString());
    INVALIDATE();
  } catch(Exception e) {
    showException(e);
  }
}

void CTestExpressionGraphicsDlg::OnFunctionsCompileFx() {
  try {
    const BitSet16 oldFlags = m_flags;
    if(oldFlags.contains(HASFVALUE)) {
      saveExprVariables();
    }

    destroyThread();
    m_flags.remove(ISCOMPILED);
    clearExprImage();
    clearDerivedImage();
    makeExprImage();
    if(oldFlags.contains(HASFVALUE) && restoreExprVariables()) {
      OnFunctionsEvaluateFx();
    }
  } catch(Exception e) {
    showException(e);
  }
  INVALIDATE();
}

void CTestExpressionGraphicsDlg::OnFunctionsDeriveFx() {
  clearDerivedExpr();
  makeDerivedImage();
  INVALIDATE();
}

void CTestExpressionGraphicsDlg::OnFunctionsReduceFx() {
  try {
    const BitSet16 oldFlags = m_flags;
    if(hasExprImage() /*&& !m_flags.contains(ISEXPRREDUCED) */) {
      clearExprImage();
      m_expr.reduce();
      m_flags.add(ISEXPRREDUCED);
      makeExprImage();
      if(oldFlags.contains(HASFVALUE)) {
        OnFunctionsEvaluateFx();
      }
      INVALIDATE();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CTestExpressionGraphicsDlg::OnFunctionsReduceDerived() {
  try {
    const BitSet16 oldFlags = m_flags;
    if(hasDerivedImage() /*&& !m_flags.contains(ISDERIVEDREDUCED) */) {
      clearDerivedImage();
      m_derivedExpr.reduce();
      m_flags.add(ISDERIVEDREDUCED);
      makeDerivedImage();
      if(oldFlags.contains(HASDERIVEDVALUE2)) {
        OnFunctionsEvaluateDerived();
      }
      INVALIDATE();
    }
  } catch(Exception e) {
    showException(e);
  }
}

double CTestExpressionGraphicsDlg::getX(bool &ok) {
  if(!UpdateData()) {
    ok = false;
    return 0;
  }
  ok = true;
  return m_x;
}

#define EPS 1e-7

void CTestExpressionGraphicsDlg::OnFunctionsEvaluateFx() {
  clearResultValue();
  if(!m_flags.contains(ISCOMPILED)) {
    return;
  }

  const ExpressionVariable *var = m_expr.getVariable(_T("x"));
  Real dummyX, &varX = var ? m_expr.getValueRef(*var) : dummyX;

  bool ok;
  const Real x0 = getX(ok);
  if(!ok) return;
  try {
    varX = x0;
    const Real y0 = m_expr.evaluate();
    setWindowText(this, IDC_EDITRESULTVALUE, toString(y0));
    m_flags.add(HASFVALUE);

    const Real x1 = (x0==0) ? EPS : x0 * (1.0+EPS);
    varX = x1;
    const Real y1   = m_expr.evaluate();
    setWindowText(this, IDC_EDITDERIVEDVALUE1, toString((y1-y0)/(x1-x0)));
    m_flags.add(HASDERIVEDVALUE1);
  } catch(Exception e) {
    showException(e);
  }
  varX = x0;
}

void CTestExpressionGraphicsDlg::OnFunctionsEvaluateDerived() {
  clearDerivedValue2();
  if(!m_flags.contains(ISDERIVED)) return;

  OnFunctionsEvaluateFx();
  if(!m_flags.contains(HASFVALUE)) return;

  ExpressionVariableArray variables = m_derivedExpr.getSymbolTable().getAllVariables();
  for(size_t i = 0; i < variables.size(); i++) {
    ExpressionVariable &v = variables[i];
    if(v.isInput()) {
      const ExpressionVariable *fv = m_expr.getVariable(v.getName());
      if(fv) {
        m_derivedExpr.setValue(v.getName(), m_expr.getValueRef(*fv));
      }
    }
  }

  try {
    setWindowText(this, IDC_EDITDERIVEDVALUE2, toString(m_derivedExpr.evaluate()));
    m_flags.add(HASDERIVEDVALUE2);
  } catch(Exception e) {
    showException(e);
  }
}

void CTestExpressionGraphicsDlg::OnFunctionsEvaluateAll() {
  makeExprImage();
  OnFunctionsEvaluateFx();
  makeDerivedImage();
  OnFunctionsEvaluateDerived();
  INVALIDATE();
}


void CTestExpressionGraphicsDlg::OnCbnSelChangeComboExpr() {
  OnCbnEditChangeComboExpr();
}

void CTestExpressionGraphicsDlg::OnCbnEditChangeComboExpr() {
  m_flags.add(ISDIRTY);
}

void CTestExpressionGraphicsDlg::OnChangeEditX() {
  clearResultValue();
  clearDerivedValue2();
}

void CTestExpressionGraphicsDlg::OnSelchangeComboNumberFormat() {
  onSelChangeCombo();
}

void CTestExpressionGraphicsDlg::OnSelChangeComboFontSize() {
  onSelChangeCombo();
}

void CTestExpressionGraphicsDlg::onSelChangeCombo() {
  saveOptions();
  if(isThreadPaused()) {
    paintDebugExpr();
  } else {
    const bool hasImage1 = hasExprImage();
    const bool hasImage2 = hasDerivedImage();

    if(hasImage1) makeExprImage(m_expr);
    if(hasImage2) makeDerivedImage(m_derivedExpr);
    INVALIDATE();
  }
}

static const TCHAR *registryEntry = _T("Software\\JGMData\\TestExpressionGraphics");

class InitRegistry {
public:
  InitRegistry();
};

InitRegistry::InitRegistry() {
  try {
    RegistryKey(HKEY_CURRENT_USER).createOrOpenPath(registryEntry);
  } catch(Exception) {
    // ignore
  }
}

static InitRegistry dummy;

static RegistryKey getKey() {
  return RegistryKey(HKEY_CURRENT_USER, registryEntry).createOrOpenKey(_T("Settings"));
}

void CTestExpressionGraphicsDlg::saveOptions() {
  RegistryKey root = getKey();
  root.setValue(_T("fontsize"    ), getFontSize());
  root.setValue(_T("numberformat"), numberFormatToString(getNumberFormat()));
}

void CTestExpressionGraphicsDlg::loadOptions() {
  RegistryKey        root     = getKey();
  const int          fontSize =                   root.getInt(   _T("fontsize"    ), 24         );
  const NumberFormat nf       = ::getNumberFormat(root.getString(_T("numberformat"), EMPTYSTRING));
  setFontSize(fontSize);
  setNumberFormat(nf);
}

void CTestExpressionGraphicsDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  const int mouseWinId = getWindowIdFromPoint(point);;
  switch(mouseWinId) {
  case IDC_STATICEXPRIMAGE     :
  case IDC_STATICDERIVEDIMAGE  :
    if(hasContextImage() && (getContextWindowId() == mouseWinId)) {
      const ExpressionRectangle *leastRect   = getContextImage()->findLeastRectangle(point);
      const ExpressionRectangle *contextRect = getContextRect();
      if(leastRect) {
        if(leastRect->hasNode() && (leastRect != m_leastContextRect)) {
          setContextRect(mouseWinId, leastRect, leastRect);
        } else if(contextRect && contextRect->hasAncestorWithNode()) {
          setContextRect(mouseWinId, contextRect->getFirstAncestorWithNode(), leastRect);
        }
      } else {
        setContextWindow(mouseWinId);
      }
    } else if(hasImageInWindow(mouseWinId)) { // clicked on a window with another id
      ExpressionImage &image = getImageFromWinId(mouseWinId);
      const ExpressionRectangle *leastRect = image.findLeastRectangle(point);
      setContextRect(mouseWinId, leastRect, leastRect);
    } else {
      clearContextWindow();
    }
    break;
  default:
    clearContextWindow();
    break;
  }
  __super::OnLButtonDown(nFlags, point);
}

void CTestExpressionGraphicsDlg::OnContextMenu(CWnd *pWnd, CPoint point) {
  CPoint mouseDown = point;
  ScreenToClient(&mouseDown);
  const int mouseWinId = getWindowIdFromPoint(mouseDown);;

  CMenu menu;
  switch(mouseWinId) {
  case IDC_STATICEXPRIMAGE     :
  case IDC_STATICDERIVEDIMAGE  :
    { const ExpressionRectangle *leastRect = getImageFromWinId(mouseWinId).findLeastRectangle(mouseDown);
      setContextRect(mouseWinId, leastRect, leastRect);
      if(!loadMenu(menu, IDR_CONTEXTMENU)) return;
      if(hasDebugThread()) {
        removeMenuItem(menu, ID_CONTEXTMENU_TOSTANDARDFORM);
        removeMenuItem(menu, ID_CONTEXTMENU_TOCANONCALFORM);
        removeMenuItem(menu, ID_CONTEXTMENU_TONUMERICFORM );
        removeMenuItem(menu, ID_CONTEXTMENU_EXPAND        );
        removeMenuItem(menu, ID_CONTEXTMENU_MULTIPLY      );
      } else {
        switch(getContextExpression()->getTreeForm()) {
        case TREEFORM_STANDARD  : removeMenuItem(menu, ID_CONTEXTMENU_TOSTANDARDFORM); break;
        case TREEFORM_CANONICAL : removeMenuItem(menu, ID_CONTEXTMENU_TOCANONCALFORM); break;
        case TREEFORM_NUMERIC   : removeMenuItem(menu, ID_CONTEXTMENU_TONUMERICFORM ); break;
        }
        if(!isContextNodeExpandable()) {
          removeMenuItem(menu, ID_CONTEXTMENU_EXPAND        );
        }
        if(!isContextNodeMultiplyable()) {
          removeMenuItem(menu, ID_CONTEXTMENU_MULTIPLY      );
        }
      }
      break;
    }
    break;
  case IDC_STATICREDUCTIONSTACK:
    if(!loadMenu(menu, IDR_CONTEXTMENUSTACK)) return;
    break;
  default:
    return;
  }
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
}

int CTestExpressionGraphicsDlg::getWindowIdFromPoint(CPoint &p) { // assume p relative to *this
  const CRect exprRect    = getWindowRect(this, IDC_STATICEXPRIMAGE);
  const CRect derivedRect = getWindowRect(this, IDC_STATICDERIVEDIMAGE);
  const CRect stackRect   = m_reductionStackWindow.IsWindowVisible() ? getWindowRect(m_reductionStackWindow) : CRect(0,0,0,0);
  int mouseWinId = -1;

  if(exprRect.PtInRect(p)) {
    mouseWinId = IDC_STATICEXPRIMAGE;
  } else if(derivedRect.PtInRect(p)) {
    mouseWinId = IDC_STATICDERIVEDIMAGE;
  } else if(stackRect.PtInRect(p)) {
#ifdef TRACE_REDUCTION_CALLSTACK
    mouseWinId = IDC_STATICREDUCTIONSTACK;
    CPoint stackPoint = p;
    ClientToScreen(&stackPoint);
    m_reductionStackWindow.ScreenToClient(&stackPoint);
    m_selectedStackElement = getSelectedStackElement(stackPoint);
    if((m_selectedStackElement == NULL) || (m_selectedStackElement->m_node == NULL)) {
      return -1;
    }
    p = stackPoint;
    return mouseWinId;
#else
    return -1;
#endif
  } else {
    return -1;
  }

  if(!hasImageInWindow(mouseWinId)) {
    return -1;
  }
  ClientToScreen(&p);
  CWnd *win = GetDlgItem(mouseWinId);
  win->ScreenToClient(&p);
  return mouseWinId;
}

bool CTestExpressionGraphicsDlg::loadMenu(CMenu &menu, int id) {
  if(!menu.LoadMenu(id)) {
    showWarning(_T("Loadmenu failed"));
    return false;
  }
  return true;
}

#ifdef TRACE_REDUCTION_CALLSTACK
const ReductionStackElement *CTestExpressionGraphicsDlg::getSelectedStackElement(CPoint p) {
  if(!isThreadPaused()) return NULL;

  const int             lineHeight  = m_reductionStackWindow.getLineHeight();
  const CSize           clSize      = getClientRect(m_reductionStackWindow).Size();
  const ReductionStack &stack       = m_debugExpr->getReductionStack();
  const int             stackHeight = stack.getHeight();
  const int             index       = stackHeight - 1 - (clSize.cy - p.y) / lineHeight ; // from top

  if(index < 0 || index >= stackHeight) {
    return NULL;
  }
  // index = [0..stack.getHeightHeight()-1]. topelement is top(0), bottom element i top(stackHeight()-1)
  return &stack.top(index);
}
#endif

void CTestExpressionGraphicsDlg::OnContextMenuShowExprTree() {
  Expression     *expr;
  ExpressionNode *node = NULL;
  bool  handleNodeChanges = false;
  if(isThreadPaused() && (getContextWindowId() == m_debugWinId)) {
    expr = m_debugExpr;
  } else {
    expr = getContextExpression();
    node = getContextNode();
    handleNodeChanges = true;
  }

  if(expr == NULL) {
    showWarning(_T("No expression to show"));
    return;
  }
  if(node) {
    CExpressionTreeDlg dlg(node);
    dlg.DoModal();
  } else {
    CExpressionTreeDlg dlg(*expr);
    m_currentChildDlg = &dlg;
    if(handleNodeChanges) {
      m_currentChildDlg->addPropertyChangeListener(this);
    }
    dlg.DoModal();
    expr->unmarkAll();
    if(handleNodeChanges) {
      updateContextWinImage();
      INVALIDATE();
    }
  }
}

void CTestExpressionGraphicsDlg::OnContextMenuToStandardForm() {
  Expression *expr = getContextExpression();
  if(expr == NULL) {
    showWarning(_T("No expression to convert"));
    return;
  }
  expr->setTreeForm(TREEFORM_STANDARD);
  updateContextWinImage();
  INVALIDATE();
}

void CTestExpressionGraphicsDlg::OnContextMenuToCanoncalForm() {
  Expression *expr = getContextExpression();
  if(expr == NULL) {
    showWarning(_T("No expression to convert"));
    return;
  }
  expr->setTreeForm(TREEFORM_CANONICAL);
  updateContextWinImage();
  INVALIDATE();
}

void CTestExpressionGraphicsDlg::OnContextMenuToNumericForm() {
  Expression *expr = getContextExpression();
  if(expr == NULL) {
    showWarning(_T("No expression to convert"));
    return;
  }
  expr->setTreeForm(TREEFORM_NUMERIC);
  updateContextWinImage();
  INVALIDATE();
}

void CTestExpressionGraphicsDlg::OnContextMenuShowNodeTree() {
#ifdef TRACE_REDUCTION_CALLSTACK
  CExpressionTreeDlg dlg(m_selectedStackElement->m_node);
  dlg.DoModal();
#endif
}

void CTestExpressionGraphicsDlg::OnContextMenuExpand() {
  if(!hasContextNode()) return;
  Expression *expr = getContextExpression();
  if(expr == NULL) {
    showWarning(_T("No expression to expand"));
    return;
  }
  expr->unmarkAll();
  getContextNode()->mark();
  expr->expandMarkedNodes();
  updateContextWinImage();
  INVALIDATE();
}

void CTestExpressionGraphicsDlg::OnContextMenuMultiply() {
  if(!hasContextNode()) return;
  Expression *expr = getContextExpression();
  if(expr == NULL) {
    showWarning(_T("No expression to multiply"));
    return;
  }
  expr->unmarkAll();
  getContextNode()->mark();
  expr->multiplyMarkedNodes();
  updateContextWinImage();
  INVALIDATE();
}

void CTestExpressionGraphicsDlg::OnDebugReduceExpr() {
  const bool hasFValue = m_flags.contains(HASFVALUE);
  makeExprImage();
  clearDerivedExpr();
  if(hasExprImage()) {
    if(hasFValue) m_flags.add(HASFVALUE);
    startThread(IDC_STATICEXPRIMAGE, true);
    INVALIDATE();
  }
}

void CTestExpressionGraphicsDlg::OnDebugReduceDerived() {
  makeDerivedImage();
  if(hasDerivedImage()) {
    startThread(IDC_STATICDERIVEDIMAGE, true);
    INVALIDATE();
  }
}

void CTestExpressionGraphicsDlg::OnDebugRun() {
  if(isThreadPaused()) {
    m_debugThread->go();
  }
}

void CTestExpressionGraphicsDlg::OnDebugTraceReductionStep() {
  if(isThreadPaused()) {
#ifdef TRACE_REDUCTION_CALLSTACK
    if(isMenuItemChecked(this, ID_VIEW_SHOWREDUCTIONSTACK)) {
      m_debugThread->goUntilReturn();
    } else
#endif
    m_debugThread->singleStep();
  }
}

void CTestExpressionGraphicsDlg::OnDebugStep1ReduceIteration() {
  if(isThreadPaused()) m_debugThread->singleSubStep();
}

void CTestExpressionGraphicsDlg::OnDebugStop() {
  try {
    if(hasDebugThread()) {
      m_debugThread->kill();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CTestExpressionGraphicsDlg::OnDebugClearAllBreakPoints() {
#ifdef TRACE_REDUCTION_CALLSTACK
  if(isThreadPaused()) {
    m_debugExpr->clearAllBreakPoints();
  }
#endif
}

void CTestExpressionGraphicsDlg::OnDebugTestTreesEqual() {
  CTestTreesEqualDlg dlg;
  dlg.DoModal();
}

void CTestExpressionGraphicsDlg::compileExpr() {
  if(m_flags.contains(ISDIRTY) || !m_flags.contains(ISCOMPILED)) {
    UpdateData();
    const String exprStr = (LPCTSTR)m_exprText;
    m_expr.compile(exprStr, false);
    clearExprImage();
    m_flags.clear();

    if(m_expr.isOk()) {
      m_flags.add(ISCOMPILED);
      m_exprCombo.updateList();
    } else {
      const StringArray &errors = m_expr.getErrors();
      String error = errors[0];
      int pos = m_expr.decodeErrorString(exprStr, error);
      OnEditGotoComboFx();
      m_exprCombo.SetEditSel(pos, pos);
      showWarning(error);
    }
  }
}

void CTestExpressionGraphicsDlg::deriveExpr() {
  compileExpr();
  if(m_flags.contains(ISCOMPILED) && !m_flags.contains(ISDERIVED)) {
    try {
      m_derivedExpr = m_expr.getDerived(_T("x"), false);
      m_flags.add(ISDERIVED);
      setWindowText(this, IDC_EDITDERIVED, m_derivedExpr.toString());
    } catch(Exception e) {
      showException(e);
    }
  }
}

void CTestExpressionGraphicsDlg::makeExprImage() {
  try {
    compileExpr();
    if(m_flags.contains(ISCOMPILED) && !hasExprImage()) {
      makeExprImage(m_expr);
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CTestExpressionGraphicsDlg::makeDerivedImage() {
  makeExprImage();
  if(hasExprImage()) {
    deriveExpr();
    if(m_flags.contains(ISDERIVED) && !hasDerivedImage()) {
      makeDerivedImage(m_derivedExpr);
    }
  }
}

void CTestExpressionGraphicsDlg::makeExprImage(const Expression &expr) {
  try {
    const int oldContextWindow = getContextWindowId();
    clearExprImage();
    m_exprImage = expressionToImage(m_device, expr, getFontSize(), getNumberFormat());
    if(oldContextWindow == IDC_STATICEXPRIMAGE) {
      setContextWindow(oldContextWindow);
    }
  } catch(Exception e) {
    showException(e);
  } catch (...) {
    showWarning(_T("Unknown exception in %s"), __TFUNCTION__);
  }
}

void CTestExpressionGraphicsDlg::makeDerivedImage(const Expression &expr) {
  try {
    const int oldContextWindow = getContextWindowId();
    clearDerivedImage();
    m_derivedImage = expressionToImage(m_device, expr, getFontSize(), getNumberFormat());
    if(oldContextWindow == IDC_STATICDERIVEDIMAGE) {
      setContextWindow(oldContextWindow);
    }
  } catch(Exception e) {
    showException(e);
  }
}

class DrawRectangle : public ExpressionRectangleHandler {
private:
  CDC &m_dc;
public:
  DrawRectangle(CDC &dc) : m_dc(dc) {
  }
  bool handleRectangle(const ExpressionRectangle &r, const ExpressionRectangle *parent);
};

bool DrawRectangle::handleRectangle(const ExpressionRectangle &r, const ExpressionRectangle *parent) {
  m_dc.MoveTo(r.TopLeft());
  m_dc.LineTo(r.TopRight());
  m_dc.LineTo(r.BottomRight());
  m_dc.LineTo(r.BottomLeft());
  m_dc.LineTo(r.TopLeft());
  return true;
}

void CTestExpressionGraphicsDlg::paintExprImage() {
  paintImage(IDC_STATICEXPRIMAGE, m_exprImage);
}

void CTestExpressionGraphicsDlg::paintDerivedImage() {
  paintImage(IDC_STATICDERIVEDIMAGE, m_derivedImage);
}

void CTestExpressionGraphicsDlg::paintImage(int id, const ExpressionImage &image) {
  CWnd       *window = GetDlgItem(id);
  const CRect cr     = getClientRect(window);
  CPaintDC    dc(window);
  dc.FillSolidRect(0,0,cr.Width(), cr.Height(), GetSysColor(COLOR_BTNFACE));
  CRgn rgn;
  rgn.CreateRectRgn(0,0,cr.right,cr.bottom);
  dc.SelectClipRgn(&rgn);
  if(!image.isEmpty()) {
    const PixRect *pr = image.getImage();
    const CSize    sz = pr->getSize();
    PixRect::bitBlt(dc,0,0,sz.cx,sz.cy,SRCCOPY,pr,0,0);
    if(isMenuItemChecked(this, ID_VIEW_SHOWRECTANGLES)) {
      image.traverseRectangleTree(DrawRectangle(dc));
    }
  }
}

void CTestExpressionGraphicsDlg::clearExprImage() {
  m_exprImage.clear();;
  clearContextWindow();
  clearResultValue();
  clearDerivedImage();
}

void CTestExpressionGraphicsDlg::clearDerivedExpr() {
  m_flags.remove(ISDERIVED);
  m_flags.remove(ISDERIVEDREDUCED);
  clearDerivedImage();
  clearDerivedValue2();
}

void CTestExpressionGraphicsDlg::clearDerivedImage() {
  m_derivedImage.clear();
  if(getContextWindowId() == IDC_STATICDERIVEDIMAGE) {
    clearContextWindow();
  }
}

void CTestExpressionGraphicsDlg::clearResultValue() {
  m_flags.remove(HASFVALUE);
  setWindowText(this, IDC_EDITRESULTVALUE, EMPTYSTRING);
  clearDerivedValue1();
}

void CTestExpressionGraphicsDlg::clearDerivedValue1() {
  m_flags.remove(HASDERIVEDVALUE1);
  setWindowText(this, IDC_EDITDERIVEDVALUE1, EMPTYSTRING);
}

void CTestExpressionGraphicsDlg::clearDerivedValue2() {
  m_flags.remove(HASDERIVEDVALUE2);
  setWindowText(this, IDC_EDITDERIVEDVALUE2, EMPTYSTRING);
}

void CTestExpressionGraphicsDlg::showDebugInfo(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  setWindowText(this, IDC_STATICDEBUGINFO, msg);
}

void CTestExpressionGraphicsDlg::clearDebugInfo() {
  showDebugInfo(EMPTYSTRING);
}

static int variableNameCompare(const ExpressionVariableWithValue &v1, const ExpressionVariableWithValue &v2) {
  return _tcscmp(v1.getName().cstr(), v2.getName().cstr());
}

static bool isSameVariableNames(const ExpressionVariableArray &a1, const ExpressionVariableArray &a2) {
  if(a1.size() != a2.size()) {
    return false;
  }
  for(size_t i = 0; i < a1.size(); i++) {
    if(a1[i].getName() != a2[i].getName()) {
      return false;
    }
  }
  return true;
}

void CTestExpressionGraphicsDlg::saveExprVariables() {
  if(m_flags.contains(HASFVALUE)) {
    m_savedVariables = m_expr.getSymbolTable().getAllVariables();
    m_savedVariables.sort(variableNameCompare);
    m_flags.add(HASSAVEDVARIABLES);
  } else {
    clearSavedVariables();
  }
}

void CTestExpressionGraphicsDlg::clearSavedVariables() {
  m_savedVariables.clear();
  m_flags.remove(HASSAVEDVARIABLES);
}

bool CTestExpressionGraphicsDlg::restoreExprVariables() {
  if(!m_flags.contains(ISCOMPILED)) {
    return false;
  }
  ExpressionVariableArray a = m_expr.getSymbolTable().getAllVariables();
  a.sort(variableNameCompare);
  if(!isSameVariableNames(a, m_savedVariables)) {
    clearSavedVariables();
    return false;
  }
  for(size_t i = 0; i < m_savedVariables.size(); i++) {
    const ExpressionVariableWithValue &v = m_savedVariables[i];
    if(v.isConstant() || v.isDefined() || v.isLoopVar()) {
      continue;
    }
    m_expr.setValue(v.getName(), v.getValue());
  }
  return true;
}

void CTestExpressionGraphicsDlg::clearContextWindow() {
  setContextWindow(-1);
}

void CTestExpressionGraphicsDlg::setContextWindow(int winId) {
  setContextRect(winId,NULL,NULL);
}

void CTestExpressionGraphicsDlg::setContextRect(int winId, const ExpressionRectangle *rect, const ExpressionRectangle *leastRect) {
  const TCHAR *winName;
  switch(winId) {
  case IDC_STATICEXPRIMAGE     : winName = _T("Expr"   ); break;
  case IDC_STATICDERIVEDIMAGE  : winName = _T("Derived"); break;
  case IDC_STATICREDUCTIONSTACK: winName = _T("Stack"  ); break;
  default                      : winName = _T("None"   ); break;
  }
  if(leastRect != NULL) {
    if(rect == NULL) {
      showMessageBox(0, _T("rect == NULL && leastRect != NULL"));
      return;
    }
    if((rect != leastRect) && !leastRect->isDescentantOf(*rect)) {
      showMessageBox(0, _T("rect (=%p), leastRect (=%p) not related"), rect,leastRect);
      return;
    }
  }
  m_contextWinId     = winId;
  m_contextRect      = rect;
  m_leastContextRect = leastRect;
  setWindowText(this, IDC_STATICCONTEXT, format(_T("Context:(%s,%s)")
                                               ,winName
                                               ,getContextNodeName().cstr()
                                               )
                );
}

void CTestExpressionGraphicsDlg::updateContextWinImage() {
  switch(getContextWindowId()) {
  case IDC_STATICEXPRIMAGE   :
    { const bool hadDerivedImage = hasDerivedImage();
      makeExprImage(m_expr);
      if(!hadDerivedImage) break;
    }
    // continue case
  case IDC_STATICDERIVEDIMAGE:
    makeDerivedImage(m_derivedExpr);
    break;
  }
}

Expression *CTestExpressionGraphicsDlg::getContextExpression() {
  return getExprFromWinId(m_contextWinId);
}

ExpressionImage *CTestExpressionGraphicsDlg::getContextImage() {
  return hasImageInWindow(m_contextWinId) ? &getImageFromWinId(m_contextWinId) : NULL;
}

bool CTestExpressionGraphicsDlg::hasImageInWindow(int winId) const {
  switch(winId) {
  case IDC_STATICEXPRIMAGE   :
    return hasExprImage();
  case IDC_STATICDERIVEDIMAGE:
    return hasDerivedImage();
  }
  return false;
}

Expression *CTestExpressionGraphicsDlg::getExprFromWinId(int winId) {
  if(!hasImageInWindow(winId)) return NULL;
  switch(winId) {
  case IDC_STATICEXPRIMAGE   : return &m_expr;
  case IDC_STATICDERIVEDIMAGE: return &m_derivedExpr;
  }
  return NULL;
}

ExpressionImage &CTestExpressionGraphicsDlg::getImageFromWinId(int winId) {
  switch(winId) {
  case IDC_STATICEXPRIMAGE   : return m_exprImage;
  case IDC_STATICDERIVEDIMAGE: return m_derivedImage;
  default                    : showWarning(_T("Invalid winId:%d"), winId);
                               return m_exprImage;
  }
}
