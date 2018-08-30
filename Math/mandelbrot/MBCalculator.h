#pragma once

#include <CompactStack.h>
#include <TinyBitSet.h>
#include <PropertyContainer.h>
#include <Math/FPU.h>
#include <Math/Transformation.h>
#include <Math/BigRealTransformation.h>
#include "CellCountMatrix.h"
#include "PointSet.h"
#include "EdgeMatrix.h"

#define ORBITCOLOR RGB(0,0,255)

class OrbitPoint : public CPoint {
public:
  COLORREF m_oldColor;
  inline OrbitPoint() {
  }
  inline OrbitPoint(const CPoint &p, COLORREF oldColor) : CPoint(p), m_oldColor(oldColor) {
  }
};

//#define SAVE_CALCULATORINFO

#ifdef SAVE_CALCULATORINFO

class CalculatorInfo {
private:
  int                  m_calculatorId;
  PointSet             m_blackSet, m_edgeSet, m_innerSet;
public:
  CalculatorInfo(int calculatorId, const CRect &r)
    : m_calculatorId(calculatorId)
    , m_blackSet(r)
    , m_edgeSet( r)
    , m_innerSet(r)
  {
  }
  inline void addBlack(const CPoint &p) {
    m_blackSet.add(p);
  }
  inline bool contains(const CPoint &p) const {
    return m_blackSet.getRect().PtInRect(p) ? m_blackSet.contains(p) : false;
  }
  void setEdgeAndInnerSet(const PointSet &edgeSet, const PointSet &innerSet) {
    m_edgeSet = edgeSet; m_innerSet = innerSet;
  }
  const PointSet &getEdgeSet() const {
    return m_edgeSet;
  }
  const PointSet &getInnerSet() const {
    return m_innerSet;
  }
  void addEdgeSetToBlackSet() {
    m_blackSet += m_edgeSet;
  }
  String toString() const;
};

#endif

class MBContainer : public CellCountAccessor, public PropertyChangeListener {
public:
  MBContainer(CellCountMatrix *m) : CellCountAccessor(m) {
  }
  virtual const RealRectangleTransformation    &getRealTransformation()    const  = 0;
  virtual const BigRealRectangleTransformation &getBigRealTransformation() const  = 0;
  virtual UINT                                  getMaxCount()              const  = 0;
  virtual bool                                  useFPUCalculators()        const  = 0;
  virtual size_t                                getDigits()                const  = 0;
  virtual CellCountAccessor                    *getCCA()                          = 0;
  virtual bool                                  calculateWithOrbit()       const  = 0;
  virtual bool                                  useEdgeDetection()         const  = 0;
  virtual bool                                  getJobToDo(CRect &rect)           = 0;
  virtual CSize                                 getWindowSize()            const  = 0;
  virtual void                                  paintMark(const CPoint &p)        = 0;
  virtual COLORREF                              setOrbitPixel(   const CPoint &p, COLORREF color) = 0; // return old color
  virtual void                                  resetOrbitPixels(const OrbitPoint *p, size_t count) = 0;
};

typedef enum {
  CALC_SUSPENDED  = 0
 ,CALC_RUNNING    = 1
 ,CALC_TERMINATED = 2
} CalculatorState;

#define CALC_SUSPEND_PENDING  0x01
#define CALC_KILL_PENDING     0x02

typedef BitSet16 CalculatorSet;

class CalculatorPool;

