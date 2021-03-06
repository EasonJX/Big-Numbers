#pragma once

#include <Config.h>

#if !defined(ISODEBUGGER)
#define D3TM_LIB_VERSION TM_LIB_VERSION
#else // ISODEBUGGER
#if defined(_DEBUG)
#define _D3CONFIGURATION_ "IsoDebuggerDebug/"
#else // _DEBUG
#define _D3CONFIGURATION_ "IsoDebuggerRelease/"
#endif // _DEBUG
#define D3TM_LIB_VERSION LIBROOT _PLATFORM_ _TMPREFIX_ _D3CONFIGURATION_
#endif // ISODEBUGGER

#define D3DGRAPHICS_TM_LIB_VERSION D3TM_LIB_VERSION

#if defined(GIVE_LINK_MESSAGE)
#pragma message("link with " D3DGRAPHICS_TM_LIB_VERSION)
#endif

#pragma comment(lib, D3DGRAPHICS_TM_LIB_VERSION "D3DGraphics.lib")
