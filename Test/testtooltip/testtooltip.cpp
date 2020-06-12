#include "stdafx.h"
#include "testtooltipDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTesttooltipApp, CWinApp)
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTesttooltipApp::CTesttooltipApp() {
}

CTesttooltipApp theApp;

BOOL CTesttooltipApp::InitInstance() {
  AfxEnableControlContainer();

  CTesttooltipDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }
  return FALSE;
}
