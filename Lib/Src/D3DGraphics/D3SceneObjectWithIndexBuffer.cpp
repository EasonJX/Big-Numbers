#include "pch.h"
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3SceneObjectWithIndexBuffer.h>

void *D3SceneObjectWithIndexBuffer::allocateIndexArray(bool int32, UINT count) {
  releaseIndexBuffer();
  m_indexBuffer = getDevice().allocateIndexBuffer(int32, count);
  void *bufferItems = NULL;
  lockIndexArray(&bufferItems);
  return bufferItems;
}

bool D3SceneObjectWithIndexBuffer::use32BitIndices(size_t vertexCount) { // static 
   return vertexCount > MAX16BITVERTEXCOUNT;
}

D3SceneObjectWithIndexBuffer &D3SceneObjectWithIndexBuffer::lockIndexArray(void **a) {
  V(m_indexBuffer->Lock(0, 0, a, 0));
  return *this;
}

D3SceneObjectWithIndexBuffer &D3SceneObjectWithIndexBuffer::unlockIndexArray() {
  V(m_indexBuffer->Unlock());
  return *this;
}

D3SceneObjectWithIndexBuffer &D3SceneObjectWithIndexBuffer::releaseIndexBuffer() {
  SAFERELEASE(m_indexBuffer)
  return *this;
}

D3Device &D3SceneObjectWithIndexBuffer::setIndices(D3Device &device) {
  return device.setIndices(m_indexBuffer);
}

#define GETLOCKEDSHORTBUFFER(count) (USHORT*)allocateIndexBuffer(false, count)
#define GETLOCKEDLONGBUFFER( count) (ULONG* )allocateIndexBuffer(true , count)

D3DINDEXBUFFER_DESC D3SceneObjectWithIndexBuffer::getDesc() const {
  D3DINDEXBUFFER_DESC desc;
  V(m_indexBuffer->GetDesc(&desc));
  return desc;
}

String D3SceneObjectWithIndexBuffer::toString() const {
  return format(_T("%s\nIndexBuffer:\n%s")
               ,__super::toString().cstr()
               ,indentString(::toString(m_indexBuffer), 2).cstr());
}
