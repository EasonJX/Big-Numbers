#pragma once

#include "Thread.h"

class Timer;

class TimeoutHandler {
public:
  virtual void handleTimeout(Timer &timer) = 0;
};

class TimerThread;

class Timer {
private:
  const int         m_id;
  TimerThread      *m_thread;
  mutable Semaphore m_gate;
public:
  Timer(int id);
  ~Timer();
  void startTimer(int msec, TimeoutHandler &handler, bool repeatTimeout = false);
  void stopTimer();
  inline int getId() const {
    return m_id;
  }
  int getTimeout() const;
  bool isRunning() const;
};
