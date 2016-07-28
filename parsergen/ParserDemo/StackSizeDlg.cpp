#include "stdafx.h"
#include "ParserDemo.h"
#include "StackSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

StackSizeDlg::StackSizeDlg(int oldSize, CWnd *pParent) : CDialog(StackSizeDlg::IDD, pParent) {
    m_stackSize = oldSize;
}

void StackSizeDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITSTACKSIZE, m_stackSize);
    DDV_MinMaxUInt(pDX, m_stackSize, 256, 10000);
}

BEGIN_MESSAGE_MAP(StackSizeDlg, CDialog)
END_MESSAGE_MAP()

void StackSizeDlg::OnOK() {
  UpdateData();

  CDialog::OnOK();
}

BOOL StackSizeDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  gotoEditBox(this, IDC_EDITSTACKSIZE);
  return false;
}
