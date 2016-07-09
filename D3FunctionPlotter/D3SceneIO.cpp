#include "stdafx.h"
#include <ByteFile.h>

void D3Scene::saveState(const String &fileName) const {
  save(ByteOutputFile(fileName));
  
}

void D3Scene::loadState(const String &fileName) {
  load(ByteInputFile(fileName));
}

void D3Scene::save(ByteOutputStream &s) const {
  s.putBytes(      (BYTE*)(&m_cameraPDUS      ), sizeof(m_cameraPDUS     ));
  s.putBytes(      (BYTE*)(&m_objectPDUS      ), sizeof(m_objectPDUS     ));
  s.putBytes(      (BYTE*)(&m_viewAngel       ), sizeof(m_viewAngel      ));
  s.putBytes(      (BYTE*)(&m_nearViewPlane   ), sizeof(m_nearViewPlane  ));
  s.putBytes(      (BYTE*)(&m_material        ), sizeof(m_material       ));
  s.putBytes(      (BYTE*)(&m_fillMode        ), sizeof(m_fillMode       ));
  s.putBytes(      (BYTE*)(&m_shadeMode       ), sizeof(m_shadeMode      ));
  s.putBytes(      (BYTE*)(&m_backgroundColor ), sizeof(m_backgroundColor));
}

void D3Scene::load(ByteInputStream  &s) {
  s.getBytesForced((BYTE*)(&m_cameraPDUS      ), sizeof(m_cameraPDUS     ));
  s.getBytesForced((BYTE*)(&m_objectPDUS      ), sizeof(m_objectPDUS     ));
  s.getBytesForced((BYTE*)(&m_viewAngel       ), sizeof(m_viewAngel      ));
  s.getBytesForced((BYTE*)(&m_nearViewPlane   ), sizeof(m_nearViewPlane  ));
  s.getBytesForced((BYTE*)(&m_material        ), sizeof(m_material       ));
  s.getBytesForced((BYTE*)(&m_fillMode        ), sizeof(m_fillMode       ));
  s.getBytesForced((BYTE*)(&m_shadeMode       ), sizeof(m_shadeMode      ));
  s.getBytesForced((BYTE*)(&m_backgroundColor ), sizeof(m_backgroundColor));
}
