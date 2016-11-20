#include "stdafx.h"
#include <io.h>
#include "EndGameUtil.h"

// _________________________________________ 1.4 _________________________________________

const char *TablebaseInfo::s_programVersion = "1.4";

void TablebaseInfo::clear() {
  memset(this, 0, sizeof(TablebaseInfo));
}

void TablebaseInfo::save(ByteOutputStream &s) const {
  memset(m_version, 0, sizeof(m_version));
  strcpy(m_version, s_programVersion);
  s.putBytes((BYTE*)this, sizeof(TablebaseInfo));
}

void TablebaseInfo::load(ByteInputStream &s) {
  s.getBytesForced((BYTE*)this, sizeof(TablebaseInfo));
  if(getVersion() != getProgramVersion()) {
    throw WrongVersionException(format(_T("Wrong fileversion:%s. Programversion=%s"), getVersion().cstr(), getProgramVersion().cstr()).cstr());
  }
}

#define WBWIDTH(n) (2*(n)+1)

#define POSITIONWIDTH    14
#define INDEXSIZEWIDTH   14
#define UNDEFINEDWIDTH   11
#define STALEMATEWIDTH   10
#define MATEPOSWIDTH      9
#define WINNERPOSWIDTH   14
#define TERMPOSWIDTH     11
#define NTPOSWIDTH       14
#define MAXVARWIDTH       7
#define VERSIONWIDTH      5

static const TCHAR *POSITIONSTR     = _T("Positions");
static const TCHAR *INDEXSIZESTR    = _T("Indexsize");
static const TCHAR *UNDEFINEDSTR    = _T("Undefined");
static const TCHAR *STALEMATESTR    = _T("Stalemates");
static const TCHAR *MATEPOSSTR      = _T("Checkmates");
static const TCHAR *TERMPOSSTR      = _T("Terminal positions");
static const TCHAR *NTPOSSTR        = _T("Non term positions");
static const TCHAR *MAXVARSTR       = _T("Max var");
static const TCHAR *VERSIONSTR      = _T("Vers");
static const TCHAR *WINNERPOSSTR    = _T("Winnerpositions");

#define FILLER(l) spaceString(l,_T(' ')).cstr()

static String wbHeader1(const TCHAR *title, int fieldWidth) {
  const int titleWidth = WBWIDTH(fieldWidth);
  String result = title;
  while (result.length() < titleWidth) {
    result += _T('-');
    if (result.length() < titleWidth) {
      result = _T("-") + result;
    }
  }
  return result;
}

static String wbHeader2(int fieldWidth) {
  return format(_T("%*s %*s"), fieldWidth, _T("W.win"), fieldWidth, _T("B.win"));
}

String TablebaseInfo::getColumnHeaders(TablebaseInfoStringFormat f, const String &headerLeft, const String &headerRight, bool plies) {   // static
  switch(f) {
  case TBIFORMAT_PRINT_COLUMNS1:
    { const String h1 = format(_T("%*s %*s %*s %*s %s %s %s %*s %-*s")
                              ,POSITIONWIDTH       , POSITIONSTR
                              ,INDEXSIZEWIDTH      , INDEXSIZESTR
                              ,UNDEFINEDWIDTH      , UNDEFINEDSTR
                              ,STALEMATEWIDTH      , STALEMATESTR
                              ,wbHeader1(MATEPOSSTR, MATEPOSWIDTH).cstr()
                              ,wbHeader1(TERMPOSSTR, TERMPOSWIDTH).cstr()
                              ,wbHeader1(NTPOSSTR  , NTPOSWIDTH  ).cstr()
                              ,MAXVARWIDTH         , MAXVARSTR
                              ,VERSIONWIDTH        , VERSIONSTR
                              );
      const String h2 = format(_T("%s %s %s %s %*s %s")
                              ,FILLER(POSITIONWIDTH + INDEXSIZEWIDTH + UNDEFINEDWIDTH + STALEMATEWIDTH + 3)
                              ,wbHeader2(MATEPOSWIDTH).cstr()
                              ,wbHeader2(TERMPOSWIDTH).cstr()
                              ,wbHeader2(NTPOSWIDTH  ).cstr()
                              ,MAXVARWIDTH , plies ? _T("(plies)") : _T("(moves)")
                              ,FILLER(VERSIONWIDTH)
                              );
    return format(
      _T("%s %s%s\n")
      _T("%s %s%s\n")
      ,headerLeft.cstr()            ,h1.cstr(), headerRight.cstr()
      ,FILLER(headerLeft.length())  ,h2.cstr(), FILLER(headerRight.length())
      );
    }
  case TBIFORMAT_PRINT_COLUMNS2:
   { const String h1 = format(_T("%*s %*s %*s %*s %s %*s %-*s")
                              ,POSITIONWIDTH  , POSITIONSTR
                              ,INDEXSIZEWIDTH , INDEXSIZESTR
                              ,UNDEFINEDWIDTH , UNDEFINEDSTR
                              ,STALEMATEWIDTH , STALEMATESTR
                              ,wbHeader1(WINNERPOSSTR, WINNERPOSWIDTH).cstr()
                              ,MAXVARWIDTH    , MAXVARSTR
                              ,VERSIONWIDTH   , VERSIONSTR
                              );
      const String h2 = format(_T("%s %s %*s %*s %s")
                              ,FILLER(POSITIONWIDTH + INDEXSIZEWIDTH + UNDEFINEDWIDTH + STALEMATEWIDTH + 3)
                              ,wbHeader2(WINNERPOSWIDTH).cstr()
                              ,MAXVARWIDTH, plies ? _T("(plies)") : _T("(moves)")
                              ,FILLER(VERSIONWIDTH)
                              );
      return format(
      _T("%s %s%s\n")
      _T("%s %s%s\n")
      ,headerLeft.cstr()          , h1.cstr(), headerRight.cstr()
      ,FILLER(headerLeft.length()), h2.cstr(), FILLER(headerRight.length())
      );
    }
  default:
    return _T("");
  }
}

