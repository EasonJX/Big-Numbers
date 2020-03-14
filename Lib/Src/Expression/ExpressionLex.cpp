/* DO NOT EDIT THIS FILE - it is machine generated */

#line 32 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
#include "pch.h"
#include <Math/Expression/ExpressionSymbol.h>
#include <CompactHashMap.h>
#include "ExpressionLex.h"

namespace Expr {

#line 9 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"

// DFA State   0 [nonAccepting]
//   goto  1 on \x00\x01\x02\x03\x04\x05\x06\x07\b\t\n\x0b\r\s
//   goto  2 on !
//   goto  3 on %
//   goto 29 on &
//   goto  4 on (
//   goto  5 on )
//   goto  6 on *
//   goto  7 on +
//   goto  8 on ,
//   goto  9 on -
//   goto 31 on .
//   goto 10 on /
//   goto 11 on 0123456789
//   goto 12 on ;
//   goto 13 on <
//   goto 14 on =
//   goto 15 on >
//   goto 16 on ABCDEFGHIJKLMNOPQRSTUVWXYZ
//   goto 17 on [
//   goto 18 on ]
//   goto 19 on ^
//   goto 16 on _abcdefghijklmnopqrstuvwxyz
//   goto 33 on |
// DFA State   1 [accepting, line 100 <;>]
//   goto  1 on \x00\x01\x02\x03\x04\x05\x06\x07\b\t\n\x0b\r\s
// DFA State   2 [accepting, line  88 <return NOT;>]
//   goto 20 on =
// DFA State   3 [accepting, line  79 <return MOD;>]
// DFA State   4 [accepting, line  82 <return LPAR;>]
// DFA State   5 [accepting, line  83 <return RPAR;>]
// DFA State   6 [accepting, line  77 <return PROD;>]
//   goto 19 on *
// DFA State   7 [accepting, line  75 <return PLUS;>]
// DFA State   8 [accepting, line  81 <return COMMA;>]
// DFA State   9 [accepting, line  76 <return MINUS;>]
// DFA State  10 [accepting, line  78 <return QUOT;>]
//   goto 22 on *
//   goto 23 on /
// DFA State  11 [accepting, line  48 <return NUMBER;>]
//   goto 30 on .
//   goto 11 on 0123456789
//   goto 34 on Ee
// DFA State  12 [accepting, line  97 <return SEMI;>]
// DFA State  13 [accepting, line  90 <return LT;>]
//   goto 24 on =
//   goto 25 on >
// DFA State  14 [accepting, line  96 <return ASSIGN;>]
//   goto 26 on =
// DFA State  15 [accepting, line  92 <return GT;>]
//   goto 27 on =
// DFA State  16 [accepting, line  99 <return nameOrKeyWord((_TUCHAR*)getText());>]
//   goto 16 on 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz
// DFA State  17 [accepting, line  84 <return LB;>]
// DFA State  18 [accepting, line  85 <return RB;>]
// DFA State  19 [accepting, line  80 <return POW;>]
// DFA State  20 [accepting, line  94 <return NE;>]
// DFA State  21 [accepting, line  86 <return AND;>]
// DFA State  22 [accepting, line  50 <{ int i;   const SourcePosition startpos = getPos(>]
// DFA State  23 [accepting, line  65 <{ int i;   while(i = input()) {     if(i < 0) {   >]
// DFA State  24 [accepting, line  91 <return LE;>]
// DFA State  25 [accepting, line  95 <return NE;>]
// DFA State  26 [accepting, line  89 <return EQ;>]
// DFA State  27 [accepting, line  93 <return GE;>]
// DFA State  28 [accepting, line  87 <return OR;>]
// DFA State  29 [nonAccepting]
//   goto 21 on &
// DFA State  30 [accepting, line  48 <return NUMBER;>]
//   goto 30 on 0123456789
//   goto 34 on Ee
// DFA State  31 [nonAccepting]
//   goto 30 on 0123456789
// DFA State  32 [accepting, line  48 <return NUMBER;>]
//   goto 32 on 0123456789
// DFA State  33 [nonAccepting]
//   goto 28 on |
// DFA State  34 [nonAccepting]
//   goto 35 on +-
//   goto 32 on 0123456789
// DFA State  35 [nonAccepting]
//   goto 32 on 0123456789

// The lexCharMap[] and lexStateMap arrays are used as follows:
//
// nextState = lexNext[lexStateMap[currentState]][lexCharMap[inputChar]];
//
// Character positions in the lexCharMap Array are:
//
//  \x00 \x01 \x02 \x03 \x04 \x05 \x06 \x07 \b   \t   \n   \x0b \f   \r   \x0e \x0f
//  \x10 \x11 \x12 \x13 \x14 \x15 \x16 \x17 \x18 \x19 \x1a \x1b \x1c \x1d \x1e \x1f
//  \s   !    "    #    $    %    &    \'   (    )    *    +    ,    -    .    /
//  0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
//  @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
//  P    Q    R    S    T    U    V    W    X    Y    Z    [    \\   ]    ^    _
//  `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
//  p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~    \x7f
//  \x80 \x81 \x82 \x83 \x84 \x85 \x86 \x87 \x88 \x89 \x8a \x8b \x8c \x8d \x8e \x8f
//  \x90 \x91 \x92 \x93 \x94 \x95 \x96 \x97 \x98 \x99 \x9a \x9b \x9c \x9d \x9e \x9f
//  \xa0 \xa1 \xa2 \xa3 \xa4 \xa5 \xa6 \xa7 \xa8 \xa9 \xaa \xab \xac \xad \xae \xaf
//  \xb0 \xb1 \xb2 \xb3 \xb4 \xb5 \xb6 \xb7 \xb8 \xb9 \xba \xbb \xbc \xbd \xbe \xbf
//  \xc0 \xc1 \xc2 \xc3 \xc4 \xc5 \xc6 \xc7 \xc8 \xc9 \xca \xcb \xcc \xcd \xce \xcf
//  \xd0 \xd1 \xd2 \xd3 \xd4 \xd5 \xd6 \xd7 \xd8 \xd9 \xda \xdb \xdc \xdd \xde \xdf
//  \xe0 \xe1 \xe2 \xe3 \xe4 \xe5 \xe6 \xe7 \xe8 \xe9 \xea \xeb \xec \xed \xee \xef
//  \xf0 \xf1 \xf2 \xf3 \xf4 \xf5 \xf6 \xf7 \xf8 \xf9 \xfa \xfb \xfc \xfd \xfe \xff

static unsigned char lexCharMap[256] = {
       0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   0,   1,   1,
       1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
       0,   2,   1,   1,   1,   3,   4,   1,   5,   6,   7,   8,   9,  10,  11,  12,
      13,  13,  13,  13,  13,  13,  13,  13,  13,  13,   1,  14,  15,  16,  17,   1,
       1,  18,  18,  18,  18,  19,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,
      18,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,  20,   1,  21,  22,  18,
       1,  18,  18,  18,  18,  19,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,
      18,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,   1,  23,   1,   1,   1,
       1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
       1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
       1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
       1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
       1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
       1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
       1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
       1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1
};

static const unsigned char lexStateMap[36] = {
    /*   0 */   0,   1,   2,   3,   3,   3,   4,   3,   3,   3,
    /*  10 */   5,   6,   3,   7,   8,   9,  10,   3,   3,   3,
    /*  20 */   3,   3,   3,   3,   3,   3,   3,   3,   3,  11,
    /*  30 */  12,  13,  14,  15,  16,  14
};

static const char lexNext[17][24] = {
    /*   0 */ { 1,-1, 2, 3,29, 4, 5, 6, 7, 8, 9,31,10,11,12,13,14,15,16,16,17,18,19,33},
    /*   1 */ { 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    /*   2 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,20,-1,-1,-1,-1,-1,-1,-1},
    /*   3 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    /*   4 */ {-1,-1,-1,-1,-1,-1,-1,19,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    /*   5 */ {-1,-1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    /*   6 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,30,-1,11,-1,-1,-1,-1,-1,34,-1,-1,-1,-1},
    /*   7 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,24,25,-1,-1,-1,-1,-1,-1},
    /*   8 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,26,-1,-1,-1,-1,-1,-1,-1},
    /*   9 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,27,-1,-1,-1,-1,-1,-1,-1},
    /*  10 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,16,-1,-1,-1,-1,16,16,-1,-1,-1,-1},
    /*  11 */ {-1,-1,-1,-1,21,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    /*  12 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,30,-1,-1,-1,-1,-1,34,-1,-1,-1,-1},
    /*  13 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,30,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    /*  14 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    /*  15 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,28},
    /*  16 */ {-1,-1,-1,-1,-1,-1,-1,-1,35,-1,35,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

static const char lexAccept[] = {
    /*   0 */  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,
    /*  10 */  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
    /*  20 */  4,  4,  4,  4,  4,  4,  4,  4,  4,  0,
    /*  30 */  4,  0,  4,  0,  0,  0
};

#line 12 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"

// nextState(state,c) is given the current state number and input
// character and evaluates to the next state.
#define nextState(state,c) (lexNext[lexStateMap[state]][lexCharMap[c]])

int ExpressionLex::getNextLexeme() {
  int  state           = 0;     // Current state
  int  lastAcceptState = 0;     // Most recently seen accept state
  int  prevState;               // State before lastAcceptState
  int  nextState;               // Next state
  unsigned int  lookahead;      // Lookahead character
  int  anchor;                  // Anchor mode for most recently seen accepting state

  unTerminateLexeme();
  initMore();
  markStart();

  for(;;) {
    // Check end of file. If there's an unprocessed accepting state,
    // lastAcceptState will be nonzero. In this case, ignore EOF for now so
    // that you can do the accepting action; otherwise, try to open another
    // file and return if you can't.

    for(;;) {
      if(((int)(lookahead = look(1))) != EOF) {
        assert(lookahead < 256);
        nextState = nextState(state, lookahead);
        break;
      } else if(lastAcceptState != 0) {   // still something to do
        nextState = -1;
        break;
      } else if(isWrap()) {               // another file?
        terminateLexeme();
        return 0;                         // EOI
      }
    }
    if(m_debug) {
      debugState(_T("--------"), state, lookahead);
    }

    if(nextState != -1) {
      if(advance() < 0) {                 // Buffer full
        const TCHAR *tooLongMessage = _T("Lexeme too long. Discarding extra characters.");
        error(getPos(), tooLongMessage);

        flush(true);

        if(m_debug) {
          debug(tooLongMessage);
          debugState(_T("--------"), state, look(1));
        }
      }

      if(anchor = lexAccept[nextState]) { // Is this an accept state
        prevState       = state;
        lastAcceptState = nextState;
        markEnd();                        // Mark input at current character
      }                                   // A subsequent gotoMark() returns us to this position.
      state = nextState;
    } else if(lastAcceptState == 0) {     // illegal input
      error(getPos(), _istprint(lookahead)?_T("Ignore bad input:'%c'"):_T("Ignore bad input:%#x"),lookahead);

      if(m_debug) {
        debug(_T("Ignore bad input:'%c'"), lookahead);
      }

      advance();
    } else {
      if(m_debug) {
        debugState(_T("accept--"), lastAcceptState, lookahead);
      }

      gotoMark();                         // Back up to previous accept state
      if(anchor & ANCHOR_END) {           // If end anchor is active
        pushback(1);                      // Push back the CR or LF
      }
      if(anchor & ANCHOR_START) {         // If start anchor is active
        moveStart();                      // Skip the leading newline
      }
      terminateLexeme();                  // Null-terminate the string

      switch(lastAcceptState) {
      case 1:
#line 100 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        ;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 2:
#line 88 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return NOT;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 3:
#line 79 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return MOD;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 4:
#line 82 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return LPAR;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 5:
#line 83 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return RPAR;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 6:
#line 77 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return PROD;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 7:
#line 75 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return PLUS;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 8:
#line 81 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return COMMA;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 9:
#line 76 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return MINUS;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 10:
#line 78 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return QUOT;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 11:
      case 30:
      case 32:
#line 48 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return NUMBER;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 12:
#line 97 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return SEMI;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 13:
#line 90 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return LT;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 14:
#line 96 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return ASSIGN;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 15:
#line 92 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return GT;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 16:
#line 99 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return nameOrKeyWord((_TUCHAR*)getText());
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 17:
#line 84 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return LB;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 18:
#line 85 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return RB;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 19:
#line 80 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return POW;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 20:
#line 94 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return NE;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 21:
#line 86 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return AND;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 22:
#line 50 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        { int i;
          const SourcePosition startpos = getPos();
          while(i = input()) {
            if(i < 0) {
              flushBuf();  /* Discard lexeme.     */
            } else if(i == '*' && look(1) == '/') {
              input();
              break;        /* Recognized comment. */
            }
          }
          if(i == 0) {
            error(startpos,_T("End of file in comment\n") );
          }
        }
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 23:
#line 65 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        { int i;
          while(i = input()) {
            if(i < 0) {
              flushBuf();  /* Discard lexeme. */
            } else if(i == '\n') {
              break;
            }
          }
        }
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 24:
#line 91 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return LE;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 25:
#line 95 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return NE;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 26:
#line 89 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return EQ;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 27:
#line 93 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return GE;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;
      case 28:
#line 87 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"
        return OR;
#line 94 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
        break;

      default:
        throwException(_T("%s:Unknown accept state:%d, text=<%s>"), __TFUNCTION__, lastAcceptState,getText());
        break;
      }

      unTerminateLexeme();
      lastAcceptState = 0;

      if(isMore()) {
        state = prevState;                // Back up
        initMore();
      } else {
        state = 0;
        markStart();
      }
    }
  }
}

#line 103 "C:\\mytools2015\\Lib\\Src\\Expression\\Expression.lex"

  /* This part goes to the last part of exprlex.cpp */

typedef struct {
  const TCHAR                *m_name;
  const ExpressionInputSymbol m_token;
} KeyWord;

static const KeyWord keywordtable[] = {
 _T("ABS"       ),ABS
,_T("ACOS"      ),ACOS
,_T("ACOSH"     ),ACOSH
,_T("ACOT"      ),ACOT
,_T("ACSC"      ),ACSC
,_T("AND"       ),AND
,_T("ASEC"      ),ASEC
,_T("ASIN"      ),ASIN
,_T("ASINH"     ),ASINH
,_T("ATAN"      ),ATAN
,_T("ATAN2"     ),ATAN2
,_T("ATANH"     ),ATANH
,_T("BINOMIAL"  ),BINOMIAL
,_T("CEIL"      ),CEIL
,_T("CHI2DENS"  ),CHI2DENS
,_T("CHI2DIST"  ),CHI2DIST
,_T("LINCGAMMA" ),LINCGAMMA
,_T("COS"       ),COS
,_T("COSH"      ),COSH
,_T("COT"       ),COT
,_T("CSC"       ),CSC
,_T("ERF"       ),ERF
,_T("EXP"       ),EXP
,_T("EXP10"     ),EXP10
,_T("EXP2"      ),EXP2
,_T("FAC"       ),FAC
,_T("FLOOR"     ),FLOOR
,_T("GAMMA"     ),GAMMA
,_T("GAUSS"     ),GAUSS
,_T("HYPOT"     ),HYPOT
,_T("IF"        ),IIF
,_T("INVERF"    ),INVERF
,_T("LN"        ),LN
,_T("LOG10"     ),LOG10
,_T("LOG2"      ),LOG2
,_T("MAX"       ),MAX
,_T("MIN"       ),MIN
,_T("MOD"       ),MOD
,_T("NORM"      ),NORM
,_T("NORMRAND"  ),NORMRAND
,_T("NORMRANDOM"),NORMRAND
,_T("NORMRND"   ),NORMRAND
,_T("NOT"       ),NOT
,_T("OR"        ),OR
,_T("POLY"      ),POLY
,_T("PROBIT"    ),PROBIT
,_T("PRODUCT"   ),INDEXEDPRODUCT
,_T("RAND"      ),RAND
,_T("RANDOM"    ),RAND
,_T("ROOT"      ),ROOT
,_T("RND"       ),RAND
,_T("SEC"       ),SEC
,_T("SIGN"      ),SIGN
,_T("SIN"       ),SIN
,_T("SINH"      ),SINH
,_T("SQR"       ),SQR
,_T("SQRT"      ),SQRT
,_T("SUM"       ),INDEXEDSUM
,_T("TAN"       ),TAN
,_T("TANH"      ),TANH
,_T("TO"        ),TO
};

typedef CompactStrIHashMap<ExpressionInputSymbol> HashMapType;

class ExpressionKeyWordMap : public HashMapType {
public:
  ExpressionKeyWordMap(size_t capacity) : HashMapType(capacity) {
    for(int i = 0; i < ARRAYSIZE(keywordtable); i++) {
      put(keywordtable[i].m_name,keywordtable[i].m_token);
    }
  }
};

static ExpressionKeyWordMap keywords(405);

ExpressionInputSymbol ExpressionLex::nameOrKeyWord(const _TUCHAR *lexeme) { // static
  const ExpressionInputSymbol *p = keywords.get(lexeme);
  return p ? *p : NAME;
}

void ExpressionLex::verror(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  if(m_parser) {
    m_parser->verror(pos, format, argptr);
  } else {
    Scanner::verror(pos, format, argptr);
  }
}

#ifdef CHECKKEYWORDMAP

class CheckKeywordMap {
private:
  void findBestHashMapSize();
public:
  CheckKeywordMap() {
    findBestHashMapSize();
  }
};

void ExpressionLex::findBestHashMapSize() { // static
  int  bestTableSize   = -1;
  UINT bestChainLength = 0;
  for(int tableSize = 20; tableSize < 2000; tableSize++) {
    ExpressionKeyWordMap ht(tableSize);
    const int chainLength = ht.getMaxChainLength();
    debugLog(_T("tableSize:%4d, maxChainLength:%d\n"), tableSize, chainLength);
    if((bestTableSize < 0) || (chainLength < bestChainLength)) {
      bestTableSize   = tableSize;
      bestChainLength = chainLength;
    }
    if(bestChainLength == 1) {
      break;
    }
  }
  debugLog(_T("\nTablesize=%d gives best hashmap (maxchainLength=%u\n")
          ,bestTableSize, bestChainLength);
}

static CheckKeywordMap checkKeywordMap;

#endif

}; // namespace Expr
#line 115 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
