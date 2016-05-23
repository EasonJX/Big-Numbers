#pragma once

#include "MyUtil.h"
#include <ctype.h>

class LexStream { // abstract class for inputstreams
public:
  virtual bool open(const String &s)                  = NULL; // Opens a new inputstream. return true on success, false on error
  virtual void close()                                = NULL; // Close stream
  virtual bool eof()                                  = NULL;
  virtual bool ok()                                   = NULL;
  virtual int  getChars(_TUCHAR *dst, unsigned int n) = NULL;
};

#ifdef UNICODE

class ByteQueue : private ByteArray {
private:
  const UINT   m_codePage;
  const DWORD  m_flags;
  bool hasFullLine() const;
  const BYTE *findLastNewLine() const;
public:
  inline ByteQueue(UINT codePage, DWORD flags) : m_codePage(codePage), m_flags(flags) {
  }
  inline void clear() {
    ByteArray::clear();
  }
  inline unsigned int size() const {
    return ByteArray::size();
  }
  inline bool isEmpty() const {
    return size() == 0;
  }
    
  String getConvertedString(int count); // count is the number of bytes to convert
  int readUntilHasNewLine(FILE *f);
};

class CharQueue : private String {
public:
  CharQueue() {
  }
  void clear() {
    (String&)(*this) = _T("");
  };
  unsigned int size() const {
    return (unsigned int)length();
  }
  bool isEmpty() const {
    return size() == 0;
  }

  void put(const String &s) {
    *this += s;
  }
  int get(_TUCHAR *dst, int n);
};

#endif

class LexFileStream : public LexStream {
private:
  FILE        *m_f;
  int          m_oldMode;

#ifdef UNICODE
  ByteQueue    m_rawQueue;
  CharQueue    m_convertedQueue;
#endif
  void initQueues();

public:

#ifdef UNICODE
  LexFileStream(                    UINT codePage = CP_UTF8, DWORD flags = 0);
  LexFileStream(const String &name, UINT codePage = CP_UTF8, DWORD flags = 0);
#else
  LexFileStream();
  LexFileStream(const String &name);
#endif

  ~LexFileStream() {
    close();
  }

  bool open(const String &s);
  void close();
  int  getChars(_TUCHAR *dst, unsigned int n);
  bool eof();

  bool ok() {
    return m_f != NULL;
  }
};

class LexStringStream : public LexStream {
private:
  String m_string;
  int m_pos;
public:
  LexStringStream() {
    open("");
  }
  LexStringStream(const String &str) {
    open(str);
  }
  bool open(const String &str);
  void close() {
    open("");
  }
  int getChars(_TUCHAR *dst, unsigned int n);

  bool eof() {
    return m_pos >= m_string.length();
  }

  bool ok() {
    return true;
  }
};

class SourcePosition {
private:
  String m_fileName;
  int    m_lineNumber;
  int    m_column;
public:
  SourcePosition(const String &fileName, int lineNumber, int column);
  SourcePosition(int lineNumber = 0, int column = 0);
  void setLocation(int lineNumber, int column);
  
  inline const String &getFileName() const {
    return m_fileName;
  }
  
  inline int getLineNumber() const {
    return m_lineNumber;
  }
  
  inline int getColumn() const {
    return m_column;
  }
  
  inline void incrLineNumber() {
    m_lineNumber++;
    m_column = 0;
  }

  inline void incrColumn(int amount = 1) {
    m_column += amount;
  }

  String toString() const;
  friend class Scanner;

  static int findCharIndex(const TCHAR *s, const SourcePosition &pos);
  static SourcePosition findSourcePosition(const TCHAR *s, int index);

  // findCharIndex(s, findSourcePosition(s, index)) == index; index = [0..len(s)[
};

class Scanner {

#define SCANNERMAXLOOK    16                     // Maximum amount of lookahead
#define SCANNERMAXLEX     1024                   // Maximum lexeme sizes.
#define SCANNERBUFSIZE    ((8*SCANNERMAXLEX)+(2*SCANNERMAXLOOK))

