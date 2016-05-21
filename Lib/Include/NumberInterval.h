#pragma once

#include "MyUtil.h"

template <class T> class NumberInterval {
private:
  T m_from, m_to;
public:
  NumberInterval(const T &from, const T &to) {
    m_from = from;
    m_to   = to;
  }
  
  NumberInterval() {
    m_from = m_to = 0;
  }
  
  const T &getFrom() const {
    return m_from;
  }
  
  const T &getTo()   const {
    return m_to;
  }
  
  NumberInterval<T> &setFrom(const T &v) {
    m_from = v;
    return *this;
  }
  
  NumberInterval<T> &setTo(const T &v) {
    m_to   = v;
    return *this;
  }
  
  const T getMin() const {
    return m_from < m_to ? m_from : m_to;
  }
  
  const T getMax() const {
    return m_from > m_to ? m_from : m_to;
  }
  
  bool contains(const T &x) const {
    return getMin() <= x && x <= getMax();
  }
  
  bool contains(const NumberInterval<T> &i) const {
    return getMin() <= i.getMin() && i.getMax() <= getMax();
  }

  bool overlap(const NumberInterval<T> &i) const {
    return getMin() <= i.getMax() && getMax() >= i.getMin();
  }
  
  NumberInterval<T> interSection(const NumberInterval<T> &i) const {
    if(!overlap(i)) {
      return NumberInterval<T>(T(0), T(0));
    } else {
      return NumberInterval<T>(max(getMin(), i.getMin()), min(getMax(), i.getMax()));
    }
  }

  T getLength() const {
    return m_to - m_from;
  }
};

typedef NumberInterval<double> DoubleInterval;
typedef NumberInterval<int>    IntInterval;
