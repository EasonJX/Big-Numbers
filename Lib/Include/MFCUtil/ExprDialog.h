#pragma once

#include <HashMap.h>
#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/OBMButton.h>

#define MAXHELPBUTTONS 10

class CExprDialog : public CDialog {
private:
  int                    m_helpButtonCount;
  OBMButton              m_helpButton[MAXHELPBUTTONS];
  CFont                  m_exprFont;
  // if >= 0, fieldId of CEdit that will be prefixed to all expressions in dialog
  int                    m_commonExprId;
  IntHashMap<int>        m_helpButtonMap;
  int                    m_selectedExprId;
  void createExprFont();
  void setExprFont(int id);
  void createMenuExprHelp(CMenu &menu);
  void showExprError(const String &msg, int id);
  void showExprHelpMenu(int id);
  void handleSelectedExprHelpId(int menuId, int ctrlId);
  String getExprSyntax(int index);
  void substituteSelectedText(int ctrlId, const String &s);

protected:
  SimpleLayoutManager m_layoutManager;

  CExprDialog(int resId, CWnd *pParent) : CDialog(resId, pParent) {
    m_helpButtonCount = 0;
    m_commonExprId    = -1;
  }
  void createExprHelpButton(int buttonId, int exprEditId);
  void handleExprHelpButtonClick(int buttonId);
  void gotoExpr(int id) {
    GetDlgItem(id)->SetFocus();
  }

  inline void   setCommonExprFieldId(int id) {
    m_commonExprId = id;
    if(id >= 0) setExprFont(id);

  }
  inline int    getCommonExprFieldId() const {
    return m_commonExprId;
  }
  inline bool   hasCommonExprField() const {
    return m_commonExprId >= 0;
  }
  inline String getCommonExprString() const {
    return hasCommonExprField() ? getWindowText(this, getCommonExprFieldId()) : EMPTYSTRING;
  }
  inline CEdit *getExprField(int id) {
    return (CEdit*)GetDlgItem(id);
  }
  inline String getExprString(int id) const {
    return getCommonExprString() + getWindowText(this, id);
  }
  inline void gotoMatchingParentesis() {
    ::gotoMatchingParanthes(this, getFocusCtrlId(this));
  }

  bool validateAllExpr();
  bool validateExpr(int id);
  bool validateInterval(int fromId, int toId);
  bool validateMinMax(int id, double min, double max);
  virtual bool validate() = 0;
  virtual void addToRecent(const String &fileName) {
  }

  void OnSize(UINT nType, int cx, int cy) {
    __super::OnSize(nType, cx, cy);
    m_layoutManager.OnSize(nType, cx, cy);
  }

  BOOL PreTranslateMessage(MSG *pMsg);
  virtual void OnFileNew() {
  }
  virtual void OnFileOpen() {
  }
  virtual void OnFileSave() {
  }
  virtual void OnFileSaveAs() {
  }
};

template <class T> class SaveLoadExprDialog : public CExprDialog {
private:
  const String m_paramTypeName;
  const String m_extension;
  const int    m_resId;
  HACCEL       m_accelTable;
  T            m_param;
  CString      m_name;

  String getFileDialogFilter() const {
    return format(_T("%s-files (*.%s)%c*.%s%cAll files (*.*)%c*.*%c%c")
                 ,m_paramTypeName.cstr()
                 ,m_extension.cstr(),0
                 ,m_extension.cstr(),0
                 ,0,0,0
                 );
  }

  inline bool updateAndValidate() {
    return UpdateData() && validate();
  }

  inline bool nameEntered(CFileDialog &dlg) {
    return (dlg.DoModal() == IDOK) && (_tcslen(dlg.m_ofn.lpstrFile) > 0);
  }


protected:
  virtual void paramToWin(const T &param) {
    setWindowText(this, format(_T("%s (%s)")
                      , firstLetterToUpperCase(m_paramTypeName).cstr()
                      , param.getDisplayName().cstr()));
    m_name = param.getName().cstr();
  }
  virtual void winToParam(T &param) const {
    param.setName((LPCTSTR)m_name);
  };

  SaveLoadExprDialog(int resId, CWnd *pParent, const T &param, const String &paramTypeName, const String &extension)
   : CExprDialog(    resId, pParent)
   , m_resId(        resId         )
   , m_param(        param         )
   , m_paramTypeName(paramTypeName )
   , m_extension(    extension     )
  {
  }

  virtual BOOL OnInitDialog() {
    __super::OnInitDialog();
    m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(m_resId));
    paramToWin(m_param);
    UpdateData(FALSE);
    return TRUE;
  }

  BOOL PreTranslateMessage(MSG *pMsg) {
    if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
      return true;
    }
    return __super::PreTranslateMessage(pMsg);
  }

  void OnOK() {
    if(!updateAndValidate()) return;
    winToParam(m_param);
    __super::OnOK();
  }

  void OnFileNew() {
    T param;
    paramToWin(param);
    UpdateData(false);
  }

  void OnFileOpen() {
    CFileDialog dlg(TRUE);
    const String dlgTitle  = format(_T("Open %s"), m_paramTypeName.cstr());
    const String dlgFilter = getFileDialogFilter();
    dlg.m_ofn.lpstrFilter = dlgFilter.cstr();
    dlg.m_ofn.lpstrTitle  = dlgTitle.cstr();
    if(!nameEntered(dlg)) {
      return;
    }
    try {
      T param;
      const String fileName = dlg.m_ofn.lpstrFile;
      param.load(fileName);
      paramToWin(param);
      addToRecent(fileName);
      UpdateData(false);
    } catch(Exception e) {
      showException(e);
    }
  }

  void OnFileSave() {
    if(!updateAndValidate()) return;
    T param;
    winToParam(param);
    if(param.hasDefaultName()) {
      saveAs(param);
    } else {
      save(param.getName(), param);
    }
  }

  void OnFileSaveAs() {
    if(!updateAndValidate()) return;
    T param;
    winToParam(param);
    saveAs(param);
  }

  void saveAs(T &param) {
    const String objname          = param.getName();
    const String dlgTitle         = format(_T("Save %s"), m_paramTypeName.cstr());
    const String defaultExtension = format(_T("*.%s"), m_extension.cstr());
    const String dlgFilter        = getFileDialogFilter();
    CFileDialog dlg(FALSE, defaultExtension.cstr(), objname.cstr());
    dlg.m_ofn.lpstrFilter = dlgFilter.cstr();
    dlg.m_ofn.lpstrTitle  = dlgTitle.cstr();
    if(!nameEntered(dlg)) {
      return;
    }
    save(dlg.m_ofn.lpstrFile, param);
  }

  void save(const String &fileName, T &param) {
    try {
      param.save(fileName);
      paramToWin(param);
      UpdateData(FALSE);
      addToRecent(fileName);
    } catch(Exception e) {
      showException(e);
    }
  }
public:
  const T &getData() const {
    return m_param;
  }
};