                                                 //  Values of the anchor field:
#define ANCHOR_NONE  0                           //  Not anchored
#define ANCHOR_START 1                           //  Lexeme Anchored at start of line
#define ANCHOR_END   2                           //  At end of line
#define ANCHOR_BOTH  (ANCHOR_START | ANCHOR_END) //  Both start and end of line.

private:
  LexStream      *m_inputStream;                 // Input source
  _TUCHAR         m_inputBuffer[SCANNERBUFSIZE]; // Input buffer
  _TUCHAR        *m_endBuf;                      // Just past last TCHAR
  _TUCHAR        *m_nextChar;                    // Next input TCHAR
  _TUCHAR        *m_startMark;                   // Start of current lexeme
  _TUCHAR        *m_endMark;                     // End of current lexeme
  _TUCHAR        *m_previousMark;                // Start of previous lexeme
  SourcePosition  m_previousPos;                 // Position of previous lexeme
  int             m_previousLength;              // Length of previous lexeme
  SourcePosition  m_pos;                         // Position of current lexeme
  SourcePosition  m_markPos;                     // Position when markEnd() called
  _TUCHAR         m_termchar;                    // Holds the TCHAR that was overwritten by a 0.
                                                 // See terminateLexeme()/unTerminateLexeme()
  bool            m_eofRead;                     // true if end of file has been read.
                                                 // It's possible for this to be true,
                                                 // and for characters to still be in the input buffer.
  bool            m_moreFlag;

  void init(LexStream *stream, const SourcePosition &pos);

  int fillBuf(_TUCHAR *start);

  bool noMoreChars() const {
    return m_eofRead && (m_nextChar >= m_endBuf);
  }

  _TUCHAR *end() {                         // Just past last char in buf
    return m_inputBuffer + SCANNERBUFSIZE;
  }

protected:
  bool m_debug;             // If true, call debug on every cycle
  int look(int n);          // Return the n'th TCHAR of lookahead.
                            // Return EOF if you try to look past end of file,
                            // 0 if you try to look past end of the buffer..

  int advance();            // Returns the next TCHAR from input and advances past it
  int flushBuf();
  int flush(bool force);
  int input();
  void unput(int ch);
  void less(int n);
  bool pushback(int count); // Push count characters back into the input. 
  void terminateLexeme();
  void unTerminateLexeme();
  void terminateInput();    // when called, next call to getNextLexeme() will return EOS
  virtual void setMore();
  virtual void initMore();
  virtual bool isMore() const;
  virtual bool isWrap();
  void debugState(const TCHAR *label, int state, int lookahead);

public:
  Scanner();
  Scanner(LexStream *stream);
  ~Scanner();
  void newStream(LexStream *stream, const SourcePosition &pos);
  void newStream(LexStream *stream, int lineno = 1);
  int setLineNumber(int lineNumber);
  SourcePosition setPos(const SourcePosition &pos);

  inline _TUCHAR *getText() const {
    return m_startMark;
  }

  inline int getLength() const {
    return m_endMark - m_startMark;
  }

  SourcePosition getPos() const;

  inline int getLineNumber() const {
    return getPos().getLineNumber();
  }

  inline _TUCHAR *getPreviousText() const {
    return m_previousMark;
  }

  inline int getPreviousLength() const {
    return m_previousLength;
  }

  inline const SourcePosition &getPreviousPos() const {
    return m_previousPos;
  }

  inline int getPreviousLineNumber() const {
    return m_previousPos.getLineNumber();
  }

  _TUCHAR *markStart();    // Set start- and endMark to current position. Returns next TCHAR
  _TUCHAR *markEnd();      // Set endMark to current position. Returns next TCHAR
  _TUCHAR *moveStart();    // If startMark is set, advance it one TCHAR
  _TUCHAR *markPrevious(); // Set previousMark and Length = position remembered by markStart/markEnd
  _TUCHAR *gotoMark();     // Set current position = position remembered by markEnd()

  inline void setDebug(bool newvalue) {
    m_debug = newvalue;
  }
  void error(const SourcePosition &pos, const TCHAR *format, ...);
  void debug(const TCHAR *format, ...);
  virtual void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr);
  virtual void vdebug(const TCHAR *format, va_list argptr);
  virtual int getNextLexeme();
};
