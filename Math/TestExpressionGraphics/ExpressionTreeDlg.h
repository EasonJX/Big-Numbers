#pragma once

#include <PropertyContainer.h>

typedef enum {
  TREE_SELECTEDNODE            // const ExpressionNode *
} ExpressionTreeDlgProperties;

class CExpressionTreeDlg : public CDialog, public PropertyContainer {
private:
  SimpleLayoutManager   m_layoutManager;
  const Expression     *m_expr;
  const ExpressionNode *m_node;
  ExpressionNode       *m_selectedNode;
  BOOL                  m_extendedInfo;

  void buildTree();
  void traverse( CTreeCtrl *ctrl, HTREEITEM p, const ExpressionNode *n);
  void expandAllNodes();
  HTREEITEM findItemFromNode(const ExpressionNode *n);
  const ExpressionNode *getNodeFromPoint(CPoint p);
  CTreeCtrl *getTreeCtrl();
  String getString(        const ExpressionNode *n) const;

  void setSelectedNode(const ExpressionNode *m_selectedNode); // property
  void updateNodeText(const ExpressionNode *n);
public:
    CExpressionTreeDlg(const Expression     &expr, CWnd *pParent = NULL);
    CExpressionTreeDlg(const ExpressionNode *n   , CWnd *pParent = NULL);

    enum { IDD = IDD_EXPRESSIONTREE_DIALOG };

    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnClose();
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
    afx_msg void OnClearBreakPoint();
    afx_msg void OnSetBreakPoint();
    afx_msg void OnSelchangedTreeExpression(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedCheckExtendedInfo();
    DECLARE_MESSAGE_MAP()
};

