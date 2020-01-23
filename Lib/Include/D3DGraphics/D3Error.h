#pragma once

#ifdef _DEBUG

void checkD3DResult(const TCHAR *method, HRESULT hr, bool exitOnError);

#define V(hr) checkD3DResult(__TFUNCTION__, hr, true )
#define W(hr) checkD3DResult(__TFUNCTION__, hr, false)
#else

void checkD3DResult(HRESULT hr, bool exitOnError);

#define V( hr) checkD3DResult(hr, true )
#define VW(hr) checkD3DResult(hr, false)

#endif

#define FV(hr) V(hr)
#define FW(hr) VW(hr)
