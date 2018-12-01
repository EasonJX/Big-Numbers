#pragma once

#include <ByteArray.h>

class BCDArray : public ByteArray {
public:
  BCDArray() {}
  BCDArray(const BCDArray &src) : ByteArray(src) {
  }
  // assume numstr contains a decimal representation of floating point number ("%le")
  explicit BCDArray(const String &numstr);
  String toString() const;
};

Packer &operator<<(Packer &p, const BCDArray &a);
Packer &operator>>(Packer &p,       BCDArray &a);
