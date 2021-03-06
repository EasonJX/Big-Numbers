#include "pch.h"
#include "DateTimeStreamScanner.h"

static const char *timestampPatterns[] = {
  "/(0?[1-9]/|[12][0-9]/|3[01]/)[//./-]/(0?[1-9]/|1[0-2]/)[//./-][0-9][0-9]?[0-9]?[0-9]?"
  "/( +/([0-1]?[0-9]/|2[0-3]/)[.:/-][0-5]?[0-9]/([.:/-][0-5]?[0-9]/([.:/-][0-9][0-9]?[0-9]?/)?/)?/)?"
 ,NULL
};

StringArray TimestampStreamScanner::getRegexLines() { // static
  return StringArray(timestampPatterns);
}
