#include "stdafx.h"
#include "Function2DSurfaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFunction2DSurfaceDlg::CFunction2DSurfaceDlg(const Function2DSurfaceParameters &param, CWnd *pParent)
  : SaveLoadExprDialog<Function2DSurfaceParameters>(IDD, pParent, param, _T("expression"), _T("exp"))
{
}

void CFunction2DSurfaceDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX , IDC_EDIT_EXPR             , m_expr                      );
    DDX_Text(pDX , IDC_EDIT_XFROM            , m_xfrom                     );
    DDX_Text(pDX , IDC_EDIT_XTO              , m_xto                       );
    DDX_Text(pDX , IDC_EDIT_YFROM            , m_yfrom                     );
    DDX_Text(pDX , IDC_EDIT_YTO              , m_yto                       );
    DDX_Text(pDX , IDC_EDIT_POINTS           , m_pointCount                );
    DDX_Check(pDX, IDC_CHECK_MACHINECODE     , m_machineCode               );
    DDX_Check(pDX, IDC_CHECK_DOUBLESIDED     , m_doubleSided               );
    DDX_Check(pDX, IDC_CHECK_INCLUDETIME     , m_includeTime               );
    DDX_Text(pDX , IDC_EDIT_FRAMECOUNT       , m_frameCount                );
    DDV_MinMaxUInt(pDX, m_frameCount, 1, 300            );
    DDX_Text(pDX , IDC_EDIT_TIMEFROM         , m_timeFrom                  );
    DDX_Text(pDX , IDC_EDIT_TIMETO           , m_timeTo                    );
}

BEGIN_MESSAGE_MAP(CFunction2DSurfaceDlg, CDialog)
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                       )
    ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                       )
    ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                     )
    ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis     )
    ON_COMMAND(ID_GOTO_EXPR                  , OnGotoExpr                       )
    ON_COMMAND(ID_GOTO_XINTERVAL             , OnGotoXInterval                  )
    ON_COMMAND(ID_GOTO_YINTERVAL             , OnGotoYInterval                  )
    ON_COMMAND(ID_GOTO_TIMEINTERVAL          , OnGotoTimeInterval               )
    ON_COMMAND(ID_GOTO_POINTCOUNT            , OnGotoPointCount                 )
    ON_COMMAND(ID_GOTO_FRAMECOUNT            , OnGotoFrameCount                  )
    ON_COMMAND_RANGE(ID_EXPRHELP_MENU_FIRST  , ID_EXPRHELP_MENU_LAST, OnExprHelp)
    ON_BN_CLICKED(IDC_BUTTON_HELP            , OnButtonHelp                     )
    ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnCheckIncludeTime               )
END_MESSAGE_MAP()

BOOL CFunction2DSurfaceDlg::OnInitDialog() {
  __super::OnInitDialog();

  createHelpButton();

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATIC_FUNCTION     , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPR           , RELATIVE_SIZE        );
  m_layoutManager.addControl(IDC_STATIC_XINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_XFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH1        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_XTO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_YINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_YFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH2        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_YTO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_POINTCOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_POINTS         , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_MACHINECODE   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_INCLUDETIME   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_DOUBLESIDED   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_TIMEINTERVAL , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMEFROM       , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH3        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMETO         , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_FRAMECOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_FRAMECOUNT     , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDOK                    , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDCANCEL                , RELATIVE_POSITION    );

  setExprFont();

  gotoEditBox(this, IDC_EDIT_EXPR);
  return FALSE;  // return TRUE  unless you set the focus to a control
}

void CFunction2DSurfaceDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

#define MAXPOINTCOUNT 200
#define MAXFRAMECOUNT 300

