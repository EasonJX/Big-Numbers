#pragma once

#include "Runnable.h"

class InterruptableRunnable : public Runnable {
private:
  HANDLE m_thr;
  BYTE   m_flags;

  InterruptableRunnable(           const InterruptableRunnable &); // not implemented. not clonable
  InterruptableRunnable &operator=(const InterruptableRunnable &); // not implemented

  void clrSuspended() {
    m_flags &= ~1;
  }

protected:
  // the next 3 functions should only be called by this (operate on currentthread)
  void die(const TCHAR *msg = NULL);  // throw Exception. If msg = NULL, exception-text is "Interrupted"
  void suspend();
  inline void setTerminated() {
    m_flags |= 4;
  }
  virtual void handleInterruptOrSuspend();
  void clearAllFlags() { // if job has to be started again, use this to reset
    m_flags = 0;
  }
public:
  inline InterruptableRunnable() : m_thr(INVALID_HANDLE_VALUE), m_flags(0) {
  }
  void setInterrupted(); // mark this as interrupted. If isSuspended(), resume will be called

  inline void setSuspended() {
    m_flags |= 1;
  }

  inline bool isInterrupted() const {
    return (m_flags & 2) != 0;
  }
  inline bool isSuspended() const {
    return (m_flags & 1) != 0;
  }
  inline bool isTerminated() const {
    return (m_flags & 4) != 0;
  }
  inline bool isInterruptedOrSuspended() const {
    return (m_flags&3) != 0;
  }

  // Should should only be called by this, or method called directly og indirectly by this
  // (operate on currentthread, that is the thread executing this runnable)
  inline void checkInterruptAndSuspendFlags() {
    if(isInterruptedOrSuspended()) {
      handleInterruptOrSuspend();
    }
  }
  // should only be called by foreign thread to wake up this
  void resume();
};
