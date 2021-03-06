#include "pch.h"

String getWindowText(const CWnd *wnd) {
  CString s;
  wnd->GetWindowText(s);
  return (LPCTSTR)s;
}

String getWindowText(const CWnd *wnd, int id) {
  CWnd *ctrl = wnd->GetDlgItem(id);
  if(ctrl == NULL) {
    showWarning(_T("No dlgItem %d in window"), id);
    return EMPTYSTRING;
  }
  return getWindowText(ctrl);
}

void setWindowText(CWnd *wnd, const String &str) {
  wnd->SetWindowText(str.cstr());
}

void setWindowText(CWnd *wnd, int id, const String &str) {
  CWnd *ctrl = wnd->GetDlgItem(id);
  if(ctrl == NULL) {
    showWarning(_T("No dlgItem %d in window"), id);
    return;
  }
  setWindowText(ctrl, str);
}

CSize getTextExtent(HDC hdc, const String &s) {
  CSize size;
  GetTextExtentPoint32(hdc, s.cstr(),(int)s.length(), &size);
  return size;
}

CSize getTextExtent1(HDC hdc, const StringArray &a) {
  CSize result(0,0);
  for(size_t i = 0; i < a.size(); i++) {
    const CSize lineSize = getTextExtent(hdc, a[i]);
    result.cy += lineSize.cy;
    result.cx = max(result.cx, lineSize.cx);
  }
  return result;
}

CSize getMaxTextExtent(HDC hdc, const StringArray &a) {
  CSize result(0,0);
  for(size_t i = 0; i < a.size(); i++) {
    const CSize sz = getTextExtent(hdc, a[i]);
    result.cx = max(result.cx, sz.cx);
    result.cy = max(result.cy, sz.cy);
  }
  return result;
}
