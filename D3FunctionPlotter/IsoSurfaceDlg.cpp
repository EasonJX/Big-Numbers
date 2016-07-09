#include "stdafx.h"
#include "IsoSurfaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
static char *torusexpr  = "sqr(sqr(x)+sqr(y)+sqr(z)+0.25-0.01) - 4*0.25*(sqr(y)+sqr(z))";
static char *jackexpr   = "(1/(sqr(x)/9+4*sqr(y)+4*sqr(z))**4 +"
                          " 1/(sqr(y)/9+4*sqr(x)+4*sqr(z))**4 +"
                          " 1/(sqr(z)/9+4*sqr(y)+4*sqr(x))**4 +"
                          " 1/((1.3333333*x-4)**2+1.77777*sqr(y)+1.77777*sqr(z))**4 +"
                          " 1/((1.3333333*x+4)**2+1.77777*sqr(y)+1.77777*sqr(z))**4 +"
                          " 1/((1.3333333*y-4)**2+1.77777*sqr(x)+1.77777*sqr(z))**4 +"
                          " 1/((1.3333333*y+4)**2+1.77777*sqr(x)+1.77777*sqr(z))**4)**-0.25-1";
static char *wifflecube = "1-(sqr(0.4348*x)+sqr(0.4348*y)+sqr(0.4348*z))**-6 -"
                          "((0.5*x)**8+(0.5*y)**8+(0.5*z)**8)**6";

static char *blob3      = "s1=1/max(sqr(x+1)+sqr(y  )+sqr(z  ),0.00001);"
                          "s2=1/max(sqr(x  )+sqr(y+1)+sqr(z  ),0.00001);"
                          "s3=1/max(sqr(x  )+sqr(y  )+sqr(z+1),0.00001);"
                          "4 - s1 - s2 - s3";

*/
 
CIsoSurfaceDlg::CIsoSurfaceDlg(const IsoSurfaceParameters &param, CWnd* pParent /*=NULL*/) : CExprDialog(CIsoSurfaceDlg::IDD, pParent), m_param(param) {
}

void CIsoSurfaceDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text( pDX, IDC_EDIT_EXPR             , m_expr                      );
    DDX_Text( pDX, IDC_EDIT_SIZE             , m_size                      );
    DDX_Check(pDX, IDC_CHECK_TETRAHEDRAL     , m_tetrahedral               );
	DDX_Check(pDX, IDC_CHECK_ADAPTIVESIZE    , m_adaptiveCellSize          );
    DDX_Check(pDX, IDC_CHECK_ORIGINOUTSIDE   , m_originOutside             );
    DDX_Check(pDX, IDC_CHECK_MACHINECODE     , m_machineCode               );
	DDX_Check(pDX, IDC_CHECK_DOUBLESIDED     , m_doubleSided               );
	DDX_Check(pDX, IDC_CHECK_INCLUDETIME     , m_includeTime               );
	DDX_Text( pDX, IDC_EDIT_TIMECOUNT        , m_timeCount                 );
	DDX_Text( pDX, IDC_EDIT_TIMEFROM         , m_timeFrom                  );
	DDX_Text( pDX, IDC_EDIT_TIMETO           , m_timeTo                    );
	DDX_Text( pDX, IDC_EDIT_XFROM            , m_xfrom                     );
	DDX_Text( pDX, IDC_EDIT_XTO              , m_xto                       );
	DDX_Text( pDX, IDC_EDIT_YFROM            , m_yfrom                     );
	DDX_Text( pDX, IDC_EDIT_YTO              , m_yto                       );
	DDX_Text( pDX, IDC_EDIT_ZFROM            , m_zfrom                     );
	DDX_Text( pDX, IDC_EDIT_ZTO              , m_zto                       );
}


BEGIN_MESSAGE_MAP(CIsoSurfaceDlg, CDialog)
	ON_WM_SIZE()
    ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                  )
    ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                  )
    ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                )
    ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis)
    ON_COMMAND(ID_GOTO_EXPR                  , OnGotoExpr                  )
    ON_COMMAND(ID_GOTO_SIZE                  , OnGotoSize                  )
    ON_COMMAND(ID_GOTO_TINTERVAL             , OnGotoTInterval             )
    ON_COMMAND(ID_GOTO_TIMECOUNT             , OnGotoTimeCount             )
    ON_COMMAND_RANGE(ID_EXPRHELP_MENU_FIRST, ID_EXPRHELP_MENU_LAST, OnExprHelp)
	  ON_BN_CLICKED(IDC_BUTTON_HELP            , OnButtonHelp                )
	  ON_BN_CLICKED(IDC_CHECK_DOUBLESIDED      , OnCheckDoublesided          )
    ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnCheckIncludeTime          )
