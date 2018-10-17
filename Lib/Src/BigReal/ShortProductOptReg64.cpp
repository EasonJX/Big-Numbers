#include "pch.h"

#if(SP_OPT_METHOD == SP_OPT_BY_REG64)

#ifdef IS32BIT
#error SP_OPT_BY_REG64 cannot be used in x86-mode
#endif

#if(BIGREALBASE != 1000000000000000000)
#error For SP_OPT_METHOD == SP_OPT_BY_REG64 BIGREALBASE must be 1e18
#endif

// return 1 if sum of products has been added. 0 if we need to do it in C-code. if so, the sum is returned in bigsum
extern "C" int BigRealMultiplyColumn(const Digit *yk, const Digit *xk, Digit *dst, BR2DigitType &bigSum);

static const BR2DigitType BIGREALBASE128(BIGREALBASE);

DECLARE_CALLCOUNTER(shortProdCallTotal);
DECLARE_CALLCOUNTER(shortProdLoopTotal);
DECLARE_CALLCOUNTER(shortProdSumTooBig);

// assume x != 0 and y != 0. and loopCount > 0
BigReal &BigReal::shortProductNoZeroCheck(const BigReal &x, const BigReal &y, size_t loopCount) { // return *this

  COUNTCALL(shortProdCallTotal);

  intptr_t         loopCounter = loopCount;
  Digit           *cd          = clearDigits1();
  BR2DigitType     bigSum;
  m_expo = m_low = x.m_expo + y.m_expo;

  for(const Digit *xk = x.m_first, *yk = y.m_first;;) { // loopcondition at the end
    cd = fastAppendDigit(cd);
    COUNTCALL(shortProdLoopTotal);

    if(!BigRealMultiplyColumn(yk, xk, cd, bigSum)) {
      COUNTCALL(shortProdSumTooBig);
      BRDigitType carry = 0;
      Digit      *d     = cd;
      d->n = bigSum % BIGREALBASE128;
      bigSum /= BIGREALBASE128;
      do {
        d = d->prev;
        carry  += d->n + (BRDigitType)(bigSum % BIGREALBASE128);
        d->n   = carry % BIGREALBASE;
        carry  /= BIGREALBASE;
        bigSum /= BIGREALBASE;
      } while(bigSum || carry);
      // dont call trimZeroes() !!;
    }

    if(--loopCounter <= 0) break;
    if(yk->next) {
      yk = yk->next;
    } else if(!(xk = xk->next)) {
      break; // we are done
    }
  }

  intptr_t digitsAdded = loopCount - loopCounter;
  if(cd->n == 0) { // Fixup both ends of digit chain, to reestablish invariant (See comments in assertIsValidBigReal)
    m_last = cd;
    for(digitsAdded--, cd = m_last->prev; cd->n == 0; cd = cd->prev, digitsAdded--);
    deleteDigits(cd->next, m_last);
  }
  m_low -= digitsAdded - 1;
  (m_last = cd)->next = NULL;

  if(m_first->n == 0) {
    cd = m_first;
    (m_first = m_first->next)->prev = NULL;
    deleteDigits(cd,cd);
  } else {
    m_first->prev = NULL;
    m_expo++;
  }
  return setSignByProductRule(x, y);
}

#define SPLIT_LENGTH 240

size_t BigReal::s_splitLength = SPLIT_LENGTH; // Value found by experiments with measureSplitFactor in testnumber.cpp

#define MAX_DIGITVALUE (BIGREALBASE-1)

UINT BigReal::getMaxSplitLength() { // static
  return 340; // floor(_UI128_MAX / (MAX_DIGITVALUE * MAX_DIGITVALUE))
}

#endif // SP_OPT_METHOD == SP_OPT_BY_REG32
