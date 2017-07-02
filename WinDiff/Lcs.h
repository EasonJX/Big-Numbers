#pragma once

#include <CompactArray.h>
#include <Semaphore.h>
#include <StringPool.h>
#include "CompareJob.h"

#ifdef MEASURE_STEPTIME

class TimeMeasure {
private:
  const int    m_id;
  const TCHAR *m_label;
  const double m_startTime;
public:
  TimeMeasure(int id, const TCHAR *label) : m_id(id), m_label(label), m_startTime(getProcessTime()) {
  }
  double getUsedTime() const {
    return getProcessTime() - m_startTime;
  }
  const TCHAR *getLabel() const {
    return m_label;
  }
};

#define RESET_TIMEMEASURE(str)                                              \
{ const String tmp = str;                                                   \
  redirectDebugLog(true);                                                   \
  debugLog(_T("%s\n"), tmp.cstr());                                         \
}

#define BEGIN_TIMEMEASURE(id, label )                                       \
  TimeMeasure _timeMeasure##id(id, label);                                  \
  if(m_job) { debugLog(_T("JobStep:%s\n"), m_job->getCurrentStep().m_msg);} \

#define END_TIMEMEASURE(  id, divide)                                       \
{ const double _usedTime = _timeMeasure##id.getUsedTime();                  \
  const double _d        = divide;                                          \
  debugLog(_T("%2d:%-30s:time:%11.0lf, divide:%11.0lf, factor:%.3le\n")     \
          ,id                                                               \
          ,_timeMeasure##id.getLabel()                                      \
          ,_usedTime                                                        \
          ,_d                                                               \
          ,_usedTime/_d);                                                   \
}

#else

#define RESET_TIMEMEASURE(str)
#define BEGIN_TIMEMEASURE(tm, label )
#define END_TIMEMEASURE(  tm, divide)

#endif

#define SPERCENT(i,n) ((unsigned short)PERCENT(i,n))

class LineArray {
private:
  CompactIntArray m_offset;
  StringPool     *m_stringPool;
  const bool      m_ownStringPool;
public:
  LineArray();
  LineArray(StringPool &stringPool);
  virtual ~LineArray();

  inline const TCHAR *operator[](size_t index) const {
    return m_stringPool->getString(m_offset[index]);
  }
  inline void add(const TCHAR *s) {
    m_offset.add((int)m_stringPool->addString(s));
  }

  void clear();

  inline size_t size() const {
    return m_offset.size();
  }
  void setLineCapacity(size_t lineCount);
  void setIndexCapacity(size_t textCapacity, size_t lineCount);
  inline size_t getTextCapacity() const {
    return m_stringPool->getTextCapacity();
  }
  inline const StringPool &getStringPool() const {
    return *m_stringPool;
  }
  void updateCapacity(); // called when we are done adding lines

  void dump(const String &fname) const;

};

class LcsElement {
public:
  const TCHAR *m_s;
  int          m_index;
  inline LcsElement() {
  }
  inline LcsElement(int index, const TCHAR *s) : m_s(s) {
    m_index = index;
  }
};

class Link {
public:
  int   m_i,m_j;
  Link *m_next;
};

#define LINKPAGESIZE 30000

class LinkPage {
private:
  int  m_count;
  Link m_page[LINKPAGESIZE];
public:
  LinkPage *m_next;
  inline LinkPage(LinkPage *next) {
    m_count = 0;
    m_next  = next;
  }
  inline Link *fetchLink(int i, int j, Link *next) {
    Link *result   = m_page + m_count++;
    result->m_i    = i;
    result->m_j    = j;
    result->m_next = next;
    return result;
  }

  inline bool isFull() const {
    return m_count == ARRAYSIZE(m_page);
  }
};

class ElementPair {
public:
  int m_aIndex, m_bIndex;
  inline ElementPair(int a, int b) {
    m_aIndex = a;
    m_bIndex = b;
  }
  inline ElementPair() {
  }
  String toString() const {
    return format(_T("(%d,%d)"), m_aIndex, m_bIndex);
  }
};

typedef CompactArray<ElementPair> ElementPairArray;

class LcsComparator : public Comparator<const TCHAR*> {
public:
  virtual int compare(const TCHAR * const &s1, const TCHAR * const &s2) = 0;
};

extern LcsComparator *stdLcsComparator;

class IndexComparator : public Comparator<LcsElement> {
protected:
  LcsComparator         &m_c;
  InterruptableRunnable *m_runnable;
  size_t                 m_compareCount;
  inline void incrCompareCount() {
    if((++m_compareCount & 0x3ff) == 0) {
      if(m_runnable) {
        m_runnable->checkInterruptAndSuspendFlags();
      }
    }
  }
public:
  IndexComparator(LcsComparator &c) : m_c(c), m_runnable(NULL) {
    m_compareCount = 0;
  }
  inline void setRunnable(InterruptableRunnable *runnable) {
    m_runnable = runnable;
  }
  AbstractComparator *clone() const {
    return new IndexComparator(m_c);
  }
  int compare(const LcsElement &e1, const LcsElement &e2);
  inline size_t getCompareCount() const {
    return m_compareCount;
  }
};

class IndexComparatorR : public IndexComparator {
public:
  inline IndexComparatorR(LcsComparator &c) : IndexComparator(c) {
  }
  AbstractComparator *clone() const {
    return new IndexComparatorR(m_c);
  }
  int compare(const LcsElement &e1, const LcsElement &e2);
};

class Lcs {
private:
  LinkPage              *m_firstLinkPage;
protected:
  size_t                 m_n;           // Number of elements in longest input-Array
  bool                   m_seqReversed; // true if b (parameter to constructor) is longest, and a and b are swapped
  LcsComparator         &m_cmp;
  UINT                  *m_tresh;
  Link                 **m_link;
  CompareJob            *m_job;
  size_t                 m_linkPageCount;

  inline Link *newLink(int i, int j, Link *next) {
    if(m_firstLinkPage->isFull()) { m_firstLinkPage = new LinkPage(m_firstLinkPage); m_linkPageCount++; }
    return m_firstLinkPage->fetchLink(i, j, next);
  }

  void            clear();
  void            dumpThresh();
  void            stopAndThrow();
public:
  Lcs(LcsComparator &cmp, CompareJob *job);
  Lcs(const Lcs &src);            // not defined
  Lcs &operator=(const Lcs &src); // not defined

  virtual ~Lcs();
  virtual void findLcs(ElementPairArray &p) = 0;
};

inline double nlogn(double x) {
  return (x <= 0) ? 0 : (x * log(x));
}

#define CHECK_INTERVAL 0x3fff
#define NL _T('\n')
