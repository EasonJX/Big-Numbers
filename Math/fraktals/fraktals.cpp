#include "stdafx.h"
#include "fraktalsDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CFraktalsApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CFraktalsApp::CFraktalsApp() {
}

CFraktalsApp theApp;

BOOL CFraktalsApp::InitInstance() {
  AfxEnableControlContainer();

  CFraktalsDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK) {
  } else if (nResponse == IDCANCEL) {
  }

  return FALSE;
}
