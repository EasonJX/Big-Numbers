#include "pch.h"
#include <MathUtil.h>

static const char *decRationalPattern[] = {
  "[+/-]?[0-9]+/(//[0-9]+/)?"
 ,NULL
};

StringArray DecRationalValueStreamScanner::getRegexLines() { // static
  return StringArray(decRationalPattern);
}
