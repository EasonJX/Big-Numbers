#include "StdAfx.h"
#include <codecvt>
#include <locale>
#include <string>
#include "utf8.h"
#include "EncodedString.h"

#if _MSC_VER >= 1900
std::string utf16_to_utf8(std::u16string utf16_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
  auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
  return convert.to_bytes(p, p + utf16_string.size());
}
#else
std::string utf16_to_utf8(std::u16string utf16_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(utf16_string);
}
#endif

String utf8_to_String(const char *b, size_t len) { // len in bytes
  String result;
  int cp;
  const char *endp = b + len;
  while((cp = utf8::next(b, endp)) != 0) {
    result += (TCHAR)cp;
  }
  return result;
}

String EncodedString::toString() const {
  if(isEmpty()) {
    return EMPTYSTRING;
  }
  String result;
  switch(m_encoding) {
  case ID3TE_UTF8   :
    result = utf8_to_String((char*)getData(), size());
    break;
  case ID3TE_UTF16  :
    { ByteArray tmp(*this);
      const USHORT *endp = (USHORT*)(tmp.getData() + tmp.size());
      for(USHORT *cp = (USHORT*)tmp.getData(); cp < endp; cp++) {
        *cp = _byteswap_ushort(*cp);
      }
      tmp.appendZeroes(2);
      result = (wchar_t*)tmp.getData();
/*
    { std::u16string utf16 = (char16_t*)getData();
      std::string utf8 = utf16_to_utf8(utf16);
      result = utf8_to_String(utf8.data(), utf8.length());
*/
    }
    break;
//  case ID3TE_UTF16BE:
  case ID3TE_ASCII:
    { const size_t len = size();
      char tmp[1024], *buf = (len < sizeof(tmp)) ? tmp : new char[len + 1];
      strncpy(buf, (char*)getData(), len); buf[len] = 0;
      result = buf;
      if(buf != tmp) delete[] buf;
    }
    break;
  default:
    result = format(_T("Unknown encoding:%d"), m_encoding);
    break;
  }
  return result;
}