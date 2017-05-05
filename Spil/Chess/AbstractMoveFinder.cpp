#include "stdafx.h"
#include "AbstractMoveFinder.h"
#include "TraceDlgThread.h"

AbstractMoveFinder::AbstractMoveFinder(Player player, ChessPlayerRequestQueue &msgQueue)
: m_player(player)
, m_msgQueue(msgQueue)
{
  m_verbose  = false;
}

void AbstractMoveFinder::initSearch(const FindMoveRequestParam &param, bool talking) {
  m_game = param.getGame();
#ifndef TABLEBASE_BUILDER
  m_game.setMaxPositionRepeat(1);
#endif
  m_timeLimit  = param.getTimeLimit();
  m_hint       = param.isHint();
  setVerbose(talking);
}

void AbstractMoveFinder::putMove(const MoveBase &m) {
  m_msgQueue.put(ChessPlayerRequest(m, m_hint));
}

PrintableMove AbstractMoveFinder::checkForSingleMove() {
  if(m_game.isSingleMovePosition()) { // dont waste time to think. just return the only possible move
    const PrintableMove result(m_game, m_game.getRandomMove());
    if(isVerbose()) {
      verbose(format(_T("%s:%s\n"), loadString(IDS_MSG_ONLY_1_MOVE).cstr(), result.toString().cstr()).cstr());
    }
    return result;
  }
  return PrintableMove();
}
