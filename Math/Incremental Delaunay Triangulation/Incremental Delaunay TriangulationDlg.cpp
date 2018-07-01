#include "stdafx.h"
#include <MFCUtil/Point2DP.h>
#include <MFCUtil/ColorSpace.h>
#include "Incremental Delaunay Triangulation.h"
#include "Incremental Delaunay TriangulationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx {
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CIncrementalDelaunayTriangulationDlg::CIncrementalDelaunayTriangulationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INCREMENTALDELAUNAYTRIANGULATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_count = 0;
  m_edges = NULL;
}

void CIncrementalDelaunayTriangulationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIncrementalDelaunayTriangulationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CIncrementalDelaunayTriangulationDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr) 	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIncrementalDelaunayTriangulationDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}	else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIncrementalDelaunayTriangulationDlg::OnPaint() {
	if (IsIconic()) {
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}
}

HCURSOR CIncrementalDelaunayTriangulationDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

void CIncrementalDelaunayTriangulationDlg::paintEdges() {
  CClientDC dc(this);
  const CRect r = getClientRect(this);
  dc.FillSolidRect(&r, WHITE);
  dc.SetDCPenColor(BLACK);
  m_edges->paint(dc);
}

void CIncrementalDelaunayTriangulationDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  if(m_edges) {
    m_edges->InsertSite(Point2DP(point));
    paintEdges();
  } else if(m_count < 3) {
    m_p[m_count++] = point;
    CClientDC dc(this);
    switch(m_count) {
    case 1:
      { const CRect r = getClientRect(this);
        dc.FillSolidRect(&r, WHITE);
        dc.FillSolidRect(point.x-1,point.y-1,2,2,BLACK);
      }
      break;
    case 2:
      { CPoint old;
        MoveToEx(dc,m_p[0].x,m_p[0].y,&old);
        LineTo(dc,m_p[1].x,m_p[1].y);
      }
      break;
    case 3:
      m_edges = new Subdivision(Point2DP(m_p[0]),Point2DP(m_p[1]),Point2DP(m_p[2]));
      paintEdges();
      break;    
    }
  }

  CDialogEx::OnLButtonDown(nFlags, point);
}
