#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CTestBlendApp : public CWinApp {
public:
    CTestBlendApp();
    PixRectDevice m_device;

    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CTestBlendApp theApp;
