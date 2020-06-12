#pragma once

#include "targetver.h"

#include <ChessGame.h>

#define CHESSLIB_ROOT "C:/mytools2015/Spil/ChessLib/"

#if defined(NEWCOMPRESSION)
#define CHESSLIB_CONFIG "NewComp" _CONFIGURATION_
#else
#define CHESSLIB_CONFIG "TablebaseBuilder" _CONFIGURATION_
#endif

#define CHESSLIB_VERSION CHESSLIB_ROOT _PLATFORM_ CHESSLIB_CONFIG

#if defined(GIVE_LINK_MESSAGE)
#pragma message(__FILE__ ":link with " CHESSLIB_VERSION)
#endif
#pragma comment(lib, CHESSLIB_VERSION "ChessLib.lib")

