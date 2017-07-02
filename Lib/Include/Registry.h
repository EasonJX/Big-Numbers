#pragma once

#include "MyUtil.h"
#include "Iterator.h"

#ifdef DEBUG_REGISTRY

void registryLog(const TCHAR *format, ...);

#define REGISTRYLOG(s, p) registryLog(_T("%s %lu\n"), _T(s), p)
#else
#define REGISTRYLOG(s, p)
#endif

class RegistryValue {
private:
  String  m_name;
  ULONG   m_type;
  BYTE   *m_buffer;
  ULONG   m_bufSize;  // in bytes

  void init(const String &name, ULONG type, ULONG bufSize);
  void cleanup();
  void setBuffer(const void *src);
public :
  RegistryValue(const RegistryValue &src);
  ~RegistryValue();
  RegistryValue &operator=(const RegistryValue &src);

  void reset(); // m_name = "", m_type = REG_NONE

  inline const String &getName() const {
    return m_name;
  }

  inline ULONG getType() const {
    return m_type;
  }

  inline String getTypeString() const {
    return typeAsString(m_type);
  }

  operator UINT()        const; // type must be REG_DWORD, REG_DWORD_LITTLE_ENDIAN, REG_DWORD_BIG_ENDIAN
  operator ULONG()       const; // same as int
  operator UINT64()      const; // type must be REG_QWORD
  operator String()      const; // type must be REG_EXPAND_SZ, REG_SZ
  operator StringArray() const; // type must be REG_MULTI_SZ

  inline ULONG getBufSize() const {
    return m_bufSize;
  }

  inline const BYTE *getBuffer() const {
    return m_buffer;
  }

  static String typeAsString(ULONG type);

  RegistryValue();
  RegistryValue(const String &name, const BYTE      *bytes, ULONG size, ULONG type = REG_BINARY);
  RegistryValue(const String &name, const String    &str  ,                     ULONG type = REG_SZ    );

  RegistryValue(const String &name, int    value, ULONG type = REG_DWORD );
  RegistryValue(const String &name, UINT   value, ULONG type = REG_DWORD );
  RegistryValue(const String &name, long   value, ULONG type = REG_DWORD );
  RegistryValue(const String &name, ULONG  value, ULONG type = REG_DWORD );
  RegistryValue(const String &name, INT64  value, ULONG type = REG_QWORD );
  RegistryValue(const String &name, UINT64 value, ULONG type = REG_QWORD );
  RegistryValue(const String &name, const StringArray &strings); // type = REG_MULTI_SZ
  String toString() const;
};

template <class T, class B> class ReferenceCountedType {
private :
  long m_count;
protected :
  virtual void preRelease() = 0;
  T m_obj;

  virtual ~ReferenceCountedType() {
  }

  ReferenceCountedType(const ReferenceCountedType &src);              // Restrict copying. Not implemented
  ReferenceCountedType &operator=(const ReferenceCountedType &src);   // Restrict copying. Not implemented

public:
  ReferenceCountedType(T obj) : m_obj(obj), m_count(1) {
    REGISTRYLOG("new", this);
  }

  const T &getObject() const {
    return m_obj;
  }

  B *addRef() {
    InterlockedIncrement(&m_count);
    return (B*)this;
  }

  B *release() {
    if(InterlockedDecrement(&m_count) == 0)  {
      try {
        preRelease();
      } catch (...) {
      }
      REGISTRYLOG("delete", this);
      delete this;
    }
    return NULL;
  }

  static B *copy(B *p) {
    return p ? p->addRef() : NULL;
  }
};

class ReferenceCountedRegKey : public ReferenceCountedType<HKEY, ReferenceCountedRegKey> {
private:
  ReferenceCountedRegKey(const ReferenceCountedRegKey &rhs);            // Restrict copying. Not implemented
  ReferenceCountedRegKey &operator=(const ReferenceCountedRegKey &rhs); // Restrict copying. Not implemented
protected:
  void preRelease() {
    REGISTRYLOG("close", m_obj);
    RegCloseKey(m_obj);
  }

  ~ReferenceCountedRegKey() {
  }

public:
  explicit ReferenceCountedRegKey(const HKEY key) : ReferenceCountedType<HKEY, ReferenceCountedRegKey>(key) {
  }
};

