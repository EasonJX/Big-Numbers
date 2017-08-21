#include "stdafx.h"
#include <MFCUtil/Clipboard.h>
#include "RegexDemoDlg.h"
#include "TestRegexDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

CRegexDemoDlg::CRegexDemoDlg(CWnd *pParent) : CDialog(CRegexDemoDlg::IDD, pParent) {
  m_pattern     = EMPTYSTRING;
  m_target      = EMPTYSTRING;
  m_hIcon       = theApp.LoadIcon(IDR_MAINFRAME);
  m_debugThread = NULL;
}

BOOL CRegexDemoDlg::DestroyWindow() {
  m_charMarkers.clear();
  unpaintRegex();
  killThread();
  return __super::DestroyWindow();
}

void CRegexDemoDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_CBString(pDX, IDC_COMBOPATTERN, m_pattern);
  DDX_CBString(pDX, IDC_COMBOTARGET, m_target);
}

BEGIN_MESSAGE_MAP(CRegexDemoDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_QUERYDRAGICON()
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_CLOSE()
  ON_COMMAND(ID_FILE_EXIT                  , OnFileExit                   )
  ON_COMMAND(ID_EDIT_COPY                  , OnEditCopy                   )
  ON_COMMAND(ID_EDIT_FIND                  , OnEditFind                   )
  ON_COMMAND(ID_EDIT_MATCH                 , OnEditMatch                  )
  ON_COMMAND(ID_EDIT_COMPILEPATTERN        , OnEditCompilePattern         )
  ON_COMMAND(ID_EDIT_FINDMATCHINGPAR       , OnEditFindMatchingParentesis )
  ON_COMMAND(ID_EDIT_STANDARDTEST          , OnEditStandardTest           )
  ON_COMMAND(ID_DEBUG_COMPILE              , OnDebugCompile               )
  ON_COMMAND(ID_DEBUG_FIND                 , OnDebugFind                  )
  ON_COMMAND(ID_DEBUG_MATCH                , OnDebugMatch                 )
  ON_COMMAND(ID_DEBUG_CONTINUE             , OnDebugContinue              )
  ON_COMMAND(ID_DEBUG_STEP                 , OnDebugStep                  )
  ON_COMMAND(ID_DEBUG_TOGGLEBREAKPOINT     , OnDebugToggleBreakPoint      )
  ON_COMMAND(ID_OPTIONS_IGNORECASE         , OnOptionsIgnoreCase          )
  ON_COMMAND(ID_OPTIONS_SEARCHBACKWARDS    , OnOptionsSearchBackwards     )
  ON_COMMAND(ID_OPTIONS_DFA_REGEX          , OnOptionsDFARegex            )
  ON_COMMAND(ID_OPTIONS_DFA_SHOWTABLES     , OnOptionsDFAShowTables       )
  ON_COMMAND(ID_OPTIONS_DFA_NO_GRAPHICS    , OnOptionsDFANoGraphics       )
  ON_COMMAND(ID_OPTIONS_DFA_PAINT_STATES   , OnOptionsDFAPaintStates      )
  ON_COMMAND(ID_OPTIONS_DFA_ANIMATE_CREATE , OnOptionsDFAAnimateCreate    )
  ON_COMMAND(ID_HELP_ABOUT                 , OnHelpAbout                  )
  ON_COMMAND(ID_HELP_SHOWCTRLID            , OnHelpShowctrlid             )
  ON_COMMAND(ID_GOTO_PATTERN               , OnGotoPattern                )
  ON_COMMAND(ID_GOTO_TEXT                  , OnGotoText                   )
  ON_COMMAND(ID_GOTO_BYTECODE              , OnGotoBytecode               )
  ON_CBN_EDITCHANGE(IDC_COMBOPATTERN       , OnEditChangeComboPattern     )
  ON_CBN_EDITCHANGE(IDC_COMBOTARGET        , OnEditChangeComboTarget      )
  ON_CBN_SELCHANGE( IDC_COMBOPATTERN       , OnSelChangeComboPattern      )
  ON_CBN_SELCHANGE( IDC_COMBOTARGET        , OnSelChangeComboTarget       )
  ON_LBN_SELCHANGE( IDC_LISTBYTECODE       , OnSelChangeListbyteCode      )
  ON_MESSAGE(       ID_MSG_THREADRUNNING   , OnMsgThreadRunning           )
END_MESSAGE_MAP()

void CRegexDemoDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

HCURSOR CRegexDemoDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

BOOL CRegexDemoDlg::OnInitDialog() {
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

    m_codeWindow.substituteControl(   this, IDC_LISTBYTECODE);
    m_patternCombo.substituteControl( this, IDC_COMBOPATTERN      , _T("PatternHistory"));
    m_targetCombo.substituteControl(  this, IDC_COMBOTARGET       , _T("TargetHistory") );
    m_stackWindow.substituteControl(  this, IDC_STATICSTACK);

    LOGFONT lf;
    GetFont()->GetLogFont(&lf);
    lf.lfHeight = (int)(1.5 * lf.lfHeight);
    m_comboFont.CreateFontIndirect(&lf);

    m_patternCombo.SetFont(&m_comboFont);
    m_targetCombo.SetFont(&m_comboFont);

    m_accelTable      = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

    m_charMarkers.add(new CharacterMarker(this, IDC_COMBOPATTERN,IDB_BITMAP_BLACK_DOWNARROW, true )); // PATTERN_POSMARK
    m_charMarkers.add(new CharacterMarker(this, IDC_COMBOTARGET, IDB_BITMAP_RED_UPARROW    , false)); // SEARCH_POSMARK
    m_charMarkers.add(new CharacterMarker(this, IDC_COMBOTARGET, IDB_BITMAP_PINK_UPARROW   , false)); // MATCH_STARTMARK
    m_charMarkers.add(new CharacterMarker(this, IDC_COMBOTARGET, IDB_BITMAP_GREEN_DOWNARROW, true )); // MATCH_DMARK
    m_charMarkers.add(new CharacterMarker(this, IDC_COMBOTARGET, IDB_BITMAP_GREEN_UPARROW  , false)); // LASTACCEPT_MARK
    m_charMarkers.last()->setBlinking(true);

    m_layoutManager.OnInitDialog(this);

    m_layoutManager.addControl(IDC_COMBOPATTERN            , RELATIVE_WIDTH                            );
    m_layoutManager.addControl(IDC_COMBOTARGET             , RELATIVE_WIDTH                            );
    m_layoutManager.addControl(IDC_STATICRESULT            , PCT_RELATIVE_RIGHT                        );
    m_layoutManager.addControl(IDC_STATICSTATENAMELABEL    , PCT_RELATIVE_RIGHT  | CONSTANT_WIDTH      );
    m_layoutManager.addControl(IDC_STATICSTATENAME         , PCT_RELATIVE_LEFT   | PCT_RELATIVE_RIGHT  );
    m_layoutManager.addControl(IDC_LISTBYTECODE            , PCT_RELATIVE_RIGHT                        | RELATIVE_BOTTOM                           );
    m_layoutManager.addControl(IDC_STATICFASTMAPLABEL      ,                                             RELATIVE_Y_POS                            );
    m_layoutManager.addControl(IDC_STATICFASTMAP           , PCT_RELATIVE_RIGHT  |                       RELATIVE_Y_POS                            );

    m_layoutManager.addControl(IDC_STATICREGISTERSLABEL    , PCT_RELATIVE_LEFT   | PCT_RELATIVE_RIGHT                                              );
    m_layoutManager.addControl(IDC_STATICSTACKLABEL        , PCT_RELATIVE_LEFT   | PCT_RELATIVE_RIGHT  | PCT_RELATIVE_TOP    | CONSTANT_HEIGHT     );

    m_layoutManager.addControl(IDC_STATICREGISTERS         , PCT_RELATIVE_LEFT   | RELATIVE_RIGHT                            | PCT_RELATIVE_BOTTOM );
    m_layoutManager.addControl(IDC_STATICDFAGRAPHICSWINDOW , PCT_RELATIVE_LEFT   | RELATIVE_RIGHT                            | PCT_RELATIVE_BOTTOM );
    m_layoutManager.addControl(IDC_STATICCYCLES            , PCT_RELATIVE_LEFT   | RELATIVE_RIGHT      | PCT_RELATIVE_TOP    | CONSTANT_HEIGHT     );
    m_layoutManager.addControl(IDC_STATICSTACK             , PCT_RELATIVE_LEFT   | RELATIVE_RIGHT      | PCT_RELATIVE_TOP    | RELATIVE_BOTTOM     );

    m_patternDirty = false;
    m_patternOk    = false;

    ajourDialogItems();
    GetDlgItem(IDC_COMBOPATTERN)->SetFocus();
    return FALSE;
}

void CRegexDemoDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
  if(isGraphicsOn()) {
    PostMessage(WM_PAINT);
  }
}

void CRegexDemoDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

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
    __super::OnPaint();
    paintRegex(true);
  }
}

BOOL CRegexDemoDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  BOOL ret = __super::PreTranslateMessage(pMsg);
  switch(getFocusCtrlId(this)) {
  case IDC_COMBOPATTERN:
  case IDC_COMBOTARGET :
    enableMenuItem(this, ID_EDIT_FINDMATCHINGPAR, true);
    break;
  default:
    enableMenuItem(this, ID_EDIT_FINDMATCHINGPAR, false);
    break;
  }
  return ret;
}

void CRegexDemoDlg::OnOK() {
}

void CRegexDemoDlg::OnCancel() {
}

void CRegexDemoDlg::OnClose() {
  OnFileExit();
}

void CRegexDemoDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CRegexDemoDlg::OnEditCopy() {
  String copyText;
  switch(getFocusCtrlId(this)) {
  case IDC_COMBOPATTERN : copyText = getWindowText(getPatternWindow()); break;
  case IDC_COMBOTARGET  : copyText = getWindowText(getTargetWindow());  break;
  case IDC_LISTBYTECODE : copyText = m_codeWindow.getText();            break;
  default: return;
  }
  putClipboard(*this, copyText);
}

void CRegexDemoDlg::OnEditFind() {
  unmarkAllCharacters();
  unmarkFoundPattern();
  if(checkPattern()) {
    startThread(searchForward() ? COMMAND_SEARCHFORWARD:COMMAND_SEARCHBACKWRD      );
  }
}

