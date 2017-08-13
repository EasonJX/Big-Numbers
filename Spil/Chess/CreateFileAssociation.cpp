#include "stdafx.h"
#include <Registry.h>
#include "Chess.h"

void CChessApp::createFileAssociation() {
  try {
    RegistryKey root(HKEY_CLASSES_ROOT);
    RegistryKey extensionKey = root.createOrOpenPath(_T(".chs"));
    RegistryKey openWithList = extensionKey.createOrOpenKey(_T("OpenWithProgids"));
    openWithList.setValue(_T("JGMData.Chess"),String(EMPTYSTRING));
    RegistryKey handlerKey = root.createOrOpenPath(_T("JGMData.Chess"));
    RegistryKey iconKey = handlerKey.createOrOpenPath(_T("DefaultIcon"));
    const String fileName = getModuleFileName();
    iconKey.setValue(EMPTYSTRING, fileName);
    RegistryKey commandKey = handlerKey.createOrOpenPath(_T("shell\\open\\Command"));
    commandKey.setValue(EMPTYSTRING, fileName + _T(" \"%1\""));
  } catch(Exception e) {
    showException(e);
  }
}
