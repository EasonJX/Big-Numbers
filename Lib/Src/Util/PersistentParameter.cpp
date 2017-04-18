#include "pch.h"
#include <PersistentParameter.h>

const TCHAR *PersistentParameter::s_defaultName = _T("Untitled");

void PersistentParameter::save(const String &fileName) {
  XMLDoc doc;
  putDataToDoc(doc);
  doc.saveToFile(fileName);
  setName(fileName);
}

void PersistentParameter::load(const String &fileName) {
  XMLDoc doc;
  doc.loadFromFile(fileName);
  getDataFromDoc(doc);
  setName(fileName);
}

void PersistentParameter::checkTag(XMLNodePtr node, const TCHAR *expectedTag) { // static
  if (_tcsicmp(node->nodeName, expectedTag) != 0) {
    throwException(_T("nodename=%s. Expected name:%s"), (TCHAR*)node->nodeName, expectedTag);
  }
}

XMLNodePtr PersistentParameter::getChild(XMLDoc &doc, XMLNodePtr n, const TCHAR *tag) { // static
  XMLNodePtr child = doc.findChild(n, tag);
  if (child == NULL) {
    throwException(_T("ChildNode with tag=\"%s\" not found in node %s"), tag, (TCHAR*)n->nodeName);
  }
  return child;
}

String PersistentParameter::getDisplayName() const {
  return FileNameSplitter(getName()).getFileName();
}