void CRegexDemoDlg::OnEditMatch()  {
  unmarkAllCharacters();
  unmarkFoundPattern();
  if(checkPattern()) {
    startThread(COMMAND_MATCH);
  }
}

void CRegexDemoDlg::OnEditCompilePattern() {
  killThread();
  clearCyclesWindow();
  unmarkAllCharacters();
  try {
    m_regex.compilePattern(getCompileParameters());
    setPatternCompiledOk();
  } catch(Exception e) {
    showCompilerError(e.what());
  }
}

void CRegexDemoDlg::OnEditFindMatchingParentesis() {
  const int ctrlId = getFocusCtrlId(this);
  switch(ctrlId) {
  case IDC_COMBOPATTERN:
  case IDC_COMBOTARGET :
    { CComboBox *cb = (CComboBox*)GetDlgItem(ctrlId);
      const String text = getWindowText(this, ctrlId);
      int cursorPos = cb->GetEditSel() & 0xffff;
      const int m = findMatchingpParanthes(text.cstr(), cursorPos);
      if(m >= 0) {
        cb->SetEditSel(m, m);
      }
    }
    break;
  }
}

void CRegexDemoDlg::OnEditStandardTest() {
  showInformation(_T("this function is disabled for the moment"));
//  CTestRegexDlg dlg;
//  dlg.DoModal();
}

void CRegexDemoDlg::OnDebugCompile() {
  UpdateData();
  startDebugCompile();
}

void CRegexDemoDlg::OnDebugFind()  {
  unmarkAllCharacters();
  unmarkFoundPattern();
  if(checkPattern()) {
    startThread(searchForward() ? COMMAND_SEARCHFORWARD:COMMAND_SEARCHBACKWRD, true);
  }
}

void CRegexDemoDlg::OnDebugMatch() {
  unmarkAllCharacters();
  unmarkFoundPattern();
  if(checkPattern()) {
    startThread(COMMAND_MATCH, true);
  }
}

void CRegexDemoDlg::OnDebugContinue() {
  if(isThreadStopped()) {
    m_debugThread->go();
  }
}

void CRegexDemoDlg::OnDebugStep() {
  if(isThreadStopped()) {
    m_debugThread->singleStep();
  }
}

void CRegexDemoDlg::OnDebugToggleBreakPoint() {
  CDebugTextWindow *cw = getCodeWindow();
  const int line = cw->getHighestBreakPointLine(cw->GetCurSel());

  if(cw->isBreakPointLine(line)) {
    cw->removeBreakPoint(line);
  } else {
    cw->addBreakPoint(line);
  }
}

void CRegexDemoDlg::OnOptionsIgnoreCase() {
  killThread();
  toggleMenuItem(this, ID_OPTIONS_IGNORECASE);
  if(getCompileParameters() == m_regex.getLastCompiledPattern()) {
    fillCodeWindow(getCompiledCodeText());
  } else {
    m_patternDirty = true;
    clearCodeWindow();
  }
  ajourDialogItems();
}

void CRegexDemoDlg::OnOptionsSearchBackwards() {
  killThread();
  toggleMenuItem(this, ID_OPTIONS_SEARCHBACKWARDS);
}

void CRegexDemoDlg::OnOptionsDFARegex() {
  killThread();
  unpaintRegex();
  unmarkFoundPattern();
  toggleMenuItem(this, ID_OPTIONS_DFA_REGEX);
  m_regex.setType(isMenuItemChecked(this, ID_OPTIONS_DFA_REGEX)?DFA_REGEX:EMACS_REGEX);
  if(getCompileParameters() == m_regex.getLastCompiledPattern()) {
    fillCodeWindow(getCompiledCodeText());
  } else {
    m_patternDirty = true;
    clearCodeWindow();
  }
  ajourDialogItems();
  Invalidate();
}

void CRegexDemoDlg::OnOptionsDFAShowTables() {
  toggleMenuItem(this, ID_OPTIONS_DFA_SHOWTABLES);
  fillCodeWindow(getCompiledCodeText());
  ajourDialogItems();
}

void CRegexDemoDlg::OnOptionsDFANoGraphics() {
  setDFAGraphicsMode(ID_OPTIONS_DFA_NO_GRAPHICS);
}

void CRegexDemoDlg::OnOptionsDFAPaintStates() {
  setDFAGraphicsMode(ID_OPTIONS_DFA_PAINT_STATES);
}

void CRegexDemoDlg::OnOptionsDFAAnimateCreate() {
  setDFAGraphicsMode(ID_OPTIONS_DFA_ANIMATE_CREATE);
}

