#include "pch.h"
#include <D3DGraphics/D3Scene.h>

LPD3DXEFFECT compileEffect(LPDIRECT3DDEVICE device, const String &srcText, StringArray &errorMsg) {
  LPD3DXEFFECT effect         = NULL;
  LPD3DXBUFFER compilerErrors = NULL;
  DWORD        flags          = D3DXFX_NOT_CLONEABLE;
#ifdef D3DXFX_LARGEADDRESS_HANDLE
  flags |= D3DXFX_LARGEADDRESSAWARE;
#endif
  USES_CONVERSION;
  const char *text = T2A(srcText.cstr());
  int textLen = (int)strlen(text);
  try {
    V(D3DXCreateEffect(device, text, textLen, NULL, NULL, flags, NULL, &effect, &compilerErrors));
    TRACE_CREATE(effect);
    return effect;
  } catch(Exception e) {
    const String errorText = (char*)compilerErrors->GetBufferPointer();
    errorMsg = StringArray(Tokenizer(errorText, _T("\n\r")));
    return NULL;
  }
}
