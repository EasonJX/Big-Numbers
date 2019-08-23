#include "stdafx.h"
#include "TestBin.h"
#include "TestFloatDlg.h"
#include "HexEditOverwrite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

CTestFloatDlg::CTestFloatDlg(CWnd *pParent)
: CDialog(CTestFloatDlg::IDD, pParent)
, m_streamOutString(_T(""))
, m_streamInString(_T(""))
, m_width(6)
, m_precision(2)
, m_fillString(_T(" "))
{
  m_showffActive     = false;
  m_setAccTypeActive = false;
}

void CTestFloatDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITSTREAMOUTSTRING, m_streamOutString      );
  DDX_Text(pDX, IDC_EDITSTREAMINSTRING , m_streamInString       );
  DDX_Text(pDX, IDC_EDITWIDTH          , m_width                );
  DDX_Text(pDX, IDC_EDITPREC           , m_precision            );
  DDX_Text(pDX, IDC_EDITFILL           , m_fillString           );
  DDV_MaxChars(pDX, m_fillString, 1);
}

BEGIN_MESSAGE_MAP(CTestFloatDlg, CDialog)
  ON_WM_CLOSE()
  ON_CBN_SELCHANGE(IDC_COMBOFLOATTYPE      , OnSelChangeComboFloatType   )
  ON_BN_CLICKED(   IDC_CHECKSIGNBIT        , OnClickedCheckSignBit       )
  ON_EN_CHANGE(    IDC_EDITEXPOFIELD       , OnChangeEditExpoField       )
  ON_EN_CHANGE(    IDC_EDITSIGNIFICAND     , OnChangeEditSigField        )
  ON_EN_SETFOCUS(  IDC_EDITEXPOFIELD       , OnEnSetFocusEditExpoField   )
  ON_EN_SETFOCUS(  IDC_EDITSIGNIFICAND     , OnEnSetFocusEditSignificand )
  ON_BN_CLICKED(   IDC_BUTTONTOFLOAT       , OnBnClickedButtonToFloat    )
  ON_BN_CLICKED(   IDC_BUTTONTODOUBLE      , OnBnClickedButtonToDouble   )
  ON_BN_CLICKED(   IDC_BUTTONTODOUBLE80    , OnBnClickedButtonToDouble80 )
  ON_BN_CLICKED(   IDC_BUTTONSAVE          , OnBnClickedButtonSave       )
  ON_BN_CLICKED(   IDC_BUTTONLOAD          , OnBnClickedButtonLoad       )
  ON_BN_CLICKED(   IDC_BUTTONRESETMEM      , OnBnClickedButtonResetMem   )
  ON_BN_CLICKED(   IDC_BUTTONSWAPACCMEM    , OnBnClickedButtonSwapAccMem )
  ON_BN_CLICKED(   IDC_BUTTONADD           , OnBnClickedButtonAdd        )
  ON_BN_CLICKED(   IDC_BUTTONSUB           , OnBnClickedButtonSub        )
  ON_BN_CLICKED(   IDC_BUTTONMULT          , OnBnClickedButtonMult       )
  ON_BN_CLICKED(   IDC_BUTTONDIV           , OnBnClickedButtonDiv        )
  ON_BN_CLICKED(   IDC_BUTTONFMOD          , OnBnClickedButtonFmod       )
  ON_BN_CLICKED(   IDC_BUTTONRECIPROC      , OnBnClickedButtonReciproc   )
  ON_BN_CLICKED(   IDC_BUTTONSQR           , OnBnClickedButtonSqr        )
  ON_BN_CLICKED(   IDC_BUTTONSQRT          , OnBnClickedButtonSqrt       )
  ON_BN_CLICKED(   IDC_BUTTONSETZERO       , OnBnClickedButtonSetZero    )
  ON_BN_CLICKED(   IDC_BUTTONSETTRUEMIN    , OnBnClickedButtonSetTrueMin )
  ON_BN_CLICKED(   IDC_BUTTONSETMIN        , OnBnClickedButtonSetMin     )
  ON_BN_CLICKED(   IDC_BUTTONSETEPS        , OnBnClickedButtonSetEps     )
  ON_BN_CLICKED(   IDC_BUTTONSETRND        , OnBnClickedButtonSetRnd     )
  ON_BN_CLICKED(   IDC_BUTTONSETONE        , OnBnClickedButtonSetOne     )
  ON_BN_CLICKED(   IDC_BUTTONSETMAX        , OnBnClickedButtonSetMax     )
  ON_BN_CLICKED(   IDC_BUTTONSETINF        , OnBnClickedButtonSetInf     )
  ON_BN_CLICKED(   IDC_BUTTONSETQNAN       , OnBnClickedButtonSetQNaN    )
  ON_BN_CLICKED(   IDC_BUTTONSETSNAN       , OnBnClickedButtonSetSNaN    )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINEXPO       , OnDeltaPosSpinExpo        )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINSIGNIFICAND, OnDeltaPosSpinSignificand )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINPRECISION  , OnDeltaPosSpinPrecision   )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINWIDTH      , OnDeltaPosSpinWidth       )
  ON_BN_CLICKED(   IDC_CHECK_MAXPRECISION  , OnBnClickedCheckMaxPrecision)
  ON_BN_CLICKED(   IDC_BUTTONSTREAMOUT     , OnBnClickedButtonStreamOut  )
  ON_BN_CLICKED(   IDC_BUTTONSTREAMIN      , OnBnClickedButtonStreamIn   )
  ON_BN_CLICKED(   IDC_BUTTONCOPYOUTTOIN   , OnBnClickedButtonCopyOutToIn)
  ON_BN_CLICKED(   IDC_CHECK_IOS_UNITBUF   , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_UPPERCASE , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_SHOWBASE  , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_SHOWPOINT , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_SHOWPOS   , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_LEFT      , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_RIGHT     , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_INTERNAL  , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_DEC       , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_OCT       , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_HEX       , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_SCIENTIFIC, OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_FIXED     , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_HEXFLOAT  , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_BOOLALPHA , OnBnClickedCheckIosFlag     )
  ON_BN_CLICKED(   IDC_CHECK_IOS_STDIO     , OnBnClickedCheckIosFlag     )
