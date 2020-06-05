#pragma once

#include "SaveLoadExprWithAnimationDialog.h"
#include <D3DGraphics/ExprFunctionR2R1SurfaceParameters.h>

class CExprFunctionR2R1SurfaceParametersDlg : public SaveLoadExprWithAnimationDialog<ExprFunctionR2R1SurfaceParameters> {
private:
  CString m_expr;
  double  m_xfrom;
  double  m_xto;
  double  m_yfrom;
  double  m_yto;
  int     m_pointCount;
  BOOL    m_doubleSided;
  BOOL    m_calculateNormals;
  BOOL    m_hasTexture;
  CString m_textureFileName;

  bool   validate();
  void   paramToWin(const ExprFunctionR2R1SurfaceParameters &param);
  bool   winToParam(      ExprFunctionR2R1SurfaceParameters &param);
  void   enableTextureFields(bool enable);
  void   enableTimeFields();
public:
  CExprFunctionR2R1SurfaceParametersDlg(const ExprFunctionR2R1SurfaceParameters &param, CWnd *pParent = NULL);

  enum { IDD = IDR_FUNCTIONR2R1SURFACE };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  afx_msg BOOL OnInitDialog();
  afx_msg void OnEditFindMatchingParentesis();
  afx_msg void OnGotoExpr();
  afx_msg void OnGotoXInterval();
  afx_msg void OnGotoYInterval();
  afx_msg void OnGotoPointCount();
  afx_msg void OnButtonHelp();
  afx_msg void OnBnClickedCheckHasTexture();
  afx_msg void OnBnClickedButtonBrowseTexture();
  DECLARE_MESSAGE_MAP()
};