END_MESSAGE_MAP()

BOOL CIsoSurfaceDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  createHelpButton();

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_EDIT_EXPR           , RELATIVE_SIZE          );
  m_layoutManager.addControl(IDC_STATIC_FUNCTION     , PCT_RELATIVE_Y_CENTER  );
  m_layoutManager.addControl(IDC_STATIC_EQUAL_ZERO   , PCT_RELATIVE_Y_CENTER | RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_STATIC_SIZE         , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_SIZE           , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_TETRAHEDRAL   , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_ADAPTIVESIZE  , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_DOUBLESIDED   , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_ORIGINOUTSIDE , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_MACHINECODE   , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_INCLUDETIME   , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_DOUBLESIDED   , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_BOUNDINGBOX  , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_XINTERVAL    , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_XFROM          , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_DASH1        , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_XTO            , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_YINTERVAL    , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_YFROM          , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_DASH2        , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_YTO            , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_ZINTERVAL    , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_ZFROM          , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_DASH3        , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_ZTO            , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_TIMEINTERVAL , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_TIMEFROM       , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_DASH4        , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_TIMETO         , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_TIMECOUNT    , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_TIMECOUNT      , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDOK                    , RELATIVE_POSITION      );
  m_layoutManager.addControl(IDCANCEL                , RELATIVE_POSITION      );

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_ISOSURFACE_ACCELERATOR));
  setExprFont();

  paramToWin(m_param);
  GetDlgItem(IDC_EDIT_EXPR)->SetFocus();
  return FALSE;
}

void CIsoSurfaceDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

BOOL CIsoSurfaceDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

#define MAXTIMECOUNT 200

bool CIsoSurfaceDlg::validate() {
  if(!CExprDialog::validate()) {
    return false;
  }
  if(m_size <= 0) {
    OnGotoSize();
    Message(_T("Size must be > 0"));
    return false;
  }
  if(m_xfrom >= m_xto) {
    gotoEditBox(this, IDC_EDIT_XFROM);
    Message(_T("Invalid interval"));
    return false;
  }
  if(m_yfrom >= m_yto) {
    gotoEditBox(this, IDC_EDIT_YFROM);
    Message(_T("Invalid interval"));
    return false;
  }
  if(m_zfrom >= m_zto) {
    gotoEditBox(this, IDC_EDIT_ZFROM);
    Message(_T("Invalid interval"));
    return false;
  }
  if(m_includeTime) {
    if(m_timeCount <= 0 || m_timeCount > MAXTIMECOUNT) {
      gotoEditBox(this, IDC_EDIT_TIMECOUNT);
      Message(_T("Number of times must be between 1 and %d"), MAXTIMECOUNT);
      return false;
    }
    if(m_timeFrom >= m_timeTo) {
      gotoEditBox(this, IDC_EDIT_TIMEFROM);
      Message(_T("Invalid interval"));
      return false;
    }
  }
  return true;
}

void CIsoSurfaceDlg::OnOK() {
  UpdateData();
  if(!validate()) {
    return;
  }
  winToParam(m_param);
  CDialog::OnOK();
}

void CIsoSurfaceDlg::OnCheckIncludeTime() {
  UpdateData(TRUE);
  enableTimeFields();
}

void CIsoSurfaceDlg::enableTimeFields() {
  BOOL enable = IsDlgButtonChecked(IDC_CHECK_INCLUDETIME);
  GetDlgItem(IDC_STATIC_TIMEINTERVAL)->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMEFROM      )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMETO        )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMECOUNT     )->EnableWindow(enable);
  setWindowText(this, IDC_STATIC_FUNCTION, enable ? _T("F(t,&x,y,z) =") : _T("F(&x,y,z) ="));
}

static const TCHAR *fileDialogExtensions = _T("Expression-files (*.imp)\0*.imp\0All files (*.*)\0*.*\0\0");

void CIsoSurfaceDlg::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open implicit surface");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }

  try {
    IsoSurfaceParameters param;
    param.load(dlg.m_ofn.lpstrFile);
    param.setName(dlg.m_ofn.lpstrFile);
    paramToWin(param);
  } catch(Exception e) {
    showException(e);
  }
}

void CIsoSurfaceDlg::OnFileSave() {
  UpdateData();
  if(!validate()) {
    return;
  }
  
  IsoSurfaceParameters param;
  winToParam(param);

  if(param.hasDefaultName()) {
    saveAs(param);
  } else {
    save(param.getName(), param);
  }
}

