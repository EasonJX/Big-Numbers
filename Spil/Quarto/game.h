#pragma once

#include <TinyBitSet.h>

#define ISWITHTOP(attr)  (((attr) & 1)?true:false)
#define ISSQUARE( attr)  (((attr) & 2)?true:false)
#define ISBIG(    attr)  (((attr) & 4)?true:false)
#define ISBLACK(  attr)  (((attr) & 8)?true:false)

#define ROWCOUNT   4
#define COLCOUNT   4
#define FIELDCOUNT (ROWCOUNT*COLCOUNT)

class Brick {
public:
  static BYTE s_attr[FIELDCOUNT];
  static inline bool isValid(char brick) {
    return (BYTE)brick < FIELDCOUNT;
  }
  static String toString(BYTE attr);
};

typedef BitSet16 BrickSet;

class Field {
public:
  inline Field() {
  }
  inline Field(int r, int c) : m_row(r), m_col(c) {
  }
  char m_row;
  char m_col;
  inline bool isField() const {
    return m_row >= 0;
  }
  inline bool isValid() const {
    return ((BYTE)m_row < ROWCOUNT) && ((BYTE)m_col < COLCOUNT);
  }
  inline String toString() const {
    return isField() ? format(_T("(%d,%d)"),m_row,m_col) : _T("NOFIELD");
  }
};

inline bool operator==(const Field &f1, const Field &f2) {
  return (f1.m_row == f2.m_row) && (f1.m_col == f2.m_col);
}

inline bool operator!=(const Field &f1, const Field &f2) {
  return !(f1 == f2);
}

typedef CompactArray<Field> FieldArray;

#define NOBRICK ((char)-1)

class Move {
public:
  Field m_field;
  char  m_brick;
  Move(BYTE r, BYTE c, char brick);
  inline Move(const Field &f, char brick) : m_field(f), m_brick(brick) {
  }
  Move();
  Move(const String &s);
  inline bool isMove() const {
    return m_brick != NOBRICK;
  }
  inline String toString() const {
    return format(_T("%s,%s"), m_field.toString().cstr(),(isMove()?format(_T("%d"), m_brick).cstr() : _T("NOBRICK")));
  }
};

typedef CompactArray<Move> MoveArray;

extern const Move  NOMOVE;
extern const Field NOFIELD;

typedef enum {
  HUMAN_PLAYER     = -1,
  COMPUTER_PLAYER  =  1
} Player;

#define OPPONENT(p) ((Player)(-(int)(p)))

class PushableGameState {
public:
  Player         m_playerInTurn;
  BitSet16       m_unused;
  int            m_score;
  bool           m_gameOver;
  bool           m_hasWinner;
  BYTE           m_board[ROWCOUNT][COLCOUNT];
};

class Game {
private:
  bool nextPos(Field &f) const;
  bool isWinnerMove(const Move &m) const;

  bool               m_colored;
  Player             m_startPlayer;
  MoveArray          m_moveList;
  PushableGameState  m_state;

  void   doMove(const Move &m, int lookahead = 0);
  int    firstBrick() const;
  bool   nextBrick(Move &m) const;
  Move   firstMove() const;
  void   nextMove(Move &m) const;
  Move   findRandomMove() const;
  friend class MoveEvaluator;
public:
  Game();
  explicit    Game(const String &s);
  void        newGame(bool colored, Player startPlayer);
  int         getBrickCount() const;
  inline bool brickUnUsed(int          b) const { return  m_state.m_unused.contains(b);           }
  inline bool brickUsed(  int          b) const { return !m_state.m_unused.contains(b);           }
  inline bool isEmpty(    const Field &f) const { return  m_state.m_board[f.m_row][f.m_col] == 0; }
  inline Player getBrickOwner(int b) const { // only valid in colored games
    return ISBLACK(Brick::s_attr[b]) ? COMPUTER_PLAYER : HUMAN_PLAYER;
  }
  int    getBrickOnField(const Field &f) const;
  bool   isSelectableBrick(int b) const;
  void   executeMove(const Move &m);
  Player getPlayerInTurn() const { return m_state.m_playerInTurn; }
  Player getStartPlayer()  const { return m_startPlayer;          }
  bool   isGameOver()      const { return m_state.m_gameOver;     }
  bool   isColored()       const { return m_colored;              }
  int    evaluate()        const { return m_state.m_score;        }
  bool   hasWinner()       const { return m_state.m_hasWinner;    }
  FieldArray getWinnerFields() const;
  const MoveArray &getHistory() const {
    return m_moveList;
  }
  String toString() const;
};

#define STACKSIZE 16
class MoveEvaluator {
private:
  int               m_maxDepth;
  int               m_bestScore;
  Move              m_bestMove;
  PushableGameState m_stateStack[STACKSIZE], *top;
  Game              m_game;
  int               m_evalCount;
  inline void initStack() { top = m_stateStack; top--; }
  inline void push()      { *(++top) = m_game.m_state; }
  inline void pop()       { top--; }
  inline void restore()   { m_game.m_state = *top; }
  int maximize(int c_min, int depth);
  int minimize(int c_max, int depth);
public:
  MoveEvaluator(const Game &game) { m_game = game; }
  Move findMove(int lookahead);
  int getBestScore() const { return m_bestScore; }
  int getEvalCount() const { return m_evalCount; }
};
