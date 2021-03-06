/* DO NOT EDIT THIS FILE - it is machine generated */
#pragma once

#include <LRparser.h>

#line 3 "C:\\Mytools2015\\CheckMnemonics\\Resource.y"
#include "ResourceSymbol.h"
#include "ResourceLex.h"
#include "ParserTree.h"

class ResourceParser : public LRparser {
private:
  static const ParserTables *ResourceTables;
  static int    _ttoi(const TCHAR *s);
  static String stripQuotes(const String &s);
public:
  ResourceParser(ParserTree &tree, ResourceLex *lex = NULL) : m_tree(tree), LRparser(*ResourceTables,lex) {}
  void  verror(const SourcePosition &pos, const TCHAR *format,va_list argptr);
  static const ParserTables &getTables() {
    return *ResourceTables;
  }
private:
  ParserTree &m_tree;
  SyntaxNode *m_leftSide,**m_stacktop,*m_userstack[256];
  SyntaxNode *getStackTop(int fromtop)           { return m_stacktop[-fromtop];            }
  void userStackInit()                           { m_stacktop = m_userstack;               }
  void userStackShiftSymbol(unsigned int symbol) { m_stacktop++;                           } // push 1 element (garbage) on userstack
  void userStackPopSymbols( unsigned int count ) { m_stacktop      -= count;               } // pop count symbols from userstack
  void userStackShiftLeftSide()                  { *(++m_stacktop) = m_leftSide;           } // push($$) on userstack
  void defaultReduce(       unsigned int prod)   { m_leftSide      = *m_stacktop;          } // $$ = $1
  int  reduceAction(        unsigned int prod);
  SyntaxNode       *newNode(const SourcePosition &pos, int symbol, ... );
};

#line 22 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"

