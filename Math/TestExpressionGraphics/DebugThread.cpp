#include "stdafx.h"
#include "DebugThread.h"

DEFINECLASSNAME(DebugThread);

DebugThread::DebugThread(Expression &expr) : m_expr(expr) {
  m_running = m_killed = m_terminated = false;
  m_exprp   = &m_expr;
}

DebugThread::~DebugThread() {
}

typedef enum {
  BREAKSTEP
 ,BREAKSUBSTEP
 ,BREAKONRETURN
} BreakPointType;

void DebugThread::throwInvalidStateException(ExpressionState state) const {
  throwMethodInvalidArgumentException(s_className, _T("handleData"), _T("State=%d"), state);
}

void DebugThread::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(m_killed) throw true;

  m_exprp = (const Expression*)source;

  switch(id) {
  case EXPR_STATE           :
    switch(m_exprp->getState()) {
    case EXPR_EMPTY    :
    case EXPR_COMPILED :
    case EXPR_DERIVED  :
      break;
    default:
      if(m_breakPoints.contains(BREAKSTEP)) stop();
      break;
    }
    break;

  case EXPR_ROOT            :
    { const ExpressionNode *newRoot = (const ExpressionNode*)newValue;
      if(newRoot) { // dont stop on null-trees
        stop();
      }
    }
    break;

  case EXPR_REDUCEITERATION :
    if(m_breakPoints.contains(BREAKSUBSTEP)) stop();
    break;

#ifdef TRACE_REDUCTION_CALLSTACK
  case REDUCTION_STACKHIGHT :
    { const int oldStack = *(int*)oldValue;
      const int newStack = *(int*)newValue;
      if(newStack > oldStack) { // its a push
        const ReductionStack        &reductionStack = m_exprp->getReductionStack();
        const ReductionStackElement &top = reductionStack.top();
        if(top.m_node && top.m_node->isBreakPoint()) {
          stop();
          break;
        }
        if(m_breakPoints.contains(BREAKONRETURN)) {
          if((oldStack == m_savedStackHeight) && (m_reductionStack->top().m_method == m_savedMethod)) {
            stop(true);
            break;
          }
        }
      }
      if(m_breakPoints.contains(BREAKSUBSTEP)) {
        stop();
        break;
      }
    }
    break;
#endif
  }
  m_exprp = &m_expr;
}

unsigned int DebugThread::run() {
  setDeamon(true);
  try {
    setBoolProperty(THREAD_RUNNING, m_running, true);
    m_expr.reduce();
  } catch(Exception e) {
    m_errorMsg = e.what();
    notifyPropertyChanged(THREAD_ERROR, _T(""), m_errorMsg.cstr());
  } catch(bool) {
    // ignore. thrown after resume, when killed
  }
  catch (...) {
    m_errorMsg = _T("Unknown exception caught in DebugThread");
  }
  setBoolProperty(THREAD_TERMINATED, m_terminated, true );
  setBoolProperty(THREAD_RUNNING   , m_running   , false);
  
  return 0;
}

void DebugThread::setBoolProperty(DebugThreadProperty id, bool &v, bool newValue) {
  if(newValue != v) {
    const bool oldValue = v;
    v = newValue;
    notifyPropertyChanged(id, &oldValue, &v);
  }
}

void DebugThread::stop(bool onReturn) {
  setBoolProperty(THREAD_RUNNING, m_running, false);
  m_stoppedOnReturn = onReturn;
  suspend();
  if(m_killed) throw true;
  setBoolProperty(THREAD_RUNNING, m_running, true );
}

void DebugThread::go() {
  if(m_running) return;
  m_breakPoints.remove( BREAKSTEP    );
  m_breakPoints.remove( BREAKSUBSTEP );
  m_breakPoints.remove( BREAKONRETURN);
  resume();
}

void DebugThread::singleStep() {
  if(m_running) return;
  m_breakPoints.add(    BREAKSTEP   );
  m_breakPoints.remove( BREAKSUBSTEP);
  m_breakPoints.remove( BREAKONRETURN);
  resume();
}

void DebugThread::singleSubStep() {
  if(m_running) return;
  m_breakPoints.add(    BREAKSTEP   );
  m_breakPoints.add(    BREAKSUBSTEP);
  m_breakPoints.remove( BREAKONRETURN);
  resume();
}

#ifdef TRACE_REDUCTION_CALLSTACK

void DebugThread::goUntilReturn() {
  if(m_running) return;
  if(m_stoppedOnReturn) {
    singleSubStep();
  } else {
    m_breakPoints.remove( BREAKSTEP    );
    m_breakPoints.remove( BREAKSUBSTEP );
    m_breakPoints.add(    BREAKONRETURN);
    m_reductionStack   = &(m_exprp->getReductionStack());
    m_savedStackHeight = m_reductionStack->getHeight();
    m_savedMethod      = m_reductionStack->top().m_method;
    resume();
  }
}
#endif

void DebugThread::stopASAP() {
  if(!m_running) return;
  m_breakPoints.add(   BREAKSTEP   );
  m_breakPoints.add(   BREAKSUBSTEP);
}

void DebugThread::kill() {
  if(!m_terminated) {
    m_killed = true;
    if(m_running) {
      stopASAP();
    } else {
      resume();
    }
  }
  for(int i = 0; i < 10; i++) {
    if(stillActive()) {
      Sleep(50);
    } else {
      break;
    }
  }
  if(stillActive()) {
    throwException(_T("Cannot kill debugThread"));
  }
}