bool CFunction2DSurfaceDlg::validate() {
  if(!validateExpr()) {
    return false;
  }
  if(m_pointCount <= 0 || m_pointCount > MAXPOINTCOUNT) {
    gotoEditBox(this, IDC_EDIT_POINTS);
    Message(_T("Number of points must be between 0 and %d"), MAXPOINTCOUNT);
    return false;
  }
  if(!validateInterval(IDC_EDIT_XFROM, IDC_EDIT_XTO)) {
    return false;
  }
  if(!validateInterval(IDC_EDIT_YFROM,IDC_EDIT_YTO)) {
    return false;
  }

  if(m_includeTime) {
    if(m_frameCount <= 0 || m_frameCount > MAXFRAMECOUNT) {
      gotoEditBox(this, IDC_EDIT_FRAMECOUNT);
      Message(_T("Number of frames must be between 1 and %d"), MAXFRAMECOUNT);
      return false;
    }
    if(!validateInterval(IDC_EDIT_TIMEFROM,IDC_EDIT_TIMETO)) {
      return false;
    }
  }
  return true;
}

void CFunction2DSurfaceDlg::OnCheckIncludeTime() {
  UpdateData(TRUE);
  enableTimeFields();
}

void CFunction2DSurfaceDlg::enableTimeFields() {
  BOOL enable = IsDlgButtonChecked(IDC_CHECK_INCLUDETIME);
  GetDlgItem(IDC_STATIC_TIMEINTERVAL)->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMEFROM      )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMETO        )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_FRAMECOUNT    )->EnableWindow(enable);
  setWindowText(this, IDC_STATIC_FUNCTION, enable ? _T("&z = F(t,x,y) =") : _T("&z = F(x,y) ="));
}

void CFunction2DSurfaceDlg::OnEditFindMatchingParentesis() {
  gotoMatchingParentesis();
}

void CFunction2DSurfaceDlg::OnGotoExpr() {
  gotoExpr();
}

void CFunction2DSurfaceDlg::OnGotoXInterval() {
  gotoEditBox(this, IDC_EDIT_XFROM);
}

void CFunction2DSurfaceDlg::OnGotoYInterval() {
  gotoEditBox(this, IDC_EDIT_YFROM);
}

void CFunction2DSurfaceDlg::OnGotoPointCount() {
  gotoEditBox(this, IDC_EDIT_POINTS);
}

void CFunction2DSurfaceDlg::OnGotoTimeInterval() {
  gotoEditBox(this, IDC_EDIT_TIMEFROM);
}

void CFunction2DSurfaceDlg::OnGotoFrameCount() {
  gotoEditBox(this, IDC_EDIT_FRAMECOUNT);
}

void CFunction2DSurfaceDlg::OnButtonHelp() {
  showExprHelpMenu();
}

void CFunction2DSurfaceDlg::OnExprHelp(UINT id) {
  handleSelectedExprHelpId(id);
}

void CFunction2DSurfaceDlg::paramToWin(const Function2DSurfaceParameters &param) {
  m_expr          = param.m_expr.cstr();
  m_xfrom         = param.getXInterval().getMin();
  m_xto           = param.getXInterval().getMax();
  m_yfrom         = param.getYInterval().getMin();
  m_yto           = param.getYInterval().getMax();
  m_timeFrom      = param.getTimeInterval().getMin();
  m_timeTo        = param.getTimeInterval().getMax();
  m_pointCount    = param.m_pointCount;
  m_frameCount    = param.m_frameCount;
  m_machineCode   = param.m_machineCode ? TRUE : FALSE;
  m_includeTime   = param.m_includeTime ? TRUE : FALSE;
  m_doubleSided   = param.m_doubleSided ? TRUE : FALSE;

  UpdateData(false);
  enableTimeFields();

  __super::paramToWin(param);
}

void CFunction2DSurfaceDlg::winToParam(Function2DSurfaceParameters &param) const {
  param.m_expr        = m_expr;
  param.m_xInterval.setFrom(   m_xfrom);
  param.m_xInterval.setTo(     m_xto);
  param.m_yInterval.setFrom(   m_yfrom);
  param.m_yInterval.setTo(     m_yto);
  param.m_timeInterval.setFrom(m_timeFrom);
  param.m_timeInterval.setTo(  m_timeTo);
  param.m_pointCount  = m_pointCount;
  param.m_frameCount  = m_frameCount;
  param.m_machineCode = m_machineCode?true:false;
  param.m_includeTime = m_includeTime?true:false;
  param.m_doubleSided = m_doubleSided?true:false;
  __super::winToParam(param);
}
