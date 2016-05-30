#include "stdafx.h"
#include <MyUtil.h>
#include <shlobj.h>
#include <MFCUtil/Clipboard.h>

void clipboardDropFiles(HWND hwnd, StringArray &fnames) {
  if(!OpenClipboard(hwnd)) {
    throwLastErrorOnSysCallException(_T("OpenClipboard"));
  }

  try {
    if(!EmptyClipboard()) {
      throwLastErrorOnSysCallException(_T("EmptyClipboard"));
    }

    DROPFILES df;
    df.pFiles = sizeof(df);
    df.pt.x   = 0;
    df.pt.y   = 0;
    df.fNC    = 0;
    df.fWide  = 0;

    const String str = fnames.getAsDoubleNullTerminatedString();
    const int strLength = str.length() + 1;
    const int nbytes = sizeof(df) + strLength*sizeof(TCHAR);

    HLOCAL buf = LocalAlloc(0, nbytes);
    memcpy(buf, &df, sizeof(df));
    memcpy(((char*)buf)+sizeof(df), str.cstr(), strLength*sizeof(TCHAR));
    if(SetClipboardData(CF_HDROP, buf) == NULL) {
      throwLastErrorOnSysCallException(_T("SetClipboardData"));
    }
    CloseClipboard();
  } catch(...) {
    CloseClipboard();
    throw;
  }
}

void clipboardDropFile(HWND hwnd, const String &fname) {
  StringArray names;
  names.add(fname);

  clipboardDropFiles(hwnd, names);
}

void clipboardDropFile(HWND hwnd, const TCHAR *fname) {
  StringArray names;
  names.add(fname);

  clipboardDropFiles(hwnd, names);
}
