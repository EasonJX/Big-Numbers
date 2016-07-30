#pragma once

class CEnterOptionsNameDlg : public CDialog {
private:
  CComboBox *getNameCombo();
public:
    CEnterOptionsNameDlg(const String &currentName, CWnd *pParent = NULL);

    enum { IDD = IDD_DIALOGENTEROPTIONSNAME };
    CString m_name;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

