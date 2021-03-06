// TestExpressionLex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Math/Expression/ExpressionParser.h>
#include <Math/Expression/ExpressionLex.h>

static void scan(TCHAR *s) {
  LexStringStream stream(s);
  ExpressionLex lex(&stream);

  int symbol;
  do {
    symbol = lex.getNextLexeme();
    const SourcePosition pos = lex.getPos();
    const SourcePosition ppos = lex.getPreviousPos();
    _tprintf(_T("ppos:%s. startPos:%s : symbol:%s\n")
            ,ppos.toString().cstr(), pos.toString().cstr()
            ,ExpressionTables->getSymbolName(symbol));
    lex.markPrevious();
  } while(symbol != 0);
}

int main(int argc, char **argv) {
  scan(_T("sin ( + * x = ;"));
  return 0;
}