END_MESSAGE_MAP()

BOOL CTestFloatDlg::OnInitDialog() {
  __super::OnInitDialog();
  showFloatFieldsValue(m_accumulator, false);
  showFloatFieldsValue(m_memory     , true );

  CButton *oldButton = ((CButton*)GetDlgItem(IDC_BUTTONCOPYOUTTOIN));
  CPoint p = getWindowPosition(this, IDC_BUTTONCOPYOUTTOIN);
  oldButton->DestroyWindow();
  m_copyOutToInButton.Create(this, OBMIMAGE(DNARROW), p, IDC_BUTTONCOPYOUTTOIN);
  
  OnBnClickedCheckIosFlag();
  CheckDlgButton(IDC_RADIOOPACCUMULATOR, BST_CHECKED);
  GetDlgItem(IDC_EDITEXPOFIELD)->SetFocus();
  return FALSE;
}

void CTestFloatDlg::OnOK() {
}

void CTestFloatDlg::OnCancel() {
}

void CTestFloatDlg::OnClose() {
  EndDialog(IDOK);
}

BOOL CTestFloatDlg::PreTranslateMessage(MSG *pMsg) {
  switch(getFocusCtrlId(this)) {
  case IDC_EDITEXPOFIELD  :
    { const UINT maxValue = m_accumulator.getMaxExpoValue();
      if(isOverwriteCurrentHexChar(this, pMsg,0,maxValue)) {
        return true;
      }
    }
    break;
  case IDC_EDITSIGNIFICAND:
    { const UINT64 maxValue = m_accumulator.getMaxSigValue();
      if(isOverwriteCurrentHexChar(this, pMsg,0,maxValue)) {
        return true;
      }
    }
    break;
  }
  try {
    return __super::PreTranslateMessage(pMsg);
  } catch(Exception e) {
    showWarning(_T("%s"), e.what());
    return true;
  }
}

void CTestFloatDlg::OnSelChangeComboFloatType() {
  FloatType newType = getWinFloatType();
  if(newType != getAccFloatType()) {
    setAccFloatType(newType);
  }
}

