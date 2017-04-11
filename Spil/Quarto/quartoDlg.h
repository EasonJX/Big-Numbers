#pragma once

#include <D3DGraphics/D3Scene.h>
#include "GraphicObjects.h"
#include <D3DGraphics/D3SceneEditor.h>

#define DEVELOPER_MODE

class CQuartoDlg : public CDialog, public D3SceneContainer {
private:
  HICON                   m_hIcon;
  HACCEL                  m_accelTable;
  D3SceneEditor           m_editor;
  D3Scene                 m_scene;
  String                  m_gameName;
  GameBoardObject        *m_boardObject;
  Game                    m_game;
  Player                  m_startPlayer;

  void createScene();
  void createLight();
  void createBoard();
  void resetCamera();
  void resetScene();
  void showEditorInfo();
  inline void setEditMode(bool value) {
    m_editor.setEnabled(value);
    showInfo(EMPTYSTRING);
  }
  inline CRect getGameRect() {
    return getClientRect(this, IDC_STATICGAMEWINDOW);
  }
  CPoint   get3DPanelPoint(CPoint point, bool screenRelative) const;
  void     setCameraPosition(const D3DXVECTOR3 &pos);
  inline const D3DXVECTOR3 getFieldCenter(const Field &f) const {
    return m_boardObject->getFieldCenter(f);
  }
  void resetBrickPositions(bool colored);
  void flashWinnerBlocks();
  void updateGraphicsDoingMove(const Move &m);

  void setGameName(const String &name);
  const String &getGameName() const {
    return m_gameName;
  }
  void save(const String &name);
  bool isGameTitled() const {
    return !m_gameName.equalsIgnoreCase(_T("Untitled"));
  }
  void        newGame(bool colored, Player startPlayer, const String &name = _T("Untitled"));
  void        executeMove(const Move &m);

  Move        findMove();
  void        endGame();
  int         getBrickFromPoint(const CPoint &p) const;
  Field       getFieldFromPoint(const CPoint &p) const;
  void        markBrick(  int b);
  void        unmarkCurrentBrick();
  void        selectField(const Field &f);
  void        selectBrick(int b);
  inline int  getSelectedBrick() const {
    return m_boardObject->getCurrentBrick();
  }
  inline Field getSelectedField() const {
    return m_boardObject->getCurrentField();
  }
  void        showInfo(const TCHAR *format,...);
  void        turnBoard(int degree);
public:
  CQuartoDlg(CWnd *pParent = NULL);
  
  D3Scene &getScene() {
    return m_scene;
  }
  CWnd    *getMessageWindow() {
    return this;
  }
  CWnd    *get3DWindow() {
    return GetDlgItem(IDC_STATICGAMEWINDOW);
  }
  void render(BYTE renderFlags) {
    PostMessage(ID_MSG_RENDER, renderFlags, 0);
  }
  enum { IDD = IDD_DIALOGQUARTO };

  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg LRESULT OnMsgRender( WPARAM wp , LPARAM lp    );
  afx_msg void    OnSysCommand(UINT   nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg void OnClose();
  afx_msg void OnLButtonDown(  UINT nFlags, CPoint point);
  afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnFileNew();
  afx_msg void OnFileOpen();
  afx_msg void OnFileSave();
  afx_msg void OnFileSaveAs();
  afx_msg void OnFileExit();
  afx_msg void OnViewLeft();
  afx_msg void OnViewRight();
  afx_msg void OnViewResetView();
  afx_msg void OnOptionsLevelBeginner();
  afx_msg void OnOptionsLevelExpert();
  afx_msg void OnOptionsColoredGame();
  afx_msg void OnHelpAboutquarto();
  afx_msg void OnDumpSetup();
  DECLARE_MESSAGE_MAP()
};
