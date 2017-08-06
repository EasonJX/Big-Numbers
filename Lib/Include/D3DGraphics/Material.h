#pragma once

#include <MyUtil.h>
#include <MFCUtil/D3DeviceFactory.h>

inline bool operator==(const D3DMATERIAL &m1, const D3DMATERIAL &m2) {
  return memcmp(&m1, &m2, sizeof(D3DMATERIAL)) == 0;
}

inline bool operator!=(const D3DMATERIAL &m1, const D3DMATERIAL &m2) {
  return !(m1 == m2);
}

// Sent to listener for id=SP_MATERIALPARAMETERS
class MATERIAL : public D3DMATERIAL {
public:
  // If < 0, material is undefined.
  int m_index;
  inline MATERIAL() : m_index(-1) {
  }
  inline bool isDefined() const {
    return m_index >= 0;
  }
  void setDefault();

  String toString(int dec=3) const;
};