class MBCalculator : public Thread {
private:
  static Semaphore    s_followBlackEdgeGate;
  const UINT          m_id;
  double              m_startTime, m_threadTime;
  bool                m_edgeTracing;
  MBContainer        &m_mbc;
  OrbitPoint         *m_orbitPoints;
  Semaphore           m_gate;
  mutable Semaphore   m_wakeup;
#ifdef SAVE_CALCULATORINFO
  const char         *m_phase;
  CalculatorInfo     *m_info;
  void addInfoToPool();
#define SETPHASE(str) m_phase = str;
#else
#define SETPHASE(str)
#endif
// assume thread is suspended
  void  allocateOrbitPoints();
// assume thread is suspended
  void  releaseOrbitPoints();

protected:
  CalculatorPool &m_pool;
  const ULONG     m_pendingMask;
  CRect           m_currentRect;
  size_t          m_doneCount; // number of pixels calculated
  MBCalculator(CalculatorPool *pool, UINT id);
  inline void initStartTime() {
    m_startTime = m_threadTime = getThreadTime();
  }
  inline void updateThreadTime() {
    m_threadTime = getThreadTime();
  }
  CellCountAccessor *fillInnerArea(PointSet &innerSet, CellCountAccessor *cca, UINT maxCount);
  inline MBContainer &getMBContainer() const {
    return m_mbc;
  }
  inline OrbitPoint *getOrbitPoints() const {
    return m_orbitPoints;
  }
  inline bool isWithOrbit() const {
    return m_orbitPoints != NULL;
  }
  bool enterFollowBlackEdge(const CPoint &p, CellCountAccessor *cca);
  void leaveFollowBlackEdge();
  inline void enableEdgeTracing(bool enable) {
    m_edgeTracing = enable;
  }
  inline bool isEdgeTracing() const {
    return m_edgeTracing;
  }
  inline UINT getId() const {
    return m_id;
  }

#define CHECKPENDING() { if(m_pool.isPending(m_pendingMask)) { cca = handlePending(); } }

  CellCountAccessor *handlePending();
  void setPoolState(CalculatorState state);
public:
  virtual ~MBCalculator() {
    releaseOrbitPoints();
  }
  void wakeUp() {
    m_wakeup.signal();
  }
  // return number of pixels done by this thread
  inline size_t getDoneCount() const {
    return m_doneCount;
  }
  // Assume m_pool.getState(getID()) != CALC_RUNNING
  void setWithOrbit();
  inline double getTimeUsed() const {
    return m_threadTime - m_startTime;
  }
};

typedef enum {
  RUNNINGSET        // CalculatorSet
 ,CALCULATIONACTIVE // bool
} PoolProperty;

class CalculatorPool : private CompactArray<MBCalculator*>, public PropertyContainer {
private:
  static UINT           s_CPUCount;
  static CalculatorSet  s_maxSet;
  static const TCHAR   *s_stateName[3];
  MBContainer          &m_mbc;
  mutable Semaphore     m_gate;
  ULONG                 m_pendingFlags;
  ULONG                 m_suspendAllPendingFlags;
  ULONG                 m_killAllPendingFlags;
  CalculatorSet         m_calculatorsInState[3]; // one for each possible value of CalculatorState
  CalculatorSet         m_existing;

  void            setStateInternal(UINT id, CalculatorState state);
  CalculatorState getStateInternal(UINT id) const;
  void            killAllInternal();
  void            wakeAllInternal();
  void            waitUntilAllTerminated();
  void            waitUntilNoRunningInternal();
  void            waitUntilAllTerminatedInternal();
  void            addToExistingInternal(UINT id);
  void            clearExistingInternal();
  static inline const TCHAR *getStateName(CalculatorState state) {
    return s_stateName[state];
  }
  void notifyIfChanged(bool          oldIsActive  );
  void notifyIfChanged(CalculatorSet oldRunningSet);
  void createAllPendinglMasks();

#ifdef SAVE_CALCULATORINFO
  Array<CalculatorInfo> m_infoArray;
#endif

public:
  CalculatorPool(MBContainer &mbc);
 ~CalculatorPool();
  MBContainer &getMBContainer() {
    return m_mbc;
  }
  static inline UINT getCPUCount() {
    return s_CPUCount;
  }
  CalculatorState getState(UINT id);
  void            setState(UINT id, CalculatorState state);
  String          getStatesString() const;

  ULONG           getPendingMask(UINT id) {
    return 3 << (2*id);
  }
  inline ULONG getPendingFlags(UINT id) {
    return (m_pendingFlags >> (2*id)) & 3;
  }
  inline bool isPending(ULONG mask) {
    return (m_pendingFlags & mask) != 0;
  }

  inline CalculatorSet getCalculatorsInState(CalculatorState state) const {
    return m_calculatorsInState[state];
  }
  void startCalculators(UINT count);
  inline bool isCalculationActive() const {
    return m_calculatorsInState[CALC_TERMINATED] < m_existing;
  }
  // Return sum of pixels done by all threads together
  size_t getDoneCount() const;
  double getTimeUsed() const;
  void suspendCalculation();
  void resumeCalculation();
  void killAll();
  void waitUntilNoRunning();

#ifdef SAVE_CALCULATORINFO
  void addCalculatorInfo(const CalculatorInfo &info);
  const CalculatorInfo *findInfo(const CPoint &p) const;
#endif
};
