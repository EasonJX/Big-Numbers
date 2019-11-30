#pragma once

#include "MyUtil.h"
#include "Runnable.h"
#include "FastSemaphore.h"
#include "PropertyContainer.h"

class Thread;

HANDLE getCurrentThreadHandle(); // call DuplicateHandle(GetCurrentThread())
                                 // returned Handle should be closed with CloseHandle()
                                 // after use. Throw Exception on failure

void   setThreadDescription(HANDLE hThread, const String &description);
String getThreadDescription(HANDLE hThread);

class UncaughtExceptionHandler {
public:
  virtual void uncaughtException(Thread &thread, Exception &e) = 0;
};

class IdentifiedResource {
private:
  const int m_id;
public:
  IdentifiedResource(int id) : m_id(id) {
  }
  inline int getId() const {
    return m_id;
  }
};

typedef enum {
  THR_SHUTTINGDDOWN
} ThreadSetProperty;

/* Threadpriorities defined in winbase.h
 *  THREAD_PRIORITY_IDLE
 *  THREAD_PRIORITY_LOWEST
 *  THREAD_PRIORITY_BELOW_NORMAL
 *  THREAD_PRIORITY_NORMAL
 *  THREAD_PRIORITY_ABOVE_NORMAL
 *  THREAD_PRIORITY_TIME_CRITICAL
 *  THREAD_PRIORITY_HIGHEST
 */
class Thread : public Runnable {
private:
  DECLARECLASSNAME;
  friend UINT          threadStartup(Thread *thread);
  friend class         ThreadMap;
  static PropertyContainer        *s_propertySource;
  static UncaughtExceptionHandler *s_defaultUncaughtExceptionHandler;
  DWORD                            m_threadId;
  HANDLE                           m_threadHandle;
  FastSemaphore                    m_terminated;
  Runnable                        *m_target;
  UncaughtExceptionHandler        *m_uncaughtExceptionHandler;
  bool                             m_isDemon;

  Thread(           const Thread &src);                                      // Not defined. Class not cloneable
  Thread &operator=(const Thread &src);                                      // Not defined. Class not cloneable

  void              handleUncaughtException(Exception &e);
  void              init(const String &desciption, Runnable *target, size_t stackSize);
  static ThreadMap *threadMapRequest(int request);
  static ThreadMap &getMap();
  static void       releaseMap();
public:
  Thread(const String &description, Runnable &target, size_t stackSize = 0); // stacksize = 0, makes createThread use the default stacksize
  Thread(const String &description                  , size_t stackSize = 0); // do
  virtual ~Thread();
  void setPriority(int priority);
  int  getPriority() const;
  void setPriorityBoost(bool disablePriorityBoost);
  bool getPriorityBoost() const;
  bool stillActive() const;
  inline DWORD getThreadId() const {
    return m_threadId;
  }
  void   setDesription(const String &description);
  String getDescription() const;
  double getThreadTime(); // microseconds
  void   setAffinityMask(DWORD mask);
  void   setIdealProcessor(DWORD cpu);
  void   suspend();
  void   resume();
  inline void start() {
    resume();
  }

  UINT run();

  inline DWORD getId() const {
    return m_threadId;
  }
  inline void setDemon(bool on) {
    m_isDemon = on;
  }
  inline bool isDemon() const {
    return m_isDemon;
  }
  ULONG getExitCode() const;

  static void setDefaultUncaughtExceptionHandler(UncaughtExceptionHandler &eh) {
    s_defaultUncaughtExceptionHandler = &eh;
  }

  static UncaughtExceptionHandler &getDefaultUncaughtExceptionHandler() {
    return *s_defaultUncaughtExceptionHandler;
  }

  void setUncaughtExceptionHandler(UncaughtExceptionHandler &eh) {
    m_uncaughtExceptionHandler = &eh;
  }

  UncaughtExceptionHandler &getUncaughtExceptionHandler() {
    return *m_uncaughtExceptionHandler;
  }

  // This pointer will be source (PropertyContainer*) int PropertyChangeListener::handlePropertyChanged
  static inline bool isPropertyContainer(const PropertyContainer *source) {
    return source == s_propertySource;
  }
  static void addPropertyChangeListener(   PropertyChangeListener *listener);
  static void removePropertyChangeListener(PropertyChangeListener *listener);

  static EXECUTION_STATE setExecutionState(EXECUTION_STATE newState);

  static Thread *getThreadById(DWORD threadId);
  static inline Thread *getCurrentThread() {
    return getThreadById(GetCurrentThreadId());
  }
  static void keepAlive(EXECUTION_STATE flags = ES_CONTINUOUS | ES_SYSTEM_REQUIRED); // can add ES_DISPLAY_REQUIRED
};
