#include "pch.h"
#include <eh.h>
#include <stdarg.h>

#define CASE(c) case EXCEPTION_##c: return _T(#c);

static String translateExceptionCode(int code) {
  switch(code) {
  CASE(ACCESS_VIOLATION         );
  CASE(ARRAY_BOUNDS_EXCEEDED    );
  CASE(BREAKPOINT               );
  CASE(DATATYPE_MISALIGNMENT    );
  CASE(FLT_DENORMAL_OPERAND     );
  CASE(FLT_DIVIDE_BY_ZERO       );
  CASE(FLT_INEXACT_RESULT       );
  CASE(FLT_INVALID_OPERATION    );
  CASE(FLT_OVERFLOW             );
  CASE(FLT_STACK_CHECK          );
  CASE(FLT_UNDERFLOW            );
  CASE(ILLEGAL_INSTRUCTION      );
  CASE(IN_PAGE_ERROR            );
  CASE(INT_DIVIDE_BY_ZERO       );
  CASE(INT_OVERFLOW             );
  CASE(INVALID_DISPOSITION      );
  CASE(NONCONTINUABLE_EXCEPTION );
  CASE(PRIV_INSTRUCTION         );
  CASE(SINGLE_STEP              );
  CASE(STACK_OVERFLOW           );
  default:
    return format(_T("Unknown exceptioncode:%lx"), code);
  }
}

void exceptionTranslator(UINT u, EXCEPTION_POINTERS *pExp) {
  throwException(translateExceptionCode(pExp->ExceptionRecord->ExceptionCode));
}

bool getDebuggerPresent() {
#ifndef _DEBUG
  return false;
#else
  static bool firstTime       = true;
  static bool debuggerPresent = false;
  if(firstTime) {
    debuggerPresent = IsDebuggerPresent() ? true : false;
    firstTime = false;
  }
  return debuggerPresent;
#endif
}

#pragma warning(disable : 4073)
#pragma init_seg(lib)

class InitExceptionClass {
private:
  _se_translator_function m_prevtranslator;
public:
  InitExceptionClass();
  ~InitExceptionClass();
};

InitExceptionClass::InitExceptionClass() {
  m_prevtranslator = _set_se_translator(exceptionTranslator);
}

InitExceptionClass::~InitExceptionClass() {
  if(m_prevtranslator != NULL) {
    _set_se_translator(m_prevtranslator);
  }
}

static InitExceptionClass exceptionClassInitializer;

void throwInvalidArgumentException(const TCHAR *method, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  throwException(_T("%s:Invalid argument. %s"), method, msg.cstr());
}

void throwException(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String tmp = vformat(format, argptr);
  va_end(argptr);
  throwException(tmp);
}

void throwException(const String &s) {
  throw Exception(s);
}

void throwTimeoutException(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String tmp = vformat(format, argptr);
  va_end(argptr);
  throw TimeoutException(tmp.cstr());
}

void throwIndexOutOfRangeException(const TCHAR *method, UINT64 index, UINT64 size) {
  throwException(_T("%s:Index %s out of range. size=%s")
                ,method
                ,format1000(index).cstr()
                ,format1000(size).cstr());
}

void throwIndexOutOfRangeException(const TCHAR *method, UINT64 index, UINT64 count, UINT64 size) {
  throwException(_T("%s(%s,%s):%s out of range. size=%s")
                ,method
                ,format1000(index).cstr()
                ,format1000(count).cstr()
                ,format1000(index+count).cstr()
                ,format1000(size).cstr());
}

void throwSelectFromEmptyCollectionException(const TCHAR *method) {
  throwException(_T("%s:Cannot select from empty collection"), method);
}

void throwMethodException(const TCHAR *className, const TCHAR *method, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  throwException(_T("%s::%s:%s"), className, method, msg.cstr());
}

void throwUnsupportedOperationException(const TCHAR *method) {
  throwException(_T("%s:Unsupported operation"), method);
}
