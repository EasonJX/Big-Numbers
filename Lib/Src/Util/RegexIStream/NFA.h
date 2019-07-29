#pragma once

#include "CompactArray.h"
#include "NFAState.h"

class NFA : public CompactArray<NFAState*> {
private:
  NFAStatePool &m_statePool;
  int           m_idCounter;

  NFA(const NFA &src);      // Not defined. Class not cloneable
  NFA &operator=(NFA &src); // Not defined. Class not cloneable

  void        addIfNotMarked(NFAState *s);
  static void unmarkAll(     NFAState *s);
  friend class PatternParser;
public:
  NFA(NFAStatePool &statePool) : m_statePool(statePool) {}
  NFA(NFAStatePool &statePool, NFAState *start);
  ~NFA(); // All elements in array will be released
          // clear and remove will NOT release the elements
  void create(NFAState *start);
};
