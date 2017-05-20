#include "stdafx.h"
#include "testbin.h"
#include "TimerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTimerDlg::CTimerDlg(UINT timerInterval, CWnd *pParent) : CDialog(CTimerDlg::IDD, pParent) {
  m_timerInterval = timerInterval;
}

void CTimerDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITTIMERINTERVAL, m_timerInterval);
  DDV_MinMaxUInt(pDX, m_timerInterval, 1, 10000);
}


BEGIN_MESSAGE_MAP(CTimerDlg, CDialog)
END_MESSAGE_MAP()

void CTimerDlg::OnOK() {
  UpdateData();
  CDialog::OnOK();
}

BOOL CTimerDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  UpdateData(false);
  gotoEditBox(this, IDC_EDITTIMERINTERVAL);
  return FALSE;
}
