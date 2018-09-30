#include "pch.h"

class ACosConstants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;
  const BigReal     &c3;

  ACosConstants()
    :c1( 0.2)
    ,c2( 0.9)
    ,c3(BIGREAL_HALF)
  {
  }
};

static const ACosConstants ACOSC;

BigReal acos(const BigReal &x, const BigReal &f) {
  VALIDATETOLERANCE(f)

  DigitPool *pool = x.getDigitPool();

  if(x == pool->get1()) {
    return pool->get0();
  }

  return pi(APCprod(<,f,ACOSC.c1,pool), pool)*ACOSC.c3 - asin(x,APCprod(<,f,ACOSC.c2,pool));
}
