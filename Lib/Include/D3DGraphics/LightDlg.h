#pragma once

#include <MFCUtil/ColormapDialog.h>
#include <D3DGraphics/resource.h>
#include "D3Scene.h"

class CLightDlg : public CColormapDialog<D3Light> {
private:
    String m_origName;

    void  resetControls();
    void  valueToWindow(   const D3Light &v);
    void  ajourSliders(    const D3Light &v);
    void  enableSliders(   const D3Light &v);
    void  showSliderValues(const D3Light &v);
    void  setCurrentValue( const D3Light &v);
public:
    CLightDlg(PropertyChangeListener *listener, CWnd *pParent = NULL);
    enum { IDD = IDD_LIGHT_DIALOG };
    static D3Light &copyModifiableValues(D3Light &dst, const D3Light &src);
    String getTypeName() const {
      return _T("D3Light");
    }
protected:
    virtual void    DoDataExchange(CDataExchange *pDX);
    virtual BOOL    OnInitDialog();
    afx_msg void    OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg void    OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void    OnHideWindow();
    virtual void    OnCancel();
    afx_msg void    OnClose();
    afx_msg LRESULT OnMsgResetControls(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()
private:
  CColormap m_colormapAmbient;
  CColormap m_colormapSpecular;
  CColormap m_colormapDiffuse;
  DECLARE_EVENTSINK_MAP()
  void OnColorchangedColormapAmbient();
  void OnColorchangedColormapDiffuse();
  void OnColorchangedColormapSpecular();
};
