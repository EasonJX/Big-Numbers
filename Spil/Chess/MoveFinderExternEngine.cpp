#include "stdafx.h"

#ifndef TABLEBASE_BUILDER

#include "MoveFinderExternEngine.h"

MoveFinderExternEngine::MoveFinderExternEngine(Player player, MFTRQueue &msgQueue)
: AbstractMoveFinder(player, msgQueue)
, m_externEngine(player, Options::getEnginePathByPlayer(player))
{
  m_externEngine.start(&m_msgQueue);
  m_externEngine.setParameters(getOptions().getEngineOptionValues(player, getOptions().getPlayerOptions(player).m_engineName));
  m_optionsDlgThread = NULL;
}

MoveFinderExternEngine::~MoveFinderExternEngine() {
  try {
    m_externEngine.quit();
    if(m_optionsDlgThread) {
      m_optionsDlgThread->closeThread();
    }
  } catch(...) {
  }
}

void MoveFinderExternEngine::runOptionsDialog() {
  m_optionsDlgThread = CEngineOptionsDlgThread::startThread(this);
}

void MoveFinderExternEngine::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if((source == m_optionsDlgThread) && (id == ENGINEOPIONDIALOG_RUNNING)) {
    const bool isRunning = *(const bool*)newValue;
    if(!isRunning) {
      m_optionsDlgThread = NULL;
    }
  }
}

void MoveFinderExternEngine::findBestMove(const FindMoveRequestParam &param, bool talking) {
  initSearch(param, talking);
  const PrintableMove m = checkForSingleMove();
  if(m.isMove()) {
    putResult(m);
  } else {
    m_externEngine.findBestMove(param);
  }
}

String MoveFinderExternEngine::getName() const {
  return m_externEngine.getDescription().getName();
}

void MoveFinderExternEngine::stopSearch() {
  m_externEngine.stopSearch();
}

void MoveFinderExternEngine::moveNow() {
  m_externEngine.moveNow();
}

void MoveFinderExternEngine::setVerbose(bool verbose) {
  AbstractMoveFinder::setVerbose(verbose);
  m_externEngine.setVerbose(isVerbose());
}

void MoveFinderExternEngine::notifyGameChanged(const Game &game) {
  m_externEngine.notifyGameChanged(game);
}

void MoveFinderExternEngine::notifyMove(const MoveBase &move) {
}

String MoveFinderExternEngine::getStateString(Player computerPlayer, bool detailed) {
  return m_externEngine.toString();
}

#endif // TABLEBASE_BUILDER