void CRegexDemoDlg::OnHelpAbout() {
  CAboutDlg().DoModal();
}

void CRegexDemoDlg::OnHelpShowctrlid() {
  showInformation(_T("Focus control has Id:%d"), getFocusCtrlId(this));
}

void CRegexDemoDlg::OnGotoPattern() {
  getPatternWindow()->SetFocus();
}

void CRegexDemoDlg::OnGotoText() {
  getTargetWindow()->SetFocus();
}

void CRegexDemoDlg::OnGotoBytecode() {
  getCodeWindow()->SetFocus();
}

void CRegexDemoDlg::OnEditChangeComboPattern() {
  if(!m_patternDirty) {
    m_patternDirty = true;
    ajourDialogItems();
  }
}

void CRegexDemoDlg::OnSelChangeComboPattern() {
  OnEditChangeComboPattern();
}

void CRegexDemoDlg::OnEditChangeComboTarget() {
  if(!m_targetDirty) {
    m_targetDirty = true;
    ajourDialogItems();
  }
}

void CRegexDemoDlg::OnSelChangeComboTarget() {
  OnEditChangeComboTarget();
}

void CRegexDemoDlg::OnSelChangeListbyteCode() {
  ajourDialogItems();
}

LRESULT CRegexDemoDlg::OnMsgThreadRunning(WPARAM wp, LPARAM lp) {
  try {
    const bool newRunning = (lp != 0);
    if(newRunning) {
      unmarkCodeLine();
      unmarkAllCharacters(PATTERN_POSMARK);
    } else {
      if(m_regex.isCodeDirty()) {
        fillCodeWindow(getCompiledCodeText());
      }

      if(isThreadFinished()) {
        clearThreadState();
        if(m_debugThread->getRegexPhase() == REGEX_SUCEEDED) {
          if(m_debugThread->getCommand() == COMMAND_COMPILE) {
            setPatternCompiledOk();
          } else {
            showPatternFound();
          }
        } else {
          showResult(m_debugThread->getResultMsg());
          showCycleCount();
        }

      } else if(isThreadStopped()) {
        switch(m_debugThread->getRegexPhase()) {
        case REGEX_COMPILING: showCompilerState(); break;
        case REGEX_SEARCHING: showSearchState();   break;
        case REGEX_MATCHING : showMatchState();    break;
        case REGEX_UNDEFINED:
          if(m_debugThread->getCommand() == COMMAND_COMPILE) { // compiler errors are given as exception, so thread is in undefined state when this happens
            showCompilerError(m_debugThread->getResultMsg());
          } else {
            showResult(m_debugThread->getResultMsg()); break; // may an exception, access-violation or somthing like that
          }
        }
      }
    }
    ajourDialogItems();
  } catch(Exception e) {
    showException(e);
  }
  return 0;
}

bool CRegexDemoDlg::checkPattern() {
  if(m_patternDirty || !m_patternOk) {
    OnEditCompilePattern();
  } else {
    UpdateData();
  }
  return m_patternOk;
}

String CRegexDemoDlg::getThreadStateName() const {
  if(!hasThread()) {
    return _T("No thread");
  } else {
    const RegexPhaseType phase = m_debugThread->getRegexPhase();
    if(m_debugThread->isFinished() && (phase != REGEX_SUCEEDED)) {
      return _T("Failed");
    }
    switch(phase) {
    case REGEX_UNDEFINED : return _T("Undefined");
    case REGEX_COMPILING : return _T("Compiling");
    case REGEX_SEARCHING : return _T("Searching");
    case REGEX_MATCHING  : return _T("Matching");
    case REGEX_SUCEEDED  : return _T("Succeeded");
    }
  }
  return _T("?");
}

void CRegexDemoDlg::markCurrentChar(CharMarkType type, intptr_t index) {
  if(index < 0) {
    unmarkAllCharacters(type);
  } else {
    m_charMarkers[type]->setMark(index);
  }
}

void CRegexDemoDlg::unmarkAllCharacters(CharMarkType type) {
  m_charMarkers[type]->unmarkAll();
}

void CRegexDemoDlg::unmarkAllCharacters() {
  for(size_t i = 0; i < m_charMarkers.size(); i++) {
    m_charMarkers[i]->unmarkAll();
  }
}

void CRegexDemoDlg::markMultiPatternChars(const BitSet &markSet) {
  CharacterMarker &marker = *m_charMarkers[PATTERN_POSMARK];
  marker.setMultiMarksAllowed(true);
  marker.setMarks(markSet);
}

typedef enum {
  WIN_REGISTERS
 ,WIN_STACK
 ,MENU_SEARCH
 ,MENU_DEBUG
 ,MENU_BREAKPOINTS
 ,MENU_SHOWDFATABLES
 ,MENU_DFAGRAPHICS
} DialogItemFlags;

