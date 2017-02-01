#include "stdafx.h"
#include "PartyMaker.h"
#include "PasswordDlg.h"
#include <md5.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PasswordDlg::PasswordDlg(CWnd *pParent) : CDialog(PasswordDlg::IDD, pParent) {
    m_password = _T("");
    m_tryCount = 0;
}

void PasswordDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_PASSWORD, m_password);
}

BEGIN_MESSAGE_MAP(PasswordDlg, CDialog)
END_MESSAGE_MAP()

BOOL PasswordDlg::OnInitDialog() {
  GetDlgItem(IDC_PASSWORD)->SetFocus();
  CDialog::OnInitDialog();
  return false;
}

void PasswordDlg::OnOK() {
  UpdateData();
  if(Options::validatePassword((LPCTSTR)m_password)) {
    CDialog::OnOK();
  } else {
    MessageBox(_T("Forkert password"),_T("Fejl"), MB_ICONWARNING);
    m_tryCount++;
    if(m_tryCount == 3) {
      OnCancel();
    }
  }
}
