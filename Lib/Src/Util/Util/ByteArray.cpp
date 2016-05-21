#include "pch.h"
#include <ByteArray.h>

DEFINECLASSNAME(ByteArray);

ByteArray::ByteArray() {
  init();
}

ByteArray::ByteArray(const BYTE *data, unsigned int size) {
  init();
  setCapacity(size);
  m_size = size;
  if(m_size) {
    memcpy(m_data, data, m_size);
  }
}

ByteArray::ByteArray(const ByteArray &src) {
  init();
  setCapacity(src.m_size);
  m_size = src.m_size;
  if(m_size) {
    memcpy(m_data,src.m_data,m_size);
  }
}

ByteArray::ByteArray(unsigned int capacity) {
  init();
  setCapacity(capacity);
}

ByteArray &ByteArray::operator=(const ByteArray &src) {
  if(&src == this) {
    return *this;
  }
  if(src.m_size != m_size) {
    cleanup();
    setCapacity(src.m_size);
  }
  m_size = src.m_size;
  if(m_size) {
    memcpy(m_data,src.m_data,m_size);
  }
  return *this;
}

ByteArray::~ByteArray() {
  cleanup();
}

ByteArray &ByteArray::clear(int newCapacity) {
  switch(newCapacity) {
  case 0 :
    cleanup(); 
    break;
  case -1:
    m_size = 0;
    break;
  default:
    m_size = 0;
    setCapacity(newCapacity);
    break;
  }
  return *this;
}

ByteArray &ByteArray::append(const ByteArray &d) {
  return append(d.m_data, d.size());
}

ByteArray &ByteArray::append(const BYTE *data, unsigned int size) {
  if(size) {
    const unsigned int newSize = m_size + size;
    if(newSize > m_capacity) {     // Careful here!!! data and m_data might overlap!!
      const unsigned int newCapacity = 2 * newSize; // > 0
      BYTE *newData = new BYTE[newCapacity];
      if(m_size > 0) {
        memcpy(newData, m_data, m_size);
      }
      memcpy(newData + m_size, data, size);
      if(m_data != NULL) { // Now we can safely delete m_data.
        delete[] m_data;
      }
      m_data     = newData;
      m_capacity = newCapacity;
    } else {
      memcpy(m_data+m_size, data, size);
    }
    m_size = newSize;
  }
  return *this;
}

ByteArray &ByteArray::insertConstant(unsigned int index, BYTE b, unsigned int count) {
  if(count == 0) {
    return *this;
  }
  if(index > m_size) {
    indexError(index);
  }
  const unsigned int newSize = m_size + count;
  if(newSize > m_capacity) {
    const unsigned int newCapacity = 2 * newSize;
    BYTE *newData = new BYTE[newCapacity];
    if(index > 0) {
      memcpy(newData, m_data, index);
    }
    if(index < m_size) {
      memcpy(newData+index+count, m_data+index, m_size-index);
    }
    if(m_data != NULL) {
      delete[] m_data;
    }
    m_data = newData;
    m_capacity = newCapacity;
  } else if(index < m_size) {
    memmove(m_data+index+count, m_data+index, m_size-index);
  }
  memset(m_data+index, b, count);
  m_size = newSize;
  return *this;
}

ByteArray ByteArray::operator+(const ByteArray &d) const {
  ByteArray result(*this);
  result.append(d);
  return result;
}

ByteArray &ByteArray::operator+=(const ByteArray &d) {
  return append(d);
}

ByteArray &ByteArray::setData(const BYTE *data, unsigned int size) {
  cleanup();
  if(size > 0) {
    setCapacity(size);
    m_size = size;
    memcpy(m_data, data, size);
  }
  return *this;
}

ByteArray &ByteArray::remove(unsigned int index, unsigned int count) {
  DEFINEMETHODNAME(remove);
  if(count == 0) {
    return *this;
  }
  const unsigned int j = index+count;
  if(j > m_size) {
    throwException(_T("%s::%s(%lu,%lu): Invalid index. size=%u"), s_className, method, index, count, m_size);
  }
  if(j < m_size) {
    memmove(m_data+index, m_data+j, (m_size-j));
  }
  m_size -= count;
  if(m_size < m_capacity/2) {
    setCapacity(m_size);
  }
  return *this;
}

void ByteArray::indexError(unsigned int i) const {
  throwException(_T("%s:Index %u out of range. Size=%u"), s_className, i, m_size);
}

void ByteArray::init() {
  m_data     = NULL;
  m_capacity = 0;
  m_size     = 0;
}

void ByteArray::cleanup() {
  if(m_data != NULL) {
    delete[] m_data;
  }
  init();
}

void ByteArray::setCapacity(unsigned int capacity) {
  if(capacity < m_size) {
    capacity = m_size;
  }
  if(capacity == 0) {
    cleanup();
  } else if(capacity != m_capacity) {
    BYTE *newData = new BYTE[capacity];
    if(m_size > 0) {
      memcpy(newData, m_data, m_size);
      delete[] m_data;
    }
    m_data     = newData;
    m_capacity = capacity;
  } 
}

bool ByteArray::operator==(const ByteArray &a) const {
  if(this == &a) {
    return true;
  }
  if(m_size != a.m_size) {
    return false;
  }
  if(m_size == 0) {
    return true;
  }
  return memcmp(m_data, a.m_data, m_size) == 0;
}

bool ByteArray::operator!=(const ByteArray &a) const {
  return !(*this == a);
}

void ByteArray::save(ByteOutputStream &s) const {
  const unsigned int size = m_size; // if modified, => take care of ByteFileArray
  s.putBytes((const BYTE *)&size, sizeof(size));
  if(size) {
    s.putBytes(m_data, size);
  }
}

void ByteArray::load(ByteInputStream &s) {
  clear();
  unsigned int size;
  s.getBytesForced((BYTE *)&size, sizeof(size));
  if(size) {
    setCapacity(size);
    s.getBytesForced(m_data, size);
    m_size = size;
  }
}

ByteFileArray::ByteFileArray(const String &fileName, unsigned int startOffset) : m_startOffset(startOffset) {
  m_f = FOPEN(fileName, _T("rb"));
  FSEEK(m_f, m_startOffset);
  unsigned int size;
  FREAD(&size, sizeof(size), 1, m_f);
  m_size = size;
}

ByteFileArray::~ByteFileArray() {
  fclose(m_f);
}

BYTE ByteFileArray::operator[](unsigned int i) const {
  FSEEK(m_f, m_startOffset + sizeof(unsigned int) + i);
  BYTE result;
  FREAD(&result, 1, 1, m_f);
  return result;
}