void CIsoSurfaceDlg::OnFileSaveAs() {
  UpdateData();
  if(!validate()) {
    return;
  }
  
  IsoSurfaceParameters param;
  winToParam(param);
  saveAs(param);
}

void CIsoSurfaceDlg::saveAs(IsoSurfaceParameters &param) {
  CString objname = param.getName().cstr();
  CFileDialog dlg(FALSE,_T("*.imp"),  objname);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save Isosurface parameters");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile, param);
}

void CIsoSurfaceDlg::save(const String &fileName, IsoSurfaceParameters &param) {
  try {
    param.setName(fileName);
    param.save(fileName);
    paramToWin(param);
  } catch(Exception e) {
    showException(e);
  }
}

void CIsoSurfaceDlg::OnEditFindMatchingParentesis() {
  gotoMatchingParentesis();
}

void CIsoSurfaceDlg::OnCheckDoublesided() {
  enableCheckBox();
}

void CIsoSurfaceDlg::enableCheckBox() {
  GetDlgItem(IDC_CHECK_ORIGINOUTSIDE)->EnableWindow(!IsDlgButtonChecked(IDC_CHECK_DOUBLESIDED));
}

void CIsoSurfaceDlg::OnGotoExpr() {
  gotoExpr();
}

void CIsoSurfaceDlg::OnGotoSize() {
  gotoEditBox(this, IDC_EDIT_SIZE);
}

void CIsoSurfaceDlg::OnGotoTInterval() {
  gotoEditBox(this, IDC_EDIT_TIMEFROM);
}

void CIsoSurfaceDlg::OnGotoTimeCount() {
  gotoEditBox(this, IDC_EDIT_TIMECOUNT);
}

void CIsoSurfaceDlg::OnButtonHelp() {
  showExprHelpMenu();
}

void CIsoSurfaceDlg::OnExprHelp(UINT id) {
  handleSelectedExprHelpId(id);
}

void CIsoSurfaceDlg::paramToWin(const IsoSurfaceParameters &param) {
  const Point3D &lbn = param.m_boundingBox.m_lbn;
  const Point3D &rtf = param.m_boundingBox.m_rtf;
  m_name             = param.getName().cstr();
  m_expr             = param.m_expr.cstr();
  m_size             = param.m_size;
  m_xfrom            = lbn.x;
  m_yfrom            = lbn.y;
  m_zfrom            = lbn.z;
  m_xto              = rtf.x;
  m_yto              = rtf.y;
  m_zto              = rtf.z;
  m_tetrahedral      = param.m_tetrahedral      ? TRUE : FALSE;
  m_adaptiveCellSize = param.m_adaptiveCellSize ? TRUE : FALSE;
  m_doubleSided      = param.m_doubleSided      ? TRUE : FALSE;
  m_originOutside    = param.m_originOutside    ? TRUE : FALSE;
  m_machineCode      = param.m_machineCode      ? TRUE : FALSE;
  m_includeTime      = param.m_includeTime      ? TRUE : FALSE;
  m_timeCount        = param.m_timeCount;
  m_timeFrom         = param.getTInterval().getMin();
  m_timeTo           = param.getTInterval().getMax();

  SetWindowText(format(_T("Isosurface parameters (%s)"), param.getDisplayName().cstr()).cstr());
  UpdateData(false);
  enableCheckBox();
  enableTimeFields();
}

void CIsoSurfaceDlg::winToParam(IsoSurfaceParameters &param) {
  Point3D &lbn = param.m_boundingBox.m_lbn;
  Point3D &rtf = param.m_boundingBox.m_rtf;

  param.setName((LPCTSTR)m_name);
  param.m_expr             = m_expr;
  param.m_size             = m_size;
  lbn.x                    = m_xfrom;
  lbn.y                    = m_yfrom;
  lbn.z                    = m_zfrom;
  rtf.x                    = m_xto  ;
  rtf.y                    = m_yto  ;
  rtf.z                    = m_zto  ;
  param.m_tetrahedral      = m_tetrahedral      ? true : false;
  param.m_adaptiveCellSize = m_adaptiveCellSize ? true : false;
  param.m_doubleSided      = m_doubleSided      ? true : false;
  param.m_originOutside    = m_originOutside    ? true : false;
  param.m_machineCode      = m_machineCode      ? true : false;
  param.m_includeTime      = m_includeTime      ? true : false;
  param.m_timeCount        = m_timeCount;
  param.m_tInterval.setFrom(m_timeFrom);
  param.m_tInterval.setTo(  m_timeTo  );
}

