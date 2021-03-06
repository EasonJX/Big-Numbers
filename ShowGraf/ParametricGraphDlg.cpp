#include "stdafx.h"
#include <FileNameSplitter.h>
#include "ParametricGraphDlg.h"

CParametricGraphDlg::CParametricGraphDlg(ParametricGraphParameters &param, CWnd *pParent)
: SaveLoadExprDialog<ParametricGraphParameters>(IDD, pParent, param, _T("parametric curve"), _T("par"))
, m_createListFile(FALSE)
{
}

CParametricGraphDlg::~CParametricGraphDlg() {
}

void CParametricGraphDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_CBString(  pDX, IDC_COMBOSTYLE         , m_style         );
  DDX_Check(     pDX, IDC_CHECKCREATELISTFILE, m_createListFile);
  DDX_Text(      pDX, IDC_EDITCOMMON         , m_commonText    );
  DDX_Text(      pDX, IDC_EDITEXPRX          , m_exprX         );
  DDX_Text(      pDX, IDC_EDITEXPRY          , m_exprY         );
  DDX_Text(      pDX, IDC_EDITTFROM          , m_tFrom         );
  DDX_Text(      pDX, IDC_EDITTTO            , m_tTo           );
  DDX_Text(      pDX, IDC_EDITSTEPS          , m_steps         );
  DDV_MinMaxUInt(pDX, m_steps, 1, 10000);
}

BEGIN_MESSAGE_MAP(CParametricGraphDlg, CDialog)
  ON_WM_SIZE()
  ON_COMMAND(   ID_FILE_NEW                                       , OnFileNew                   )
  ON_COMMAND(   ID_FILE_OPEN                                      , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                                      , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS                                   , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS                    , OnEditFindmatchingparentesis)
  ON_BN_CLICKED(IDC_BUTTON_HELPX                                  , OnButtonHelpX               )
  ON_BN_CLICKED(IDC_BUTTON_HELPY                                  , OnButtonHelpY               )
END_MESSAGE_MAP()

BOOL CParametricGraphDlg::OnInitDialog() {
  __super::OnInitDialog();

  createExprHelpButton(IDC_BUTTON_HELPX, IDC_EDITEXPRX);
  createExprHelpButton(IDC_BUTTON_HELPY, IDC_EDITEXPRY);
  setCommonExprFieldId(IDC_EDITCOMMON);
  gotoEditBox(this, IDC_EDITEXPRX);
  return FALSE;
}

bool CParametricGraphDlg::validate() {
  if(!validateAllExpr()) {
    return false;
  }
  if(!validateInterval(IDC_EDITTFROM, IDC_EDITTTO)) {
    return false;
  }
  return true;
}

void CParametricGraphDlg::addToRecent(const String &fileName) {
  theApp.AddToRecentFileList(fileName.cstr());
}

String CParametricGraphDlg::getListFileName() const {
  if(!m_createListFile) return __super::getListFileName();
  return FileNameSplitter(getData().getName()).setExtension(_T("lst")).getFullPath();
}

void CParametricGraphDlg::OnEditFindmatchingparentesis() {
  gotoMatchingParentesis();
}

void CParametricGraphDlg::OnButtonHelpX() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPX);
}

void CParametricGraphDlg::OnButtonHelpY() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPY);
}

void CParametricGraphDlg::paramToWin(const ParametricGraphParameters &param) {
  m_style      = param.getGraphStyleStr();
  getColorButton()->SetColor(param.getColor());
  m_commonText = param.getCommonText().cstr();
  m_exprX      = param.getExprXText(false).cstr();
  m_exprY      = param.getExprYText(false).cstr();
  m_tFrom      = param.getInterval().getMin();
  m_tTo        = param.getInterval().getMax();
  m_steps      = param.getSteps();
  __super::paramToWin(param);
}

bool CParametricGraphDlg::winToParam(ParametricGraphParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.setCommonText((LPCTSTR)m_commonText);
  param.setExprXText((LPCTSTR)m_exprX);
  param.setExprYText((LPCTSTR)m_exprY);
  param.setGraphStyle((GraphStyle)getStyleCombo()->GetCurSel());
  param.setColor(getColorButton()->GetColor());
  param.setInteval(m_tFrom, m_tTo);
  param.setSteps(m_steps);
  return true;
}
