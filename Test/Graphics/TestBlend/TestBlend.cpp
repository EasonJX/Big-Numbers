#include "stdafx.h"
#include "TestBlend.h"
#include "TestBlendDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestBlendApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestBlendApp::CTestBlendApp() {
}

CTestBlendApp theApp;

BOOL CTestBlendApp::InitInstance() {
    AfxEnableControlContainer();

    CTestBlendDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }
    return FALSE;
}
