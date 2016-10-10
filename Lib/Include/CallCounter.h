#pragma once

#include "HashMap.h"

class CallCounter {
private:
  const String             m_name;
  IntHashMap<UINT>         m_map;
  UINT                     m_callCount;
  UINT                     m_maxInt;
  double                   m_maxDouble;
public:
  CallCounter(const String &name) : m_name(name), m_callCount(0), m_maxInt(0), m_maxDouble(0) {
  }
  ~CallCounter();
  void incr(int mapKey);
  inline void incr() {
    m_callCount++;
  }
  inline void count(UINT n) {
    m_callCount += n;
  }
  inline void updateMax(UINT v) {
    m_callCount++;
    if(v > m_maxInt) m_maxInt = v;
  }
  inline void updateMax(double v) {
    m_callCount++;
    if(v > m_maxDouble) m_maxDouble = v;
  }
  String mapToString();
};
