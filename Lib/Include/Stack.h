#pragma once

#include "Array.h"

template <typename T> class Stack : private Array<T> {
public:
  void push(const T &v) {
    add(v);
  }

  T pop() {
    if(isEmpty()) {
      throwException(_T("Stack::pop:Stack is empty"));
    }
    T e = last();
    removeLast();
    return e;
  }

  T &top(UINT index = 0) {
    const int i = (int)size() - 1 - index;
    if(i < 0) {
      throwException(_T("Cannot get element %u of stack with height %u"), index, getHeight());
    }
    return (*this)[i];
  }

  const T &top(UINT index = 0) const {
    const int i = (int)size() - 1 - index;
    if(i < 0) {
      throwException(_T("Cannot get element %u of stack with height %u"), index, getHeight());
    }
    return (*this)[i];
  }

  inline UINT getHeight() const {
    return (UINT)size();
  }

  inline void clear() {
    __super::clear();
  }

  inline bool isEmpty() const {
    return size() == 0;
  }

  String toString() const {
    String result;
    for(UINT i = 0; i < getHeight(); i++) {
      result += top(i).toString() + _T("\n");
    }
    return result;
  }
};