String TablebaseInfo::toString(TablebaseInfoStringFormat f, bool plies) const {
  switch(f) {
  case TBIFORMAT_PRINT_TERMINALS:
    return format(_T("Undef:%*s Checkmates:(%s) Stalemates:%*s Term:(%s)")
                 ,UNDEFINEDWIDTH, format1000(m_undefinedPositions      ).cstr()
                 ,m_checkMatePositions.toString(  _T(','), MATEPOSWIDTH).cstr()
                 ,STALEMATEWIDTH, format1000(m_stalematePositions      ).cstr()
                 ,m_terminalWinPositions.toString(_T(','), TERMPOSWIDTH).cstr()
                 );

  case TBIFORMAT_PRINT_NONTERMINALS:
    return format(_T("Pos:%*s Undef:%*s (%5.2lf%%) Term:(%s) Win:(%s) MaxVar:%s")
                 ,POSITIONWIDTH , format1000(m_totalPositions    ).cstr()
                 ,UNDEFINEDWIDTH, format1000(m_undefinedPositions).cstr()
                 ,PERCENT(m_undefinedPositions, m_totalPositions )
                 ,(m_checkMatePositions + m_terminalWinPositions ).toString(_T(','), WINNERPOSWIDTH).cstr()
                 ,m_nonTerminalWinPositions.toString(_T(','), NTPOSWIDTH).cstr()
                 ,formatMaxVariants(plies).cstr()
                 );

  case TBIFORMAT_PRINT_UNDEFANDWINS:
    return format(_T("Pos:%*s Undef:%*s (%5.2lf%%) Win:(%s)")
                 ,POSITIONWIDTH,  format1000(m_totalPositions    ).cstr()
                 ,UNDEFINEDWIDTH, format1000(m_undefinedPositions).cstr()
                 ,PERCENT(m_undefinedPositions, m_totalPositions )
                 ,getWinnerPositionCount().toString(_T(','), WINNERPOSWIDTH).cstr()
                 );

  case TBIFORMAT_PRINT_COLUMNS1:
    return format(_T(" %*s %*s %*s %*s %s %s %s %s %-*s")
                 ,POSITIONWIDTH , format1000(m_totalPositions    ).cstr()
                 ,INDEXSIZEWIDTH, format1000(m_indexCapacity     ).cstr()
                 ,UNDEFINEDWIDTH, format1000(m_undefinedPositions).cstr()
                 ,STALEMATEWIDTH, format1000(m_stalematePositions).cstr()
                 ,m_checkMatePositions.toString(     _T(' '), MATEPOSWIDTH).cstr()
                 ,m_terminalWinPositions.toString(   _T(' '), TERMPOSWIDTH).cstr()
                 ,m_nonTerminalWinPositions.toString(_T(' '), NTPOSWIDTH  ).cstr()
                 ,formatMaxVariants(plies).cstr()
                 ,VERSIONWIDTH, format(_T("%s%s"), getVersion().cstr(), isConsistent()?_T("*"):format(_T("%#02x"), m_stateFlags).cstr()).cstr()
                 );
  case TBIFORMAT_PRINT_COLUMNS2:
    return format(_T(" %*s %*s %*s %*s %s %s %-*s")
                 ,POSITIONWIDTH , format1000(m_totalPositions    ).cstr()
                 ,INDEXSIZEWIDTH, format1000(m_indexCapacity     ).cstr()
                 ,UNDEFINEDWIDTH, format1000(m_undefinedPositions).cstr()
                 ,STALEMATEWIDTH, format1000(m_stalematePositions).cstr()
                 ,getWinnerPositionCount().toString(_T(' '), WINNERPOSWIDTH).cstr()
                 ,formatMaxVariants(plies).cstr()
                 ,VERSIONWIDTH, format(_T("%s%s"), getVersion().cstr(), isConsistent()?_T("*"):format(_T("%#02x"), m_stateFlags).cstr()).cstr()
                 );
  default:
    return format(_T("Unknown print-format:%d"), f);
  }
}