void CTestFloatDlg::OnClickedCheckSignBit()       { updateAccumulator();                                     }
void CTestFloatDlg::OnChangeEditExpoField()       { updateAccumulator();                                     }
void CTestFloatDlg::OnChangeEditSigField()        { updateAccumulator();                                     }
void CTestFloatDlg::OnEnSetFocusEditExpoField()   { setCaretPos((CEdit*)GetDlgItem(IDC_EDITEXPOFIELD  ), 0); }
void CTestFloatDlg::OnEnSetFocusEditSignificand() { setCaretPos((CEdit*)GetDlgItem(IDC_EDITSIGNIFICAND), 0); }
void CTestFloatDlg::OnBnClickedButtonToFloat()    { setAccFloatType(FT_FLOAT   );                            }
void CTestFloatDlg::OnBnClickedButtonToDouble()   { setAccFloatType(FT_DOUBLE  );                            }
void CTestFloatDlg::OnBnClickedButtonToDouble80() { setAccFloatType(FT_DOUBLE80);                            }
void CTestFloatDlg::updateAccumulator()           { showFloatFieldsValue(editFieldToFloatFields()               , false); }
void CTestFloatDlg::OnBnClickedButtonSave()       { showFloatFieldsValue(m_accumulator                          , true ); }
void CTestFloatDlg::OnBnClickedButtonLoad()       { showFloatFieldsValue(m_memory                               , false); }
void CTestFloatDlg::OnBnClickedButtonAdd()        { showFloatFieldsValue(m_accumulator + m_memory               , false); }
void CTestFloatDlg::OnBnClickedButtonSub()        { showFloatFieldsValue(m_accumulator - m_memory               , false); }
void CTestFloatDlg::OnBnClickedButtonMult()       { showFloatFieldsValue(m_accumulator * m_memory               , false); }
void CTestFloatDlg::OnBnClickedButtonDiv()        { showFloatFieldsValue(m_accumulator / m_memory               , false); }
void CTestFloatDlg::OnBnClickedButtonFmod()       { showFloatFieldsValue(m_accumulator % m_memory               , false); }
void CTestFloatDlg::OnBnClickedButtonReciproc()   { showFloatFieldsValue(reciproc(m_accumulator)                , false); }
void CTestFloatDlg::OnBnClickedButtonSqr()        { showFloatFieldsValue(sqr( m_accumulator)                    , false); }
void CTestFloatDlg::OnBnClickedButtonSqrt()       { showFloatFieldsValue(sqrt(m_accumulator)                    , false); }
void CTestFloatDlg::OnBnClickedButtonResetMem()   { showFloatFieldsValue(FloatFields(m_memory     ).setZero()   , true ); }
void CTestFloatDlg::OnBnClickedButtonSetZero()    { showFloatFieldsValue(FloatFields(m_accumulator).setZero()   , false); }
void CTestFloatDlg::OnBnClickedButtonSetTrueMin() { showFloatFieldsValue(FloatFields(m_accumulator).setTrueMin(), false); }
void CTestFloatDlg::OnBnClickedButtonSetMin()     { showFloatFieldsValue(FloatFields(m_accumulator).setMin()    , false); }
void CTestFloatDlg::OnBnClickedButtonSetEps()     { showFloatFieldsValue(FloatFields(m_accumulator).setEps()    , false); }
void CTestFloatDlg::OnBnClickedButtonSetRnd()     { showFloatFieldsValue(FloatFields(m_accumulator).setRnd()    , false); }
void CTestFloatDlg::OnBnClickedButtonSetOne()     { showFloatFieldsValue(FloatFields(m_accumulator).setOne()    , false); }
void CTestFloatDlg::OnBnClickedButtonSetMax()     { showFloatFieldsValue(FloatFields(m_accumulator).setMax()    , false); }
void CTestFloatDlg::OnBnClickedButtonSetInf()     { showFloatFieldsValue(FloatFields(m_accumulator).setPInf()   , false); }
void CTestFloatDlg::OnBnClickedButtonSetQNaN()    { showFloatFieldsValue(FloatFields(m_accumulator).setQNaN()   , false); }
void CTestFloatDlg::OnBnClickedButtonSetSNaN()    { showFloatFieldsValue(FloatFields(m_accumulator).setSNaN()   , false); }

void CTestFloatDlg::OnBnClickedButtonSwapAccMem() {
  const FloatFields mem = m_memory, acc = m_accumulator;
  showFloatFieldsValue(mem, false);
  showFloatFieldsValue(acc, true );
}