class RegistryKey {
private:
  ReferenceCountedRegKey *m_key;
  String                  m_name;
  static ReferenceCountedRegKey *newCountedKey(HKEY key, bool closeKeyOnFailure = false);
  static void checkResult(long result, const TCHAR *function, const TCHAR *regFunction);
  static void checkResult(long result, const TCHAR *function, const TCHAR *regFunction, const String &argument);
  static void createChildName(String &dst, const String &parentName, const String &name);
  friend class SubKeyIterator;
  friend class RegValueIterator;
  static const TCHAR *getRootName(HKEY key);

public:
  // Valid values for key are:
  // HKEY_CLASSES_ROOT
  // HKEY_CURRENT_USER
  // HKEY_LOCAL_MACHINE
  // HKEY_USERS
  // HKEY_PERFORMANCE_DATA
  // HKEY_CURRENT_CONFIG
  // HKEY_DYN_DATA
  explicit RegistryKey(HKEY key);
  RegistryKey(const String &remoteMachine, HKEY key);
  RegistryKey(HKEY key, const String &subKey, REGSAM samDesired = KEY_ALL_ACCESS, const String &remoteMachine=_T(""));
  RegistryKey(const RegistryKey &src);
  ~RegistryKey();

  RegistryKey &operator=(const RegistryKey &src);

  RegistryKey openKey(const String &subKey, REGSAM samDesired = KEY_ALL_ACCESS) const;

  RegistryKey createKey(        const String         &subKey
                               ,const String         &keyClass           = _T("")
                               ,ULONG                 options            = REG_OPTION_NON_VOLATILE
                               ,REGSAM                samDesired         = KEY_ALL_ACCESS
                               ,SECURITY_ATTRIBUTES  *securityAttributes = NULL) const;

  RegistryKey createOrOpenKey(  const String         &subKey
                               ,ULONG                *disposition        = NULL
                               ,const String         &keyClass           = _T("")
                               ,ULONG                 options            = REG_OPTION_NON_VOLATILE
                               ,REGSAM                samDesired         = KEY_ALL_ACCESS
                               ,SECURITY_ATTRIBUTES  *securityAttributes = NULL) const;

  RegistryKey createOrOpenPath( const String         &path
                               ,ULONG                *disposition        = NULL
                               ,const String         &keyClass           = _T("")
                               ,ULONG                 options            = REG_OPTION_NON_VOLATILE
                               ,REGSAM                samDesired         = KEY_ALL_ACCESS
                               ,SECURITY_ATTRIBUTES  *securityAttributes = NULL) const;

  void deleteKey(          const String &keyName) const;
  void deleteKeyAndSubkeys(const String &keyName) const;
  void deleteSubKeys() const;
  void deleteValue(        const String &valueName) const;
  void deleteValues() const;
  bool hasSubkey(          const String &subkey, REGSAM samDesired = KEY_ALL_ACCESS) const;

  RegistryKey connectRegistry(const String &machineName) const;

  void flushKey() const;

  const String &getName() const;
  String getClass();

  void setValue(const String &valueName, const String    &value , ULONG type = REG_SZ   ) const;
  void setValue(const String &valueName, UINT64           value , ULONG type = REG_QWORD) const;
  void setValue(const String &valueName, INT64            value , ULONG type = REG_QWORD) const;
  void setValue(const String &valueName, ULONG            value , ULONG type = REG_DWORD) const;
  void setValue(const String &valueName, long             value , ULONG type = REG_DWORD) const;
  void setValue(const String &valueName, UINT             value , ULONG type = REG_DWORD) const;
  void setValue(const String &valueName, int              value , ULONG type = REG_DWORD) const;
  void setValue(const String &valueName, USHORT           value , ULONG type = REG_DWORD) const;
  void setValue(const String &valueName, short            value , ULONG type = REG_DWORD) const;
  void setValue(const String &valueName, bool             value                         ) const;
  void setValue(const String &valueName, BYTE            *bytes , ULONG size            ) const; // type = REG_BINARY
  void setValue(const RegistryValue &value) const;

  void getValue(const String &valueName, String           &value) const;
  void getValue(const String &valueName, UINT64           &value) const;
  void getValue(const String &valueName, INT64            &value) const;
  void getValue(const String &valueName, ULONG            &value) const;
  void getValue(const String &valueName, long             &value) const;
  void getValue(const String &valueName, UINT             &value) const;
  void getValue(const String &valueName, int              &value) const;
  void getValue(const String &valueName, USHORT           &value) const;
  void getValue(const String &valueName, short            &value) const;
  void getValue(const String &valueName, bool             &value) const;
  void getValue(const String &valueName, RegistryValue    &value) const;

  String  getString(const String &name, const String    &defaultValue) const;
  INT64   getInt64( const String &name, INT64            defaultValue) const;
  UINT64  getUint64(const String &name, UINT64           defaultValue) const;
  int     getInt(   const String &name, int              defaultValue) const;
  UINT    getUint(  const String &name, UINT             defaultValue) const;
  short   getShort( const String &name, short            defaultValue) const;
  USHORT  getUshort(const String &name, USHORT           defaultValue) const;
  bool    getBool(  const String &name, bool             defaultValue) const;

  Iterator<String>        getSubKeyIterator() const;
  Iterator<RegistryValue> getValueIterator()  const;
};