PositionCount64 TablebaseInfo::getWinnerPositionCount() const {
  return m_checkMatePositions + m_terminalWinPositions + m_nonTerminalWinPositions;
}

UINT64 TablebaseInfo::getWinnerPositionCount(Player winner) const {
  return m_checkMatePositions.m_count[winner]
       + m_terminalWinPositions.m_count[winner]
       + m_nonTerminalWinPositions.m_count[winner];
}

#define FORMATMAXVARIANT(v,plies) ((v) ? format(_T("%3d"), plies ? v : PLIESTOMOVES(v)).cstr() : _T("---"))

String TablebaseInfo::formatMaxVariants(bool plies) const {
  return format(_T("%3s/%3s")
               ,FORMATMAXVARIANT(m_maxPlies.m_count[WHITEPLAYER], plies)
               ,FORMATMAXVARIANT(m_maxPlies.m_count[BLACKPLAYER], plies)
               );
}

//----------------------------------------------------------------------------------------------------
class MoveComparator : public Comparator<MoveWithResult> {
private:
  const Player m_playerInTurn;

  bool isWinnerMove(const MoveWithResult &m) const {
    return m.m_result.getStatus() == EG_WHITEWIN && m_playerInTurn == WHITEPLAYER
        || m.m_result.getStatus() == EG_BLACKWIN && m_playerInTurn == BLACKPLAYER;
  }
  bool isDrawMove(const MoveWithResult &m) const {
    return m.m_result.getStatus() == EG_DRAW || m.m_result.getStatus() == EG_UNDEFINED;
  }
  int positionCompare(const MoveWithResult &m1, const MoveWithResult &m2) const;
public:
  MoveComparator(Player playerInTurn) : m_playerInTurn(playerInTurn) {
  }
  int compare(const MoveWithResult &m1, const MoveWithResult &m2);
  AbstractComparator *clone() const {
    return new MoveComparator(m_playerInTurn);
  }

};

int MoveComparator::compare(const MoveWithResult &m1, const MoveWithResult &m2) {
  const bool m1Win = isWinnerMove(m1);
  const bool m2Win = isWinnerMove(m2);
  if(m1Win) {
    if(m2Win) {
      const int c = m1.m_result.getMovesToEnd() - m2.m_result.getMovesToEnd();
      return c ? c : positionCompare(m1,m2);
    } else {
      return -1;
    }
  } else if(m2Win) { // && !m1Win
    return 1;
  } else { // m1Win == false && m2Win == false
    const bool m1Draw = isDrawMove(m1);
    const bool m2Draw = isDrawMove(m2);
    if(m1Draw) {
      if(m2Draw) {
        return positionCompare(m1,m2);
      } else { // m1 draw && m2 loose
        return -1;
      }
    } else if(m2Draw) { // m1 Loose
      return 1;
    } else { // m1 loose && m2 loose
      const int c = m2.m_result.getMovesToEnd() - m1.m_result.getMovesToEnd();
      return c ? c : positionCompare(m1,m2);
    }
  }
}

int MoveComparator::positionCompare(const MoveWithResult &m1, const MoveWithResult &m2) const {
  int c = m1.getFrom() - m2.getFrom();
  if(c) return c;
  c = m1.getTo() - m2.getTo();
  if(c) return c;
  return pieceTypeCmp(m1.getPromoteTo(), m2.getPromoteTo());
}

CompactIntArray MoveResultArray::findShortestWinnerMoves() const {
  CompactIntArray tmp;
  int minWinDistance = 9999;
  const EndGamePositionStatus usableStatus = (m_playerInTurn==WHITEPLAYER) ? EG_WHITEWIN : EG_BLACKWIN;
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult &m = (*this)[i];
    if(m.m_result.getStatus() != usableStatus) {
      tmp.add(-1);
    } else {
      const int pliesToEnd = m.m_result.getPliesToEnd();
      tmp.add(pliesToEnd);
      if(pliesToEnd < minWinDistance) {
        minWinDistance = pliesToEnd;
      }
    }
  }
  CompactIntArray result;
  for(UINT i = 0; i < tmp.size(); i++) {
    if(tmp[i] == minWinDistance) {
      result.add(i);
    }
  }
  return result;
}