void CTestFloatDlg::OnBnClickedCheckMaxPrecision() {
  updatePrecision();
}
void CTestFloatDlg::OnDeltaPosSpinExpo(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  const UINT oldValue  = getExpoField();
  const UINT maxValue  = m_accumulator.getMaxExpoValue();
  const int  add       = -pNMUpDown->iDelta;
  if(((add < 0) && (-add > oldValue)) || ((add > 0) && (oldValue + add > maxValue))) {
    return;
  }
  setExpoField(oldValue + add);
  *pResult = 0;
}
void CTestFloatDlg::OnDeltaPosSpinSignificand(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN   pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  const UINT64 oldValue  = getSigField();
  const UINT64 maxValue  = m_accumulator.getMaxSigValue();
  const int    add       = -pNMUpDown->iDelta;
  if(((add < 0) && (-add > oldValue)) || ((add > 0) && (oldValue + add < oldValue))
                                      || ((add > 0) && (oldValue + add > maxValue))
     ) {
    return;
  }
  setSigField(oldValue + add);
  *pResult = 0;
}
void CTestFloatDlg::OnDeltaPosSpinWidth(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UpdateData();
  const int newValue = max(m_width - pNMUpDown->iDelta, 0);
  m_width = newValue;
  UpdateData(false);
  *pResult = 0;
}
void CTestFloatDlg::OnDeltaPosSpinPrecision(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UpdateData();
  const int newValue = max(m_precision - pNMUpDown->iDelta, 0);
  m_precision = newValue;
  UpdateData(false);
  *pResult = 0;
}
void CTestFloatDlg::OnBnClickedButtonStreamOut() {
  UpdateData();
  m_streamOutString = streamOutSelectedOp();
  UpdateData(FALSE);
}

void CTestFloatDlg::OnBnClickedButtonStreamIn() {
  UpdateData();
  streamInSelectedOp(m_streamInString);
}

void CTestFloatDlg::OnBnClickedButtonCopyOutToIn() {
  UpdateData();
  m_streamInString = m_streamOutString;
  UpdateData(FALSE);
}

void CTestFloatDlg::OnBnClickedCheckIosFlag() {
  const FormatFlags flags = getStreamOutFormatFlags();
  setWindowText(this, IDC_STATICSTREAMFLAGSVALUE, format(_T("%08X"), flags));
  updatePrecision();
}

// -------------------------------------------------------------------------------------

void CTestFloatDlg::setAccFloatType(FloatType type) {
  if(m_setAccTypeActive || (type == getAccFloatType())) {
    return;
  }
  m_setAccTypeActive = true;
  FloatFields acc = m_accumulator, mem = m_memory;
  showFloatFieldsValue(acc.setType(type), false);
  showFloatFieldsValue(mem.setType(type), true);
  m_setAccTypeActive = false;
}

void CTestFloatDlg::setWinFloatType(FloatType type) {
  ((CComboBox*)GetDlgItem(IDC_COMBOFLOATTYPE))->SetCurSel(type);
  static int convertButtonId[] = { IDC_BUTTONTOFLOAT, IDC_BUTTONTODOUBLE, IDC_BUTTONTODOUBLE80 };
  for(size_t i = 0; i < ARRAYSIZE(convertButtonId); i++) {
    GetDlgItem(convertButtonId[i])->EnableWindow(TRUE);
  }
  GetDlgItem(convertButtonId[type])->EnableWindow(FALSE);
  updatePrecision();
}

void CTestFloatDlg::updatePrecision() {
  const bool maxIsChecked = IsDlgButtonChecked(IDC_CHECK_MAXPRECISION) == BST_CHECKED;
  if(maxIsChecked) {
    const int maxPrecision = ((getStreamOutFormatFlags() & ios::floatfield) == ios::hexfloat)
                           ? FloatFields::getSigHexDigitCount(getWinFloatType())
                           : FloatFields::getMaxDigits10(getWinFloatType());
    m_precision = maxPrecision;
    UpdateData(FALSE);
  }
  GetDlgItem(IDC_EDITPREC     )->EnableWindow(maxIsChecked ? FALSE : TRUE);
  GetDlgItem(IDC_SPINPRECISION)->EnableWindow(maxIsChecked ? FALSE : TRUE);
}

