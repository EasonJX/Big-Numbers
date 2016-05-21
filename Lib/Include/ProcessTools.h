#pragma once

#include <psapi.h>
#include <windows.h>
#include "CompactArray.h"

HANDLE                  getProcessHandle(     DWORD processID = -1);
PROCESS_MEMORY_COUNTERS getProcessMemoryUsage(DWORD processID = -1); // processID == -1 gives Usage for current process

void enableTokenPrivilege(LPCTSTR privilege, bool enable);
String getProcessCommandLine(       HANDLE hProcess = NULL);
CompactArray<HMODULE> getModules(   HANDLE hProcess = NULL);
CompactArray<DWORD>   getProcessIds();

class ResourceCounters {
public:
  DWORD m_gdiObjectCount;
  DWORD m_userObjectCount;
};

ResourceCounters getProcessResources(DWORD processID = -1);          // processID == -1 gives ResourceCounters for current process

class ModuleInfo {
private:
  const String m_path;
  const DWORD  m_size;
public:
  ModuleInfo(const String &path, DWORD size) : m_path(path), m_size(size) {
  }

  const String &getPath() const {
    return m_path;
  }

  DWORD getSize() const {
    return m_size;
  }
};

class ProcessInfo {
private:
  DWORD             m_processId;
  String            m_commandLine;
  Array<ModuleInfo> m_moduleArray;
public:
  ProcessInfo(DWORD processId);

  DWORD getProcessId() const {
    return m_processId;
  }

  String getExePath() const;

  const String &getCommandLine() const {
    return m_commandLine;
  }

  const Array<ModuleInfo> &getModuleArray() const {
    return m_moduleArray;
  }
};

#define BELOW_NORMAL_PRIORITY_CLASS       0x00004000
#define ABOVE_NORMAL_PRIORITY_CLASS       0x00008000