CompactIntArray MoveResultArray::findLongestLoosingMoves(int defendStrength) const {
  CompactIntArray tmp;
  int maxLooseDistance = 0;
  const EndGamePositionStatus usableStatus = (m_playerInTurn==WHITEPLAYER) ? EG_BLACKWIN : EG_WHITEWIN;
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult &m = (*this)[i];
    if(m.m_result.getStatus() != usableStatus) {
      tmp.add(-1);
    } else {
      const int pliesToEnd = m.m_result.getPliesToEnd();
      tmp.add(pliesToEnd);
      if(pliesToEnd > maxLooseDistance) {
        maxLooseDistance = pliesToEnd;
      }
    }
  }
  const int minLooseDistance = maxLooseDistance * defendStrength / 100;

  CompactIntArray result;
  for(UINT i = 0; i < tmp.size(); i++) {
    int p = tmp[i];
    if((p >= 0) && (p >= minLooseDistance)) {
      result.add(i);
    }
  }
  return result;
}

CompactIntArray MoveResultArray::findDrawMoves() const {
  CompactIntArray result;
  for(UINT i = 0; i < size(); i++) {
    if((*this)[i].m_result.getStatus() == EG_DRAW) {
      result.add(i);
    }
  }
  return result;
}

MoveResultArray &MoveResultArray::sort() {
  CompactArray<MoveWithResult>::sort(MoveComparator(m_playerInTurn));
  return *this;
}

MoveWithResult MoveResultArray::selectBestMove(int defendStrength) const {
  CompactIntArray moves = findShortestWinnerMoves();
  if(!moves.isEmpty()) {
    return (*this)[moves.select()];
  } else if(!(moves = findDrawMoves()).isEmpty()) {
    return (*this)[moves.select()];
  } else {
    return selectLongestLoosingMove(defendStrength);
  }
}


String MoveResultArray::toString(const Game &game, MoveStringFormat mf, bool depthInPlies) {
  sort();
  StringArray msa;
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult &mr = (*this)[i];
    const ExecutableMove em = game.generateMove(mr.getFrom(), mr.getTo(), mr.getPromoteTo());
    msa.add(em.toString(mf));
  }
  const int maxMoveStrLength = (int)msa.maxLength();
  String result;
  for(size_t i = 0; i < size(); i++) {
    result += format(_T("%-*s - %s\n"), maxMoveStrLength, msa[i].cstr(), (*this)[i].m_result.toString(m_playerInTurn, depthInPlies).cstr());
  }
  return result;
}

StreamProgress::StreamProgress(const EndGameTablebase *tb) : m_timer(1) {
  m_total     = sizeof(TablebaseInfo) + tb->getKeyDefinition().getIndexSize() * sizeof(EndGameResult);
  m_byteCount = 0;
  m_timer.startTimer(1500, *this, true);
}

StreamProgress::StreamProgress(const String &fileName) : m_timer(1) {
  m_total     = STAT(fileName).st_size;
  m_byteCount = 0;
  m_timer.startTimer(1500, *this, true);
}

StreamProgress::~StreamProgress() {
  m_timer.stopTimer();
  verbose(_T("        \b")); // clear the decimals written in hanbdleTimeout
  verbose(_T("100%%\n"));
}

void StreamProgress::handleTimeout(Timer &timer) {
  verbose(_T("%.2lf%%\b"), PERCENT(m_byteCount, m_total));
}

#ifdef TABLEBASE_BUILDER

String secondsToString(double msec, bool showMilliSeconds) {
  if(msec < 60000) {
    if(showMilliSeconds) {
      return format(_T("%.3lf sec."), msec / 1000.0);
    } else {
      return format(_T("%.0lf sec."), msec / 1000.0);
    }
  } else {
    const int seconds = (int)(msec / 1000);
    if(seconds < 3600) {
      return format(_T("%02d:%02d min."), seconds/60, seconds%60);
    } else if(seconds < 24*3600) {
      return format(_T("%02d:%02d:%02d"), seconds/3600, (seconds/60) % 60, seconds % 60);
    } else {
      const int days    =  seconds / (3600*24);
      const int hours   = (seconds / 3600) % 24;
      const int minutes = (seconds / (24*60)) % 60;
      const int sec     = seconds % 60;
      return format(_T("%d days, %02d:%02d:%02d hours"), days, hours, minutes, sec);
    }
  }
}

IntervalChecker::IntervalChecker(const TCHAR *name) : m_name(name) {
  m_minValue = -1;
  m_maxValue = 0;
}

IntervalChecker::~IntervalChecker() {
  _tprintf(_T("Interval[%s]:[%s..%s]\n"), m_name, format1000(m_minValue).cstr(), format1000(m_maxValue).cstr());
}


void IntervalChecker::update(EndGamePosIndex v) {
  if(v < m_minValue) {
    m_minValue = v;
  }
  if(v > m_maxValue) {
    m_maxValue = v;
  }
}

#endif