FloatType CTestFloatDlg::getWinFloatType() {
  return (FloatType)(((CComboBox*)GetDlgItem(IDC_COMBOFLOATTYPE))->GetCurSel());
}

void CTestFloatDlg::floatFieldsToEditFields(const FloatFields &ff) {
  setSignField(ff.getSignBit());
  setExpoField(ff.getExpoField(), ff.getType());
  setSigField( ff.getSig()      , ff.getType());
}

FloatFields    CTestFloatDlg::editFieldToFloatFields() {
  return FloatFields(getWinFloatType(), getSignField(), getExpoField(), getSigField());
}

void CTestFloatDlg::showFloatFieldsValue(const FloatFields &ff, bool mem) {
  if(m_showffActive) return;
  m_showffActive = true;
  const int         fpclass  = ff.getFpClass();
  String            valueStr = ff.toHexString();
  int valueWinId, classWinId;
  if(mem) {
    valueWinId = IDC_STATICMEMVALUE;
    classWinId = IDC_STATICMEMCLASSVALUE;
  } else {
    valueWinId = IDC_STATICACCVALUE;
    classWinId = IDC_STATICACCCLASSVALUE;
  }
  if(!mem) {
    m_accumulator = ff;
    setWinFloatType(ff.getType());
    floatFieldsToEditFields(m_accumulator);
    setWindowText(this, IDC_STATICACCBIN, ff.toBinString());
  } else {
    m_memory = ff;
  }
  setWindowText(this, valueWinId, valueStr);
  setWindowText(this, classWinId, fpclassToString(fpclass));
  if(mem && (ff.getType() != getWinFloatType())) {
    showWarning(_T("Memory-floatType=%s. .winFloatType=%s")
               ,toString(ff.getType()).cstr(), toString(getWinFloatType()).cstr());
  }
  m_showffActive = false;
}

// ----------------------------------------- Edit fields:sign, exponent and significand ----------------------------
void CTestFloatDlg::setSignField(bool v) {
  CheckDlgButton(IDC_CHECKSIGNBIT, v ? BST_CHECKED : BST_UNCHECKED);
}
bool CTestFloatDlg::getSignField() {
  return IsDlgButtonChecked(IDC_CHECKSIGNBIT) == BST_CHECKED;
}

void CTestFloatDlg::setExpoField(UINT v, FloatType type) {
  setWindowText(this, IDC_EDITEXPOFIELD, format(_T("%0*X"), FloatFields::getExpoHexDigitCount(type), v));
  expo2ToWin();
}
void CTestFloatDlg::setExpoField(UINT v) {
  setExpoField(v, getWinFloatType());
}
void CTestFloatDlg::expo2ToWin() {
  const FloatType type      = getWinFloatType();
  const int       expo2     = FloatFields::expoFieldToExpo2(type, getExpoField());
  const int       hexDigits = FloatFields::getExpoHexDigitCount(type);
  const UINT      expo2Hex  =  expo2 & FloatFields::getExpoMask(type);
  setWindowText(this, IDC_STATICEXPO2VALUE, format(_T("%+7d  %0*X"), expo2, hexDigits, expo2Hex));
}
UINT CTestFloatDlg::getExpoField() {
  UINT v;
  _stscanf(getWindowText(this, IDC_EDITEXPOFIELD).cstr(), _T("%X"), &v);
  return v;
}
void CTestFloatDlg::setSigField(UINT64 v, FloatType type) {
  setWindowText(this, IDC_EDITSIGNIFICAND, format(_T("%0*I64X"), FloatFields::getSigHexDigitCount(type), v));
}
void CTestFloatDlg::setSigField(UINT64 v) {
  setSigField(v, getWinFloatType());
}

UINT64 CTestFloatDlg::getSigField() {
  UINT64 v;
  _stscanf(getWindowText(this, IDC_EDITSIGNIFICAND).cstr(), _T("%I64X"), &v);
  return v;
}

bool CTestFloatDlg::useFloatManip() {
  return IsDlgButtonChecked(IDC_CHECKSTREAMINMANIP) == BST_CHECKED;
}

bool CTestFloatDlg::streamOperandIsMemory() {
  return IsDlgButtonChecked(IDC_RADIOOPMEMORY) == BST_CHECKED;
}

