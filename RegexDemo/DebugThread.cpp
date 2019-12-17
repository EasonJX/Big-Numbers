#include "stdafx.h"
#include "DebugThread.h"

DebugThread::DebugThread(DebugRegex &regex, const CompileParameters &cp, const BitSet &breakPoints)
: Thread(_T("Debugger"))
, m_command(COMMAND_COMPILE)
, m_regex(regex)
, m_compileParameters(cp)
, m_breakPoints(breakPoints)
{
  initThread(true);
}

DebugThread::DebugThread(ThreadCommand command, DebugRegex &regex, const String &text, const BitSet &breakPoints)
: Thread(_T("Debugger"))
, m_command(command)
, m_regex(regex)
, m_text(text)
, m_breakPoints(breakPoints)
{
  initThread(m_command == COMMAND_COMPILE);
  if(m_command == COMMAND_COMPILE) {
    m_compileParameters = text;
  }
}

void DebugThread::initThread(bool singleStep) {
  clearStates();
  m_running     = m_finished = m_killed = false;
  m_singleStep  = singleStep;
  m_regexPhase  = REGEX_UNDEFINED;
}

void DebugThread::clearStates() {
  m_handlerState = NULL;
}

DebugThread::~DebugThread() {
  enableHandleStep(false);
  kill();
  for(int i = 0; i < 10; i++) {
    if(!stillActive()) {
      return;
    }
    Sleep(50);
  }
  if(stillActive()) {
    showWarning(_T("DebugThread still active in destructor"));
  }
}

void DebugThread::singleStep() {
  if(isFinished()) {
    throwException(_T("%s:Thread has exited"),__TFUNCTION__);
  }
  m_singleStep = true;
  enableHandleStep(true);
  resume();
}

void DebugThread::go() {
  if(isFinished()) {
    throwException(_T("%s:Thread has exited"),__TFUNCTION__);
  }
  m_singleStep = false;
  enableHandleStep(!m_breakPoints.isEmpty());
  resume();
}

void DebugThread::kill() {
  if(!isFinished()) {
    m_killed = true;
    if(!isRunning()) {
      resume();
    }
  }
}

UINT DebugThread::run() {
  setPropRunning(true);
  m_foundStart   = -1;
  m_resultLength = 0;
  try {
    switch(m_command) {
    case COMMAND_COMPILE:
      m_regex.compilePattern(m_compileParameters);
      m_resultMsg  = _T("Pattern Ok");
      m_regexPhase = REGEX_COMPILEDOK;
      break;
    case COMMAND_SEARCHFORWARD:
    case COMMAND_SEARCHBACKWRD:
      m_foundStart = m_regex.search(m_text, m_command == COMMAND_SEARCHFORWARD);
      if(m_foundStart >= 0) {
        m_resultLength = m_regex.getResultLength();
        m_resultMsg    = format(_T("Found at %d. length:%d"), m_foundStart, m_resultLength);
        m_regexPhase   = REGEX_PATTERNFOUND;
      } else {
        m_resultMsg    = _T("Not found");
        m_regexPhase   = REGEX_SEARCHFAILED;
      }
      break;
    case COMMAND_MATCH  :
      if(m_regex.match(m_text)) {
        m_foundStart   = 0;
        m_resultLength = (int)m_text.length();
        m_resultMsg    = _T("Match");
        m_regexPhase   = REGEX_PATTERNFOUND;
      } else {
        m_resultMsg    = _T("No match");
        m_regexPhase   = REGEX_MATCHFAILED;
      }
      break;
    }
  } catch(Exception e) {
    m_resultMsg  = e.what();
    m_regexPhase = REGEX_UNDEFINED;
  } catch(...) {
    m_resultMsg  = _T("Unknown exception");
    m_regexPhase = REGEX_UNDEFINED;
  }
  m_finished = true;
  setPropRunning(false);
  return 0;
}

void DebugThread::enableHandleStep(bool enabled) {
  m_regex.setHandler(enabled ? this : NULL);
}

void DebugThread::handleCompileStep(const _RegexCompilerState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_COMPILING);
}

void DebugThread::handleSearchStep(const _RegexSearchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_SEARCHING);
}

void DebugThread::handleMatchStep(const _RegexMatchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_MATCHING, state.getDBGLineNumber());
}

void DebugThread::handleCompileStep(const _DFARegexCompilerState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_COMPILING);
}

