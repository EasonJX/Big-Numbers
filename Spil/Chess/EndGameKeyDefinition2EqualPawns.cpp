#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

EndGameKeyDefinition2EqualPawns::EndGameKeyDefinition2EqualPawns(PieceKey pk23)
: EndGameKeyDefinitionDupletsAllowed(pk23) {
#ifdef TABLEBASE_BUILDER
  setPawnsOwner();
#endif
}

EndGameKeyDefinition2EqualPawns::EndGameKeyDefinition2EqualPawns(PieceKey pk2, PieceKey pk34)
: EndGameKeyDefinitionDupletsAllowed(pk2, pk34)
{
#ifdef TABLEBASE_BUILDER
  setPawnsOwner();
#endif
}

#ifdef TABLEBASE_BUILDER
void EndGameKeyDefinition2EqualPawns::setPawnsOwner() {
  switch(getPieceCount()) {
  case 4 :
    { checkIsPawn(2, true);
      checkIsPawn(3, true);
      checkSameOwner(2, 3, true);
      m_pawnsOwner.setOwner(0, getPieceOwner(2));
    }
    break;
  case 5 :
    { checkIsPawn(2, false);
      checkIsPawn(3, true );
      checkIsPawn(4, true );
      checkSameOwner(3, 4, true);
      m_pawnsOwner.setOwner(0, getPieceOwner(3));
    }
    break;
  default:
    throwException(_T("totalPieceCount=%d"), getPieceCount());
  }
}
#endif

#ifdef _DEBUG

void set2EqualPawnsNoFlip(EndGameKey &key, unsigned long &addr, unsigned int maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = EndGameKeyDefinition::findRange2Equal(maxAddr, addr);
  addr -= GET_RANGESTART2EQUAL(maxAddr, r);
  r++;
  key.setPosition(hpIndex, EndGameKeyDefinition::pawnIndexToPos[r]);
  key.setPosition(lpIndex, EndGameKeyDefinition::pawnIndexToPos[(addr) % r]);
  addr /= r;
}

void set2EqualPawnsFlipi( EndGameKey &key, unsigned long &addr, unsigned int maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = EndGameKeyDefinition::findRange2Equal(maxAddr, addr);
  addr -= GET_RANGESTART2EQUAL(maxAddr, r);
  key.setPosition(hpIndex, EndGameKeyDefinition::pawnIndexToPos[r]);
  r++;
  const int lpPos = EndGameKeyDefinition::pawnIndexToPos[(addr) % r];
  key.setPosition(lpIndex, MIRRORCOLUMN(lpPos));
  addr /= r;
}

void set2EqualPawnsFlipj( EndGameKey &key, unsigned long &addr, unsigned int maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = EndGameKeyDefinition::findRange2Equal(maxAddr, addr);
  addr -= GET_RANGESTART2EQUAL(maxAddr, r);
  r++;
  key.setPosition(hpIndex, MIRRORCOLUMN(EndGameKeyDefinition::pawnIndexToPos[r]));
  key.setPosition(lpIndex, EndGameKeyDefinition::pawnIndexToPos[(addr) % r]);
  addr /= r;
}

void set2EqualPawnsFlipij(EndGameKey &key, unsigned long &addr, unsigned int maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = EndGameKeyDefinition::findRange2Equal(maxAddr, addr);
  addr -= GET_RANGESTART2EQUAL(maxAddr, r);
  r++;
  int pos = EndGameKeyDefinition::pawnIndexToPos[r];
  key.setPosition(hpIndex, MIRRORCOLUMN(pos));
  pos = EndGameKeyDefinition::pawnIndexToPos[(addr) % r];
  key.setPosition(lpIndex, MIRRORCOLUMN(pos));
  addr /= r;
}

#endif
