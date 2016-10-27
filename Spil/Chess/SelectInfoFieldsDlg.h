#pragma once


class CSelectInfoFieldsDlg : public CDialog, public OptionsAccessor {
	DECLARE_DYNAMIC(CSelectInfoFieldsDlg)

private:
  void windowToData(      EngineVerboseFields &evf);
  void dataToWindow(const EngineVerboseFields &evf);
public:
	CSelectInfoFieldsDlg(CWnd* pParent = NULL);
	virtual ~CSelectInfoFieldsDlg();

	enum { IDD = IDD_SELECTINFOFIELDS_DIALOG };
  BOOL m_depth   ;
  BOOL m_seldepth;
  BOOL m_score   ;
  BOOL m_time    ;
  BOOL m_nodes   ;
  BOOL m_nodesps ;
  BOOL m_pv      ;
  BOOL m_string  ;
  BOOL m_hashfull;
  BOOL m_multipv ;
  BOOL m_cpuLoad ;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
  virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
