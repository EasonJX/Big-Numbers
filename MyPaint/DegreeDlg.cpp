#include "stdafx.h"
#include "DegreeDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CDegreeDlg::CDegreeDlg(double degree, CWnd *pParent) : CDialog(CDegreeDlg::IDD, pParent) {
  m_degree = degree;
}

void CDegreeDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITDEGREE, m_degree);
  DDV_MinMaxDouble(pDX, m_degree, -180., 180.);
}

BEGIN_MESSAGE_MAP(CDegreeDlg, CDialog)
END_MESSAGE_MAP()

void CDegreeDlg::OnOK() {
  UpdateData();
  __super::OnOK();
}

BOOL CDegreeDlg::OnInitDialog() {
  __super::OnInitDialog();

  gotoEditBox(this, IDC_EDITDEGREE);
  return FALSE;
}
