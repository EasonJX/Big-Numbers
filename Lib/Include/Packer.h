#pragma once

#include "MyString.h"
#include "ByteArray.h"

class Packer {
private:
  size_t         m_first;
  ByteArray      m_buffer;
public:
  enum ElementType { // always append new types at the end
    E_BOOL
   ,E_CHAR
   ,E_SHORT
   ,E_INT
   ,E_LONG
   ,E_FLOAT
   ,E_DOUBLE
   ,E_STR
   ,E_WSTR
   ,E_VOID
   ,E_LONG_LONG
  };
  Packer();
  void clear();
  void write(    ByteOutputStream &s) const;
  bool read(     ByteInputStream  &s);
  void writeEos( ByteOutputStream &s) const;

  size_t getDataSize()        const { return m_buffer.size(); }
  size_t getRequestedSize()   const { return getDataSize() + sizeof(size_t); }
  inline ElementType peekType() const {
    return (ElementType)m_buffer[m_first];
  }
  Packer &addElement(ElementType  t, const void *e, size_t size);
  Packer &getElement(ElementType et,       void *e, size_t size);
  Packer &operator<<(         bool         n) { return addElement(E_BOOL , &n, sizeof(n)); }
  Packer &operator<<(         char         n) { return addElement(E_CHAR , &n, sizeof(n)); }
  Packer &operator<<(unsigned char         n) { return addElement(E_CHAR , &n, sizeof(n)); }
  Packer &operator<<(         short        n);
  Packer &operator<<(unsigned short        n);
  Packer &operator<<(         int          n);
  Packer &operator<<(UINT                  n);
  Packer &operator<<(         long         n);
  Packer &operator<<(unsigned long         n);
  Packer &operator<<(         __int64      n);
  Packer &operator<<(unsigned __int64      n);
  Packer &operator<<(         float        n);
  Packer &operator<<(         double       n);
  Packer &operator<<( const   char    * const s);
  Packer &operator<<( const   wchar_t * const s);
  Packer &operator<<( const   String      &s);
  Packer &operator<<( const   ByteArray   &a);

  Packer &operator>>(         bool        &n) { return getElement(E_BOOL , &n, sizeof(n)); }
  Packer &operator>>(         char        &n) { return getElement(E_CHAR , &n, sizeof(n)); }
  Packer &operator>>(unsigned char        &n) { return getElement(E_CHAR , &n, sizeof(n)); }
  Packer &operator>>(         short       &n);
  Packer &operator>>(unsigned short       &n);
  Packer &operator>>(         int         &n);
  Packer &operator>>(UINT                 &n);
  Packer &operator>>(         long        &n);
  Packer &operator>>(unsigned long        &n);
  Packer &operator>>(         __int64     &n);
  Packer &operator>>(unsigned __int64     &n);
  Packer &operator>>(         float       &n);
  Packer &operator>>(         double      &n);
  Packer &operator>>(         char        *s);
  Packer &operator<<(         wchar_t     *s);
  Packer &operator>>(         String      &s);
  Packer &operator>>(         ByteArray   &a);
};

inline Packer &operator<<(Packer &p, const StreamDelimiter &d) {
  return p;
}