void CRegexDemoDlg::ajourDialogItems() {
  BitSet16 flags;

  if(!m_patternDirty && m_patternOk) {
    flags.add(MENU_SEARCH);
  }
  if(isThreadStopped() && ((m_debugThread->getCommand() == COMMAND_COMPILE) || (!m_patternDirty && m_patternOk && !m_targetDirty))) {
    flags.add(MENU_DEBUG);
  }
  if(isMenuItemChecked(this, ID_OPTIONS_DFA_REGEX)) {
    flags.add(MENU_SHOWDFATABLES);
    flags.add(MENU_DFAGRAPHICS);
  }
  if((getCodeWindow()->GetCurSel()>=0) && (!isMenuItemChecked(this, ID_OPTIONS_DFA_REGEX) || !isMenuItemChecked(this, ID_OPTIONS_DFA_SHOWTABLES))) {
    flags.add(MENU_BREAKPOINTS);
  }

  setWindowText(this, IDC_STATICSTATENAME, getThreadStateName());
  if(hasThread()) {
    if(m_debugThread->isRunning()) {
      return;
    }
    switch(getThreadState()) {
    case REGEX_UNDEFINED:
      flags.remove(MENU_DEBUG);
      break;
    case REGEX_COMPILING:
      flags.add(WIN_REGISTERS);
      flags.add(WIN_STACK);
      break;
    case REGEX_SEARCHING:
      break;
    case REGEX_MATCHING :
      flags.add(WIN_REGISTERS);
      flags.add(WIN_STACK);
      break;
    case REGEX_SUCEEDED :
      flags.add(WIN_REGISTERS);
      break;
    default:
      showError(_T("%s:Unnknown threadState:%d"), __TFUNCTION__, getThreadState());
      break;
    }
  }
  GetDlgItem(IDC_STATICCYCLES)->EnableWindow(m_patternOk);
  enableDialogItems(flags);
  setRegisterWindowMode();
}

void CRegexDemoDlg::enableDialogItems(BitSet16 flags) {
  const BOOL enableRegisters  = flags.contains(WIN_REGISTERS);
  const BOOL enableMatchStack = flags.contains(WIN_STACK    );

  GetDlgItem(IDC_STATICREGISTERSLABEL   )->EnableWindow(enableRegisters );
  GetDlgItem(IDC_STATICREGISTERS        )->EnableWindow(enableRegisters );
  GetDlgItem(IDC_STATICSTACKLABEL       )->EnableWindow(enableMatchStack);
  GetDlgItem(IDC_STATICSTACK            )->EnableWindow(enableMatchStack);

  enableMenuItem(this, ID_DEBUG_TOGGLEBREAKPOINT, flags.contains(MENU_BREAKPOINTS));

  const bool enableSearch = flags.contains(MENU_SEARCH);
  enableMenuItem(           this, ID_EDIT_FIND               , enableSearch);
  enableMenuItem(           this, ID_EDIT_MATCH              , enableSearch);
  enableMenuItem(           this, ID_DEBUG_FIND              , enableSearch);
  enableMenuItem(           this, ID_DEBUG_MATCH             , enableSearch);

  const bool enableDebug = flags.contains(MENU_DEBUG);
  enableMenuItem(           this, ID_DEBUG_CONTINUE          , enableDebug);
  enableMenuItem(           this, ID_DEBUG_STEP              , enableDebug);
  enableMenuItem(           this, ID_OPTIONS_DFA_SHOWTABLES  , flags.contains(MENU_SHOWDFATABLES));
  enableSubMenuContainingId(this, ID_OPTIONS_DFA_NO_GRAPHICS , flags.contains(MENU_DFAGRAPHICS  ));
}

void CRegexDemoDlg::setPatternCompiledOk() {
  const String codeText = getCompiledCodeText();

  if(codeText != getCodeWindow()->getText()) {
    fillCodeWindow(codeText);
  }
  m_patternDirty = false;
  m_patternOk    = true;
  m_patternCombo.updateList();
  ajourDialogItems();
  clearResult();
  paintRegex();
  if(m_target.GetLength() == 0) {
    OnGotoText();
  } else {
    OnGotoBytecode();
  }
}

CompileParameters CRegexDemoDlg::getCompileParameters() {
  UpdateData();
  const String pattern    = m_pattern;
  const bool   ignoreCase = isMenuItemChecked(this, ID_OPTIONS_IGNORECASE);
  return CompileParameters(pattern, ignoreCase);
}

void CRegexDemoDlg::showCompilerError(const String &errorMsg) {
  String msg = errorMsg;
  int errorIndex = -1;
  if(_stscanf(msg.cstr(), _T("(%d):"), &errorIndex) == 1) {
    intptr_t colon = msg.find(':');
    msg = substr(msg, colon+1, errorMsg.length());
  }
  clearCodeWindow();
  showResult(msg);
  m_patternDirty = true;
  m_patternOk    = false;
  ajourDialogItems();
  OnGotoPattern();
  if(errorIndex >= 0) {
    getPatternWindow()->SetEditSel(errorIndex, errorIndex);
  }
}

