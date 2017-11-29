#include "pch.h"
#include "D80ToDbgString.h"

ADDIN_API HRESULT WINAPI AddIn_Double80(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Double80 d;
    pHelper->getRealObject(&d, sizeof(d));
    const String s = D80ToDbgString(d);
    USES_CONVERSION;
    char str[150];
    strcpy(str,T2A(s.cstr()));
    strncpy(pResult, str, maxResult);
  } catch (...) {
    strncpy(pResult, "", maxResult);
  }
  return S_OK;
}

String D80ToDbgString(const Double80 &d80) {
  const int expo10 = Double80::getExpo10(d80);
  if(expo10 < -4 || (expo10 >= 18)) {
    TCHAR str[150];
    return d80tot(str, d80);
  } else {
    return toString(d80, 17-expo10,19,ios::fixed);
  }
}
