%{

  /* This part goes to dummyparser.h */

#include "dummysymbol.h"
#include "dummylex.h"

#ifndef YYSTYPE
#define YYSTYPE int
#endif

class DummyParser : public LRparser {
public:
  DummyParser(Scanner *lex) : LRparser(*DummyTables,lex) {}
private:
  YYSTYPE m_leftSide,*m_stacktop,m_userstack[256];
  void userStackInit()                           { m_stacktop = m_userstack;               }
  void userStackShiftSymbol(unsigned int symbol) { m_stacktop++;                           } // push 1 element (garbage) on userstack
  void userStackPopSymbols( unsigned int count ) { m_stacktop      -= count;               } // pop count symbols from userstack
  void userStackShiftLeftSide()                  { *(++m_stacktop) = m_leftSide;           } // push($$) on userstack
  void defaultReduce(       unsigned int prod)   { m_leftSide      = *m_stacktop;          } // $$ = $1
  int  reduceAction(        unsigned int prod);
};
%}
%right a b
%{
  /* This part goes to the first part of dummyaction.cpp */

#include "stdafx.h"
#include "dummyparser.h"
%}
%%

start	: S;

S	: a S	{ printf("reduce by S -> a S\n"); }
	| S a	{ printf("reduce by S -> S a\n"); }
	| b		{ printf("reduce by S -> b\n");   }
	| a		{ printf("reduce by S -> a\n");   }
	;

%%

  /* This part goes to the last part of dummyaction.cpp */
