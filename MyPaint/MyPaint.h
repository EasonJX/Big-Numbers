#pragma once

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#define APSTUDIO_INVOKED

#include "MainFrm.h"

class CMyPaintApp : public CWinApp {
  CMainFrame *getMainFrame() {
    return (CMainFrame*)GetMainWnd();
  }

public:
  PixRectDevice m_device;
  CMyPaintApp();
  String getRecentFile(int index);
  void   removeRecentFile(int index);
  PixRect *fetchPixRect(const CSize &size);

  virtual BOOL InitInstance();
  afx_msg void OnAppAbout();
  DECLARE_MESSAGE_MAP()
};

extern CMyPaintApp theApp;