struct ButtunIdFlag {
  int         m_id;
  FormatFlags m_flag;
};

static const ButtunIdFlag buttonFlagArray[] = {
  IDC_CHECK_IOS_UNITBUF   , ios::unitbuf
 ,IDC_CHECK_IOS_UPPERCASE , ios::uppercase
 ,IDC_CHECK_IOS_SHOWBASE  , ios::showbase
 ,IDC_CHECK_IOS_SHOWPOINT , ios::showpoint
 ,IDC_CHECK_IOS_SHOWPOS   , ios::showpos
 ,IDC_CHECK_IOS_LEFT      , ios::left
 ,IDC_CHECK_IOS_RIGHT     , ios::right
 ,IDC_CHECK_IOS_INTERNAL  , ios::internal
 ,IDC_CHECK_IOS_DEC       , ios::dec
 ,IDC_CHECK_IOS_OCT       , ios::oct
 ,IDC_CHECK_IOS_HEX       , ios::hex
 ,IDC_CHECK_IOS_SCIENTIFIC, ios::scientific
 ,IDC_CHECK_IOS_FIXED     , ios::fixed
 ,IDC_CHECK_IOS_HEXFLOAT  , ios::hexfloat
 ,IDC_CHECK_IOS_BOOLALPHA , ios::boolalpha
 ,IDC_CHECK_IOS_STDIO     , ios::_Stdio
};

FormatFlags CTestFloatDlg::getStreamOutFormatFlags() {
  FormatFlags result = 0;
  for(size_t i = 0; i < ARRAYSIZE(buttonFlagArray); i++) {
    const ButtunIdFlag &idFlag = buttonFlagArray[i];
    if(IsDlgButtonChecked(idFlag.m_id) == BST_CHECKED) {
      result |= idFlag.m_flag;
    }
  }
  return result;
}

wchar_t CTestFloatDlg::getFillChar() {
  return (m_fillString.GetLength() == 0) ? _T(' ') : m_fillString.GetAt(0);
}

wostream &operator<<(wostream &out, const FloatFields &ff) {
  switch(ff.getType()) {
  case FT_FLOAT   : out << ff.getFloat();    break;
  case FT_DOUBLE  : out << ff.getDouble();   break;
  case FT_DOUBLE80: out << ff.getDouble80(); break;
  }
  return out;
}

CString CTestFloatDlg::streamOutSelectedOp() {
  wstringstream strStream;
  strStream.width(m_width);
  strStream.precision(m_precision);
  strStream.flags(getStreamOutFormatFlags());
  strStream.fill(getFillChar());
  strStream << getSelectedOperand();
  return strStream.str().c_str();
}

void CTestFloatDlg::streamInSelectedOp(const CString &str) {
  float         f;
  double        d;
  Double80      d80;
  wstringstream strStream((LPCTSTR)str);
  if(useFloatManip()) {
    switch(getWinFloatType()) {
    case FT_FLOAT   : strStream >> WcharManip<float >   >> f  ; break;
    case FT_DOUBLE  : strStream >> WcharManip<double>   >> d  ; break;
    case FT_DOUBLE80: strStream >> WcharManip<Double80> >> d80; break;
    }
  } else {
    switch(getWinFloatType()) {
    case FT_FLOAT   : strStream >> f  ; break;
    case FT_DOUBLE  : strStream >> d  ; break;
    case FT_DOUBLE80: strStream >> d80; break;
    }
  }
  String result;
  if(strStream       ) {
    result =  _T("ok "  );
    FloatFields ff = m_accumulator;
    switch(getWinFloatType()) {
    case FT_FLOAT   : ff = f  ; break;
    case FT_DOUBLE  : ff = d  ; break;
    case FT_DOUBLE80: ff = d80; break;
    }
    showFloatFieldsValue(ff, streamOperandIsMemory());
  }
  if(strStream.fail()) result += _T("fail ");
  if(strStream.bad() ) result += _T("bad " );
  if(strStream.eof()) {
    result += _T("eof ");
  } else {
    wchar_t next = strStream.peek();
    result += format(_T("next:'%c'"), next);
  }
  setWindowText(this, IDC_STATICSTREAMINSTATUS, result);
}
