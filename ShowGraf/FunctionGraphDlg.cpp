#include "stdafx.h"
#include "FunctionGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFunctionGraphDlg::CFunctionGraphDlg(FunctionGraphParameters &param, int showFlags, CWnd *pParent)
: SaveLoadExprDialog<FunctionGraphParameters>(IDD, pParent, param, _T("expression"), _T("exp"))
, m_showFlags(showFlags)
{
}

void CFunctionGraphDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITEXPR, m_expr);
  DDX_Text(pDX, IDC_EDITXFROM, m_xFrom);
  DDX_Text(pDX, IDC_EDITXTO, m_xTo);
  DDX_Text(pDX, IDC_EDITSTEPS, m_steps);
  DDV_MinMaxUInt(pDX, m_steps, 1, 10000);
  DDX_CBString(pDX, IDC_COMBOSTYLE, m_style);
}

BEGIN_MESSAGE_MAP(CFunctionGraphDlg, CDialog)
  ON_WM_SIZE()
  ON_COMMAND(   ID_FILE_NEW                                       , OnFileNew                   )
  ON_COMMAND(   ID_FILE_OPEN                                      , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                                      , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS                                   , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS                    , OnEditFindmatchingparentesis)
  ON_COMMAND(   ID_GOTO_STYLE                                     , OnGotoStyle                 )
  ON_COMMAND(   ID_GOTO_EXPR                                      , OnGotoExpr                  )
  ON_COMMAND(   ID_GOTO_XINTERVAL                                 , OnGotoXInterval             )
  ON_COMMAND(   ID_GOTO_STEP                                      , OnGotoStep                  )
  ON_BN_CLICKED(IDC_BUTTON_HELP                                   , OnButtonHelp                )
END_MESSAGE_MAP()

BOOL CFunctionGraphDlg::OnInitDialog() {
  __super::OnInitDialog();

  createExprHelpButton(IDC_BUTTON_HELP, IDC_EDITEXPR);

  if(!(m_showFlags & SHOW_INTERVAL)) {
    GetDlgItem(IDC_STATICINTERVAL)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDITXFROM     )->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATICDASH    )->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDITXTO       )->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDITXFROM     )->ShowWindow(SW_HIDE);
  }
  if(!(m_showFlags & SHOW_STEP)) {
    GetDlgItem(IDC_STATICSTEPS   )->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDITSTEPS     )->ShowWindow(SW_HIDE);
  }

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDOK                , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDCANCEL            , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDC_STATICEXPRLABEL , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDITEXPR        , RELATIVE_SIZE        | RESIZE_FONT);

  if(m_showFlags & SHOW_INTERVAL) {
    m_layoutManager.addControl(IDC_STATICINTERVAL, RELATIVE_Y_POS);
    m_layoutManager.addControl(IDC_EDITXFROM     , RELATIVE_Y_POS);
    m_layoutManager.addControl(IDC_STATICDASH    , RELATIVE_Y_POS);
    m_layoutManager.addControl(IDC_EDITXTO       , RELATIVE_Y_POS);
    m_layoutManager.addControl(IDC_EDITXFROM     , RELATIVE_Y_POS);
  }
  if(m_showFlags & SHOW_STEP) {
    m_layoutManager.addControl(IDC_STATICSTEPS   , RELATIVE_Y_POS);
    m_layoutManager.addControl(IDC_EDITSTEPS     , RELATIVE_Y_POS);
  }
  gotoEditBox(this, IDC_EDITEXPR);
  return FALSE;
}

bool CFunctionGraphDlg::validate() {
  if(!validateExpr(IDC_EDITEXPR)) {
    return false;
  }
  if(m_showFlags & SHOW_INTERVAL) {
    if(!validateInterval(IDC_EDITXFROM, IDC_EDITXTO)) {
      return false;
    }
  }
  return true;
}

void CFunctionGraphDlg::OnGotoExpr() {
  gotoExpr(IDC_EDITEXPR);
}

void CFunctionGraphDlg::OnGotoStyle() {
  getStyleCombo()->SetFocus();
}

void CFunctionGraphDlg::OnGotoXInterval() {
  gotoEditBox(this,IDC_EDITXFROM);
}

void CFunctionGraphDlg::OnGotoStep() {
  gotoEditBox(this,IDC_EDITSTEPS);
}

void CFunctionGraphDlg::addToRecent(const String &fileName) {
  theApp.AddToRecentFileList(fileName.cstr());
}

void CFunctionGraphDlg::OnEditFindmatchingparentesis() {
  gotoMatchingParentesis();
}

void CFunctionGraphDlg::OnButtonHelp() {
  handleExprHelpButtonClick(IDC_BUTTON_HELP);
}

void CFunctionGraphDlg::paramToWin(const FunctionGraphParameters &param) {
  m_style    = param.getGraphStyleStr();
  getColorButton()->SetColor(param.getColor());
  m_expr     = param.getExprText().cstr();
  m_xFrom    = param.getInterval().getMin();
  m_xTo      = param.getInterval().getMax();
  m_steps    = param.getSteps();
  __super::paramToWin(param);
}

bool CFunctionGraphDlg::winToParam(FunctionGraphParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.setExprText((LPCTSTR)m_expr);
  param.setGraphStyle((GraphStyle)getStyleCombo()->GetCurSel());
  param.setColor(getColorButton()->GetColor());
  param.setInteval(m_xFrom,m_xTo);
  param.setSteps(m_steps);
  return true;
}
