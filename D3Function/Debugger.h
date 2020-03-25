#pragma once

#ifdef DEBUG_POLYGONIZER

#ifndef _DEBUG
#error "Must compile with _DEBUG"
#endif

#include <InterruptableRunnable.h>
#include <PropertyContainer.h>
#include <FlagTraits.h>
#include <D3DGraphics/D3SceneEditor.h>
#include "DebugIsoSurface.h"

#define FL_BREAKONNEXTLEVEL  0x01
#define FL_BREAKONNEXTOCTA   0x02
#define FL_BREAKONNEXTTETRA  0x04
#define FL_BREAKONNEXTFACE   0x08
#define FL_BREAKONNEXTVERTEX 0x10
#define FL_KILLED            0x20
#define FL_ALLBREAKFLAGS (FL_BREAKONNEXTLEVEL | FL_BREAKONNEXTOCTA | FL_BREAKONNEXTTETRA | FL_BREAKONNEXTFACE | FL_BREAKONNEXTVERTEX)

typedef enum {
  DEBUGGER_STATE
} DebuggerProperties;

typedef enum {
  DEBUGGER_CREATED
 ,DEBUGGER_RUNNING
 ,DEBUGGER_PAUSED
 ,DEBUGGER_TERMINATED
} DebuggerState;

class Debugger : public InterruptableRunnable, public PropertyContainer {
private:
  FLAGTRAITS(Debugger, BYTE, m_flags)
    DebugIsoSurface                   *m_surface;
  DebuggerState                      m_state;
  inline Debugger &checkKilled() {
    if(isSet(FL_KILLED)) throwException(_T("Killed"));
    return *this;
  }
  inline Debugger &checkTerminated() {
    if(getState() == DEBUGGER_TERMINATED) throwException(_T("Debugger is terminated"));
    return *this;
  }
  void suspend();
public:
  Debugger(D3SceneContainer *sc, const IsoSurfaceParameters &param);
  ~Debugger();
  UINT safeRun();
  void singleStep(BYTE breakFlags);
  inline void go() {
    singleStep(0);
  }
  void kill();
  void handleStep(StepType type);
  inline BYTE getFlags() const {
    return m_flags;
  }
  inline String getFlagNames() const {
    return getFlagNames(getFlags());
  }
  static String getFlagNames(BYTE flags);
  inline DebuggerState getState() const {
    return m_state;
  }
  inline String getStateName() const {
    return getStateName(getState());
  }
  static String getStateName(DebuggerState state);
  D3SceneObjectVisual *getSceneObject();
  const DebugIsoSurface &getDebugSurface() const {
    return *m_surface;
  }
};

#endif // DEBUG_POLYGONIZER
