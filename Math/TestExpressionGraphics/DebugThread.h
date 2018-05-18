#pragma once

#include <Thread.h>
#include <TinyBitSet.h>
#include <PropertyContainer.h>

using namespace Expr;

typedef enum {
  THREAD_RUNNING           // bool*
 ,THREAD_TERMINATED        // bool*
 ,THREAD_ERROR             // TCHAR*
} DebugThreadProperty;

class DebugThread : public Thread, public PropertyContainer, public PropertyChangeListener {
private:
  bool                  m_running, m_killed, m_terminated;

#ifdef TRACE_REDUCTION_CALLSTACK
  const ReductionStack *m_reductionStack;
  int                   m_savedStackHeight;
  const TCHAR          *m_savedMethod;
#endif

  String                m_errorMsg;
  BitSet8               m_breakPoints;
  bool                  m_stoppedOnReturn;
  Expression           &m_expr;
  const Expression     *m_exprp;
  ParserTree           *m_treep;
  void stop(bool onReturn = false);
  void throwInvalidStateException(const TCHAR *method, ParserTreeState state) const;
public:
  DebugThread(Expression &expr);
  ~DebugThread();
  unsigned int run();

  void go();
  void singleStep();
  void singleSubStep();
#ifdef TRACE_REDUCTION_CALLSTACK
  void goUntilReturn();
#endif
  void stopASAP();
  void kill();

  Expression &getDebugExpr() {
    return *(Expression*)m_exprp;
  }
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  inline const String &getErrorMsg() const {
    return m_errorMsg;
  }
  inline bool isRunning() const {
    return m_running;
  }
  inline bool isTerminated() const {
    return m_terminated;
  }
};
