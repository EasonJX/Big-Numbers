#include "stdafx.h"
#include "mandelbrot.h"
#include "JuliaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CJuliaDlg::CJuliaDlg(const RealPoint2D &point, CWnd* pParent /*=NULL*/)
: m_point(point)
, CDialog(CJuliaDlg::IDD, pParent)
{
  m_thread = NULL;

}

CJuliaDlg::~CJuliaDlg() {
  if(m_thread != NULL) {
    delete m_thread;
  }
}

void CJuliaDlg::DoDataExchange(CDataExchange* pDX){
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CJuliaDlg, CDialog)
END_MESSAGE_MAP()

BOOL CJuliaDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  const RealRectangle rr(-2,-2,4,4);

  m_transform.setFromRectangle(rr);
  adjustToRectangle();

  m_thread = new JuliaCalculatorThread(*this);
  m_thread->start();
  return TRUE;
}

void CJuliaDlg::adjustToRectangle() {
  CRect cl = getClientRect(this);
//  const int t = cl.bottom;
//  cl.bottom = cl.top;
//  cl.top = t;
  m_transform.setToRectangle(cl);
}

JuliaCalculatorThread::JuliaCalculatorThread(CJuliaDlg &dlg) : m_dlg(dlg) {
  m_killed = false;
}


JuliaCalculatorThread::~JuliaCalculatorThread() {
  kill();
}

void JuliaCalculatorThread::kill() {
  m_killed = true;
  while(stillActive()) {
    Sleep(20);
  }
};

UINT JuliaCalculatorThread::run() {
  const RealPoint2D                  c    = m_dlg.getC();
  const RealRectangleTransformation &tr   = m_dlg.getTransform();

  RealPoint2D z = c;
#define POINTBUFFERSIZE 10000
  CPoint pointBuffer[POINTBUFFERSIZE];
  int index = 0;
  while(!m_killed) {
    pointBuffer[index++] = tr.forwardTransform(z);
    if(index == POINTBUFFERSIZE) {
      CClientDC dc(&m_dlg);
      for(const CPoint *p = pointBuffer; index--; p++) {
        dc.SetPixel(p->x,p->y, RGB(0,0,255));
      }
      index = 0;
    }

    const Real x2 = z.x*z.x;
    const Real y2 = z.y*z.y;
    if(x2+y2 > 4) {
      break;
    }
    const Real tmp = x2-y2-c.x;
    z.y = 2*z.x*z.y-c.y;
    z.x = tmp;
  }
  if(!m_killed && (index > 0)) {
    CClientDC dc(&m_dlg);
    for(const CPoint *p = pointBuffer; index--; p++) {
      dc.SetPixel(p->x,p->y, RGB(0,0,0));
    }
  }
  return 0;
}

