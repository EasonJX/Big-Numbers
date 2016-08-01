#pragma once

class CEnterAddressDlg : public CDialog, SettingsAccessor {
private:
  HACCEL                 m_accelTable;
  const unsigned __int64 m_docSize;
  int                    m_addrRadix;
  bool                   m_addrUppercase;
  unsigned __int64       m_addr;
  bool                   m_changeEditAddressActive;

  bool validateAddress();
  void errorMessage(const TCHAR *format, ...);
  CEdit *getAddressField();
public:
	CEnterAddressDlg(unsigned __int64 docSize, CWnd* pParent = NULL);
    unsigned __int64 getAddress();
	enum { IDD = IDD_DIALOGENTERADDR };
	CString	m_addressText;

	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual void OnOK();
	afx_msg void OnChangeEditAddress();
	virtual BOOL OnInitDialog();
    afx_msg void OnGotoEditAddress();
	DECLARE_MESSAGE_MAP()
};

