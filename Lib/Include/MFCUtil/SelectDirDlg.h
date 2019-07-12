#pragma once

#include "resource.h"
#include "WinTools.h"

class CSelectDirDlg : public CDialog {
private:
  HACCEL     m_accelTable;
  CImageList m_images;
  void       fillTree(const TCHAR *path);
  CString    getSelectedPath() const;
  CTreeCtrl *getTreeCtrl() const;
public:
  CSelectDirDlg(const String &startDir, CWnd *pParent = NULL);
  String getSelectedDir() const {
    return (LPCTSTR)m_dir;
  }
    enum { IDD = _IDD_SELECTDIR_DIALOG };
    CString	m_dir;
    CString	m_drive;
public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnEditChangeDriveCombo();
    afx_msg void OnGotoDir();
    afx_msg void OnGotoDrive();
    afx_msg void OnSelChangeDriveCombo();
    afx_msg void OnDblclkDirTree(NMHDR *pNMHDR, LRESULT *pResult);
  DECLARE_MESSAGE_MAP()
};

StringArray selectMultipleFileNames(const String      &title
                                   ,const TCHAR       *extensions
                                   ,UINT               defaultExtensionIndex  = 0
                                   ,const String      &initialDir             = EMPTYSTRING
                                   ,DWORD              flags                  = OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING
                                   ,UINT              *selectedExtensionIndex = NULL
                                   );