void DebugThread::handleSearchStep(const _DFARegexSearchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_SEARCHING);
}

void DebugThread::handleMatchStep(const _DFARegexMatchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_MATCHING, state.getDBGLineNumber());
}

void DebugThread::suspendOnSingleStep(RegexPhaseType phase, int lineNumber) {
  if(m_singleStep || ((lineNumber>=0) && m_breakPoints.contains(lineNumber))) {
    m_regexPhase  = phase;
    setPropRunning(false);
    suspend();
    m_regexPhase  = REGEX_UNDEFINED;
    setPropRunning(true);
  }
  clearStates();
  if(m_killed) {
    throwException(_T("Killed"));
  }
}

void DebugThread::getFoundPosition(int &start, int &end) {
  if(getRegexPhase() != REGEX_PATTERNFOUND) {
    start = end = -1;
  } else {
    start = (int)m_foundStart;
    end   = (int)(start + m_resultLength);
  }
}

String DebugThread::getResultMsg() const {
  if(isRunning()) {
    return _T("No result yet. Thread not finished");
  }
  return m_resultMsg;
}

String DebugThread::registersToString() const {
  if(m_regexPhase != REGEX_PATTERNFOUND) {
    throwException(_T("Cannot get register at this time of the search"));
  }
  return m_regex.registersToString();
}

const _RegexCompilerState &DebugThread::getEmacsCompilerState() const {
  validateRegexTypeAndPhase(EMACS_REGEX, REGEX_COMPILING);
  return *(_RegexCompilerState*)m_handlerState;
}

const _RegexSearchState &DebugThread::getEmacsSearchState() const {
  validateRegexTypeAndPhase(EMACS_REGEX, REGEX_SEARCHING);
  return *(_RegexSearchState*)m_handlerState;
}

const _RegexMatchState &DebugThread::getEmacsMatchState() const {
  validateRegexTypeAndPhase(EMACS_REGEX, REGEX_MATCHING);
  return *(_RegexMatchState*)m_handlerState;
}

const _DFARegexCompilerState &DebugThread::getDFACompilerState() const {
  validateRegexTypeAndPhase(DFA_REGEX, REGEX_COMPILING);
  return *(_DFARegexCompilerState*)m_handlerState;
}

const _DFARegexSearchState &DebugThread::getDFASearchState() const {
  validateRegexTypeAndPhase(DFA_REGEX, REGEX_SEARCHING);
  return *(_DFARegexSearchState*)m_handlerState;
}

const _DFARegexMatchState &DebugThread::getDFAMatchState() const {
  validateRegexTypeAndPhase(DFA_REGEX, REGEX_MATCHING);
  return *(_DFARegexMatchState*)m_handlerState;
}

void DebugThread::setPropRunning(bool value) {
  setProperty(THREAD_RUNNING, m_running, value);
}

static const TCHAR *regexTypeName[] = {
  _T("EMACS_REGEX")
 ,_T("DFA_REGEX")
};

String DebugThread::getPhaseName(RegexPhaseType phase) { // static
  switch(phase) {
  case REGEX_UNDEFINED     : return _T("Undefined"      );
  case REGEX_COMPILING     : return _T("Compiling"      );
  case REGEX_COMPILEDOK    : return _T("Compiled ok"    );
  case REGEX_COMPILEDFAILED: return _T("Compiled failed");
  case REGEX_SEARCHING     : return _T("Searching"      );
  case REGEX_MATCHING      : return _T("Matching"       );
  case REGEX_PATTERNFOUND  : return _T("Pattern found"  );
  case REGEX_SEARCHFAILED  : return _T("Search failed"  );
  case REGEX_MATCHFAILED   : return _T("Match failed"   );
  default                  : return format(_T("Unknown phase (=%d)"), phase);
  }
}

void DebugThread::validateRegexTypeAndPhase(RegexType expectedType, RegexPhaseType expectedPhase) const {
  if(m_regex.getType() != expectedType) {
    throwException(_T("Expected regextype %s. current is %s")
                  ,regexTypeName[expectedType]
                  ,regexTypeName[m_regex.getType()]);
  }
  if(m_running || m_finished || (m_regexPhase != expectedPhase)) {
    throwException(_T("Cannot get %s-state at this time")
                  ,getPhaseName(expectedPhase).cstr());
  }
}