void CRegexDemoDlg::startDebugCompile() {
  try {
    killThread();
    clearResult();
    clearCodeWindow();
    const String pattern = m_pattern;

    m_debugThread = new DebugThread(m_regex, getCompileParameters(), getCodeWindow()->getBreakPoints()); TRACE_NEW(m_debugThread);
    m_debugThread->addPropertyChangeListener(this);
    m_debugThread->singleStep();
  } catch(Exception e) {
    showException(e);
  }
}

void CRegexDemoDlg::startThread(ThreadCommand command, bool singleStep) {
  try {
    killThread();
    clearResult();
    m_targetCombo.updateList();
    m_targetDirty = false;
    getTargetWindow()->SetEditSel(-1,-1);
    const String target = m_target;
    m_debugThread = new DebugThread(command, m_regex, target, getCodeWindow()->getBreakPoints()); TRACE_NEW(m_debugThread);
    m_debugThread->addPropertyChangeListener(this);
    if(singleStep) {
      m_debugThread->singleStep();
    } else {
      m_debugThread->go();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CRegexDemoDlg::killThread() {
  if(m_debugThread != NULL) {
    m_debugThread->removePropertyChangeListener(this);
    SAFEDELETE(m_debugThread);
  }
  unmarkAllCharacters();
}

void CRegexDemoDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  switch(id) {
  case SEARCH_RUNNING      :
    { const bool oldRunning = *(bool*)oldValue;
      const bool newRunning = *(bool*)newValue;
      PostMessage(ID_MSG_THREADRUNNING, oldRunning, newRunning);
    }
    break;
  case SEARCH_REGEXFINISHED:
    break;
  default:
    showError(_T("%s:Unknown property:%d"), __TFUNCTION__,id);
    break;
  }
}

void CRegexDemoDlg::clearThreadState() {
  unmarkCodeLine();
  setWindowText(this, IDC_STATICSTACK, EMPTYSTRING);
  unmarkAllCharacters(SEARCH_POSMARK );
  unmarkAllCharacters(MATCH_STARTMARK);
  unmarkAllCharacters(MATCH_DMARK    );
}

void CRegexDemoDlg::showCompilerState() {
  switch(m_regex.getType()) {
  case EMACS_REGEX:
    showEmacsCompilerState();
    break;
  case DFA_REGEX:
    showDFACompilerState();
    break;
  }
}

void CRegexDemoDlg::showEmacsCompilerState() {
  const _RegexCompilerState  &state = m_debugThread->getEmacsCompilerState();
  if(state.m_codeText != getCodeWindow()->getText()) {
    fillCodeWindow(state.m_codeText);
    getCodeWindow()->scrollToBottom();
  }
  markCurrentChar(PATTERN_POSMARK, state.m_scannerIndex);

  String stateStr = format(_T("State:\n%s"), state.m_currentState.cstr());
  if(state.m_usedCharSets.length() > 0) {
    stateStr += _T("\n\nCharacterSets:\n") + state.m_usedCharSets;
  }
  if(state.m_fastMap.length() > 0) {
    stateStr += _T("\n\nFastMap:\n") + state.m_fastMap;
  }
  if(state.m_registerInfo.length() > 0) {
    stateStr += _T("\n\nRegisters:\n") + state.m_registerInfo;
  }
  getRegistersWindow()->SetWindowText(stateStr.replace('\n',_T("\r\n")).cstr());

  String stackStr = state.m_compilerStack;
  m_stackWindow.SetWindowText(stackStr.replace('\n',_T("\r\n")).cstr());
}

bool CRegexDemoDlg::isGraphicsOn() {
  switch(getDFAGraphicsMode()) {
  case ID_OPTIONS_DFA_NO_GRAPHICS   :
    return false;
  case ID_OPTIONS_DFA_PAINT_STATES  :
  case ID_OPTIONS_DFA_ANIMATE_CREATE:
    return true;
  default:
    showError(_T("%s:Unknown DFAGraphicsmode:%d"), __TFUNCTION__,getDFAGraphicsMode());
    return false;
  }
}

void CRegexDemoDlg::showDFACompilerState() {
  const _DFARegexCompilerState &state = m_debugThread->getDFACompilerState();
  if(state.m_codeText != getCodeWindow()->getText()) {
    fillCodeWindow(state.m_codeText);
    getCodeWindow()->scrollToBottom();
  }
  markCurrentChar(PATTERN_POSMARK, state.m_scannerIndex);
  String stackStr = state.m_compilerStack;
  m_stackWindow.SetWindowText(stackStr.replace('\n',_T("\r\n")).cstr());
  switch(getDFAGraphicsMode()) {
  case ID_OPTIONS_DFA_NO_GRAPHICS   :
    break;
  case ID_OPTIONS_DFA_PAINT_STATES  :
    paintRegex();
    break;
  case ID_OPTIONS_DFA_ANIMATE_CREATE:
    paintRegex(false, true);
    break;
  }
}

void CRegexDemoDlg::showSearchState() {
  switch(m_regex.getType()) {
  case EMACS_REGEX:
    showEmacsSearchState();
    break;
  case DFA_REGEX:
    showDFASearchState();
    break;
  }
  showCycleCount();
}

void CRegexDemoDlg::showEmacsSearchState() {
  const _RegexSearchState &state = m_debugThread->getEmacsSearchState();
  unmarkAllCharacters(MATCH_STARTMARK);
  unmarkAllCharacters(MATCH_DMARK    );
  markCurrentChar(  SEARCH_POSMARK, state.m_charIndex);
}

void CRegexDemoDlg::showDFASearchState() {
  const _DFARegexSearchState &state = m_debugThread->getDFASearchState();
  unmarkAllCharacters(MATCH_STARTMARK);
  unmarkAllCharacters(MATCH_DMARK    );
  markCurrentChar(  SEARCH_POSMARK, state.m_charIndex);
}

void CRegexDemoDlg::showMatchState() { // assume thread exists and is stopped but not finished
  switch(m_regex.getType()) {
  case EMACS_REGEX:
    showEmacsMatchState();
    break;
  case DFA_REGEX:
    showDFAMatchState();
    break;
  }
}

void CRegexDemoDlg::showEmacsMatchState() { // assume thread exists and is stopped but not finished
  const _RegexMatchState &state = m_debugThread->getEmacsMatchState();
  setCurrentCodeLine(state.getDBGLineNumber());
  markCurrentChar(PATTERN_POSMARK, state.getDBGPatternCharIndex());
  setWindowText(getRegistersWindow(), state.registersToString());
  showMatchStack(state);

  unmarkAllCharacters(SEARCH_POSMARK);
  markCurrentChar(    MATCH_STARTMARK, state.getPos());
  if(state.m_sp == NULL) {
    unmarkAllCharacters(MATCH_DMARK);
  } else {
    const intptr_t fromEnd   = state.m_spEnd - state.m_sp;
    const intptr_t fromStart = m_target.GetLength() - fromEnd;
    markCurrentChar(MATCH_DMARK, fromStart);
  }
}

void CRegexDemoDlg::showDFAMatchState() {
  const _DFARegexMatchState &state = m_debugThread->getDFAMatchState();
  setCurrentCodeLine(state.getDBGLineNumber());
  const BitSet *patternIndexSet = state.getDBGPatternIndexSet();
  if(patternIndexSet) {
    markMultiPatternChars(*patternIndexSet);
  } else {
    unmarkAllCharacters(PATTERN_POSMARK);
  }

  unmarkAllCharacters(SEARCH_POSMARK );
  markCurrentChar(MATCH_STARTMARK, state.getPos());
  markCurrentChar(MATCH_DMARK    , state.getDBGTextCharIndex());
  markCurrentChar(LASTACCEPT_MARK, state.getDBGLastAcceptIndex());
  showCycleCount();
  paintRegex();
}

void CRegexDemoDlg::showPatternFound() {
  showResult(m_debugThread->getResultMsg(), m_debugThread->registersToString());
  showCycleCount();
  getTargetWindow()->SetFocus();
  markFoundPattern();
  unmarkAllCharacters();
  paintRegex();
  setCurrentCodeLine(m_regex.getPatternFoundCodeLine());
}

void CRegexDemoDlg::markFoundPattern() {
  int start, end;
  m_debugThread->getFoundPosition(start, end);
  getTargetWindow()->SetEditSel(start, end);
}

void CRegexDemoDlg::unmarkFoundPattern() {
  getTargetWindow()->SetEditSel(-1,-1);
}

void CRegexDemoDlg::clearCyclesWindow() {
  showCyclesText(EMPTYSTRING);
}

void CRegexDemoDlg::showCycleCount() {
  showCyclesText(format(_T("Cycles:%d"), m_regex.getCycleCount()));
}

void CRegexDemoDlg::showCyclesText(const String &text) {
  setWindowText(this, IDC_STATICCYCLES, text);
}

void CRegexDemoDlg::showResult(const String &result, const String &registerString) {
  setWindowText(this, IDC_STATICRESULT, result);
  setWindowText(getRegistersWindow(), registerString);
}

void CRegexDemoDlg::clearRegisterWindow() {
  setWindowText(getRegistersWindow(), EMPTYSTRING);
}

void CRegexDemoDlg::setRegisterWindowMode() {
  if(isMenuItemChecked(this, ID_OPTIONS_DFA_REGEX) && isGraphicsOn()) {
    GetDlgItem(IDC_STATICREGISTERSLABEL)->ShowWindow(SW_HIDE);
    getRegistersWindow()->ShowWindow(SW_HIDE);
/*
    CRect rect;
    rect.left   = getWindowRect(this, IDC_STATICSTACKLABEL).left;
    rect.top    = getWindowRect(this, IDC_STATICRESULT).top;
    rect.right  = getWindowRect(this, IDC_STATICSTACK).right;
    rect.bottom = getClientRect(this).Height() - 200;
*/
    getGraphicsWindow()->ShowWindow(SW_SHOW);
    setCylceAndStackWindowTop(getWindowRect(getGraphicsWindow()).bottom);
  } else {
    GetDlgItem(IDC_STATICREGISTERSLABEL)->ShowWindow(SW_SHOW);
    getRegistersWindow()->ShowWindow(SW_SHOW);
    unpaintRegex();
    getGraphicsWindow()->ShowWindow(SW_HIDE);
    setCylceAndStackWindowTop(getWindowRect(getRegistersWindow()).bottom);
  }
}

void CRegexDemoDlg::setCylceAndStackWindowTop(int top) {
  CRect cycleRect = getWindowRect(this, IDC_STATICCYCLES);
  CRect stackRect = getWindowRect(getStackWindow());
  CRect labelRect = getWindowRect(this, IDC_STATICSTACKLABEL);

  int dt = top - cycleRect.top;

  cycleRect.top    += dt;
  cycleRect.bottom += dt;
  labelRect.top    += dt;
  labelRect.bottom += dt;
  stackRect.top    += dt;
  setWindowRect(getStackWindow()          , stackRect);
  setWindowRect(this, IDC_STATICCYCLES    , cycleRect);
  setWindowRect(this, IDC_STATICSTACKLABEL, labelRect);
}

void CRegexDemoDlg::showMatchStack(const _RegexMatchState &state) {
  m_stackWindow.SetWindowText(state.stackToString().cstr());
  showCyclesText(format(_T("Cycles:%d %s"), m_regex.getCycleCount(), state.countersToString().cstr()));
}

void CRegexDemoDlg::clearCodeWindow() {
  getCodeWindow()->setText(EMPTYSTRING);
  setWindowText(this, IDC_STATICFASTMAP, EMPTYSTRING);
  m_codeWindow.setAllowMarking(false);
  unmarkAllCharacters();
}

String CRegexDemoDlg::getCompiledCodeText() const {
  if(isCodeTextDFATables()) {
    return m_regex.getDFATablesToString();
  } else {
    return m_regex.codeToString();
  }
}

void CRegexDemoDlg::fillCodeWindow(const String &codeText) {
  CDebugTextWindow *cw = getCodeWindow();

  cw->setText(codeText);

  if(m_regex.isCompiled() && !isCodeTextDFATables()) {
    const BitSet blSet = m_regex.getPossibleBreakPointLines();
    cw->setAllowMarking(true, &blSet);
  } else {
    cw->setAllowMarking(false);
  }

  String fastMapStr;
  if(m_regex.isCompiled()) {
    fastMapStr = m_regex.fastMapToString();
    if(m_regex.getMatchEmpty()) {
      fastMapStr += _T("\r\nMatch empty string");
    }
  }
  setWindowText(this, IDC_STATICFASTMAP, fastMapStr);
}

void CRegexDemoDlg::paintRegex(bool msgPaint, bool animate) {
  if(isGraphicsOn()) {
    CWnd *wnd = getGraphicsWindow();
    if(msgPaint) {
      m_regex.paint(wnd, CPaintDC(wnd), animate);
    } else {
      m_regex.paint(wnd, CClientDC(wnd), animate);
    }
  }
}

void CRegexDemoDlg::unpaintRegex() {
  CWnd *wnd = getGraphicsWindow();
  m_regex.unpaintAll(wnd, CClientDC(wnd));
}

bool CRegexDemoDlg::isCodeTextDFATables() const {
  return isMenuItemChecked(this, ID_OPTIONS_DFA_REGEX) && isMenuItemChecked(this, ID_OPTIONS_DFA_SHOWTABLES) && m_regex.hasDFATables();
}

void CRegexDemoDlg::setDFAGraphicsMode(int id) {
  static const int menuIds[] = {
    ID_OPTIONS_DFA_NO_GRAPHICS
   ,ID_OPTIONS_DFA_PAINT_STATES
   ,ID_OPTIONS_DFA_ANIMATE_CREATE
  };
  for(int i = 0; i < ARRAYSIZE(menuIds); i++) {
    checkMenuItem(this, menuIds[i], menuIds[i] == id);
  }
  ajourDialogItems();
  if(id != ID_OPTIONS_DFA_NO_GRAPHICS) {
    if(m_regex.isCompiled()) {
      paintRegex();
    }
  }
}

int CRegexDemoDlg::getDFAGraphicsMode() {
  if(!isMenuItemChecked(this, ID_OPTIONS_DFA_REGEX)) {
    return ID_OPTIONS_DFA_NO_GRAPHICS;
  }
  if(isMenuItemChecked(this, ID_OPTIONS_DFA_PAINT_STATES)) {
    return ID_OPTIONS_DFA_PAINT_STATES;
  } else if(isMenuItemChecked(this, ID_OPTIONS_DFA_ANIMATE_CREATE)) {
    return ID_OPTIONS_DFA_ANIMATE_CREATE;
  } else {
    return ID_OPTIONS_DFA_NO_GRAPHICS;
  }
}
