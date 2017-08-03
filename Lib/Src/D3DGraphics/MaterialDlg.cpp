#include "pch.h"
#include <MFCUtil/resource.h>
#include <D3DGraphics/MaterialDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMaterialDlg::CMaterialDlg(PropertyChangeListener *listener, CWnd *pParent)
: CColormapDialog<MATERIAL>(CMaterialDlg::IDD, SP_MATERIALPARAMETERS, pParent)
{
  if(listener) {
    addPropertyChangeListener(listener);
  }
}

void CMaterialDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COLORMAP_AMBIENT, m_colormapAmbient);
  DDX_Control(pDX, IDC_COLORMAP_DIFFUSE, m_colormapDiffuse);
  DDX_Control(pDX, IDC_COLORMAP_EMISSIVE, m_colormapEmissive);
  DDX_Control(pDX, IDC_COLORMAP_SPECULAR, m_colormapSpecular);
}

BEGIN_MESSAGE_MAP(CMaterialDlg, CDialog)
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SHOWWINDOW()
    ON_MESSAGE(_ID_MSG_RESETCONTROLS  , OnMsgResetControls)
    ON_BN_CLICKED(ID_BUTTON_HIDEWINDOW, OnHideWindow      )
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CMaterialDlg::OnInitDialog() {
  __super::OnInitDialog();
  m_origName = getWindowText(this);
  initSlider(IDC_SLIDER_POWER       , 0.1, 400, 200, LOGARITHMIC);
  initSlider(IDC_SLIDER_TRANSPARENCY, 1  ,   0, 200);
  return TRUE;
}

LRESULT CMaterialDlg::OnMsgResetControls(WPARAM wp, LPARAM lp) {
  resetControls();
  return 0;
}

void CMaterialDlg::resetControls() {
  setNotifyEnabled(false);

  const MATERIAL &v = getStartValue();
  setCurrentValue(v);
  valueToWindow(v);

  setNotifyEnabled(true);
}

void CMaterialDlg::valueToWindow(const MATERIAL &v) {
  setWindowText(this, format(_T("%s (Material %d)"), m_origName.cstr(), v.m_index));

  setSliderPower(v.Power);
  setSliderTransparency(v.Diffuse.a);
  setD3DCOLORVALUE(IDC_COLORMAP_AMBIENT , v.Ambient );
  setD3DCOLORVALUE(IDC_COLORMAP_DIFFUSE , v.Diffuse );
  setD3DCOLORVALUE(IDC_COLORMAP_SPECULAR, v.Specular);
  setD3DCOLORVALUE(IDC_COLORMAP_EMISSIVE, v.Emissive);
}

void CMaterialDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  MATERIAL v = getCurrentValue();
  v.Power    = getSliderPower();
  setCurrentValue(v);
  showPower(v.Power);
  __super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMaterialDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  MATERIAL v  = getCurrentValue();
  v.Diffuse.a = getSliderTransparency();
  setCurrentValue(v);
  showTransparency(v.Diffuse.a);
  __super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMaterialDlg::setSliderPower(double v) {
  setSliderValue(IDC_SLIDER_POWER, v);
  showPower(v);
}

float CMaterialDlg::getSliderPower() const {
  return getSliderValue(IDC_SLIDER_POWER);
}

void CMaterialDlg::showPower(double v) {
  setWindowText(this, IDC_STATIC_POWER, format(_T("%.2f"), v));
}

void CMaterialDlg::setSliderTransparency(double v) {
  setSliderValue(IDC_SLIDER_TRANSPARENCY, v);
  showTransparency(v);
}

float CMaterialDlg::getSliderTransparency() const {
  return getSliderValue(IDC_SLIDER_TRANSPARENCY);
}

void  CMaterialDlg::showTransparency(double v) {
  setWindowText(this, IDC_STATIC_TRANSPARENCY, format(_T("%.1f%%"), v*100.0));
}

void CMaterialDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
  PropertyDialog::OnShowWindow(bShow, nStatus);
}

void CMaterialDlg::OnHideWindow() {
  ShowWindow(SW_HIDE);
}

void CMaterialDlg::OnClose() {
  OnHideWindow();
}

void CMaterialDlg::OnCancel() {
  const MATERIAL &v = getStartValue();
  setCurrentValue(v);
  valueToWindow(v);
}

BEGIN_EVENTSINK_MAP(CMaterialDlg, CColormapDialog)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_AMBIENT , 1, CMaterialDlg::OnColorchangedColormapAmbient , VTS_NONE)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_DIFFUSE , 1, CMaterialDlg::OnColorchangedColormapDiffuse , VTS_NONE)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_SPECULAR, 1, CMaterialDlg::OnColorchangedColormapSpecular, VTS_NONE)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_EMISSIVE, 1, CMaterialDlg::OnColorchangedColormapEmissive, VTS_NONE)
END_EVENTSINK_MAP()


void CMaterialDlg::OnColorchangedColormapAmbient(){
  MATERIAL v = getCurrentValue();
  v.Ambient = getD3DCOLORVALUE(IDC_COLORMAP_AMBIENT);
  setCurrentValue(v);
}

void CMaterialDlg::OnColorchangedColormapDiffuse() {
  MATERIAL v  = getCurrentValue();
  v.Diffuse   = getD3DCOLORVALUE(IDC_COLORMAP_DIFFUSE);
  v.Diffuse.a = getSliderTransparency();
  setCurrentValue(v);
}

void CMaterialDlg::OnColorchangedColormapSpecular() {
  MATERIAL v = getCurrentValue();
  v.Specular = getD3DCOLORVALUE(IDC_COLORMAP_SPECULAR);
  setCurrentValue(v);
}

void CMaterialDlg::OnColorchangedColormapEmissive() {
  MATERIAL v = getCurrentValue();
  v.Emissive = getD3DCOLORVALUE(IDC_COLORMAP_EMISSIVE);
  setCurrentValue(v);
}
